package ima2c.aadlparser;

import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Collection;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Optional;
import java.util.Set;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.stream.Collectors;

import org.eclipse.emf.common.util.EList;
import org.osate.aadl2.ContainmentPathElement;
import org.osate.aadl2.MemorySubcomponent;
import org.osate.aadl2.ProcessSubcomponent;
import org.osate.aadl2.ProcessorSubcomponent;
import org.osate.aadl2.PropertyAssociation;
import org.osate.aadl2.VirtualProcessorSubcomponent;
import org.osate.aadl2.contrib.deployment.DeploymentProperties;
import org.osate.aadl2.impl.ListValueImpl;
import org.osate.aadl2.impl.MemorySubcomponentImpl;
import org.osate.aadl2.impl.ModalPropertyValueImpl;
import org.osate.aadl2.impl.ProcessImplementationImpl;
import org.osate.aadl2.impl.ProcessSubcomponentImpl;
import org.osate.aadl2.impl.ProcessorImplementationImpl;
import org.osate.aadl2.impl.PropertyImpl;
import org.osate.aadl2.impl.ReferenceValueImpl;
import org.osate.aadl2.impl.SystemImplementationImpl;
import org.osate.aadl2.impl.VirtualProcessorSubcomponentImpl;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;
import org.osate.contribution.sei.arinc653.Arinc653;
import org.stringtemplate.v4.ST;
import org.stringtemplate.v4.STGroup;
import org.stringtemplate.v4.STGroupFile;

import ima2c.codegen.CodeGenEngine;
import ima2c.codegen.CodeGenEngineFactory;
import ima2c.conf.CodeGenConf;
import ima2c.conf.ParameterConf;
import ima2c.conf.TemplateConf;
import ima2c.entity.code.CommunicationName;
import ima2c.entity.code.CommunicationProperty;
import ima2c.entity.code.EventInfo;
import ima2c.entity.code.PartitionProperty;
import ima2c.entity.code.ProcessProperty;
import ima2c.entity.code.ProgramCall;
import ima2c.entity.corexml.Channel;
import ima2c.entity.corexml.MemoryProperty;
import ima2c.entity.corexml.PartitionAttribute;
import ima2c.entity.corexml.PartitionSchedule;
import ima2c.exception.ParameterException;
import ima2c.utils.FileUtils;
import ima2c.utils.StringUtils;

@SuppressWarnings("all")
public class SystemWalker implements AadlWalker<List, SystemInstanceImpl> {
	private ProcessWalker processWalker;
	private ConnectionWalker connectionWalker;
	private MemoryWalker memoryWalker;
	private ProcessorWalker processorWalker;
	private CodeGenEngine codeGenEngine;

	public static HashMap<String, Integer> partitionMap = new HashMap<>();

	public SystemWalker() {
		super();
		processWalker = new ProcessWalker();
		connectionWalker = new ConnectionWalker();
		memoryWalker = new MemoryWalker();
		processorWalker = new ProcessorWalker();
	}

	public static void getPartitionBinding(SystemInstanceImpl system) {
		partitionMap.clear();
		SystemImplementationImpl systemImpl = (SystemImplementationImpl) system.getComponentImplementation();
	
		EList<ProcessSubcomponent> processes = systemImpl.getOwnedProcessSubcomponents();

		AtomicInteger index = new AtomicInteger(1);
		for (ProcessSubcomponent process : processes) {
			partitionMap.put(process.getName(), index.getAndIncrement());
		}
	
	}

	public void genCode(SystemInstanceImpl system) throws Exception {
		SystemImplementationImpl systemImpl = (SystemImplementationImpl) system.getComponentImplementation();
		List<PartitionProperty> partitionProperties = walk(system);
		for (PartitionProperty partitionProperty : partitionProperties) {
			String partiitonName = partitionProperty.getPartitionName();
			List<ProcessProperty> processInfos = partitionProperty.getThreadInfo();

			String filePath = StringUtils.getFilePath(StringUtils.convert(system.getFullName()),
					StringUtils.convert(partiitonName));
		
			genGlobalsCode(filePath);
			genDeploymentCode(filePath, partitionProperty);
			genGtypesCode(filePath, (HashMap) partitionProperty.getDatatype());
			genMainCode(filePath, partitionProperty);
			genSubprogramCode(filePath, partitionProperty);
			genActivityCode(filePath, partitionProperty);
		}

		for (ProcessSubcomponent process : systemImpl.getOwnedProcessSubcomponents()) {
			CommunicationProperty communication = connectionWalker.walk((ProcessSubcomponentImpl) process);
			String filePath = StringUtils.getFilePath(system.getFullName(), process.getFullName());
			genDeploymentCode(filePath, communication);
		}

	}

	public void genAperiodicCode(String filePath, PartitionProperty partitionProperty) {

	}

	public void genActivityCode(String filePath, PartitionProperty partitionProperty) throws Exception {
		HashMap<String, List> content = new HashMap<>();

		content.put(ParameterConf.getThreadsParameter(), partitionProperty.getThreadInfo());
		this.codeGenEngine = CodeGenEngineFactory.getHCodeGenEngine(TemplateConf.getActivityHSt(), content, filePath,
				CodeGenConf.getActivityH());
		this.codeGenEngine.genCode();
		
		content.clear();

		
		StringBuilder codeContent = new StringBuilder();
		STGroup group = new STGroupFile(TemplateConf.getActivityCStg());
		ST activityHeader = group.getInstanceOf("activityHeader");
		activityHeader.add(ParameterConf.getPartitionParameter(), partitionProperty);
		
		codeContent.append(activityHeader.render());
		List<ProcessProperty> threads = partitionProperty.getThreadInfo();

		for (ProcessProperty thread : partitionProperty.getThreadInfo()) {
	
			STGroup stGroup = new STGroupFile(TemplateConf.getActivityCStg());

			ST activity = stGroup.getInstanceOf(ParameterConf.getActivityParameter());
			activity.add(ParameterConf.getThreadParameter(), thread);

			ST inport = stGroup.getInstanceOf(ParameterConf.getInportParameter());
			inport.add(ParameterConf.getThreadParameter(), thread);
			activity.add(ParameterConf.getInportParameter(), inport);

			ST outport = stGroup.getInstanceOf(ParameterConf.getOutportParameter());
			outport.add(ParameterConf.getThreadParameter(), thread);
			activity.add(ParameterConf.getOutportParameter(), outport);


			codeContent.append(activity.render());
		}
		FileUtils.createAndWriteFile(filePath, CodeGenConf.getActivityC(), codeContent.toString());

	}

	private void genSubprogramCode(String filePath, PartitionProperty partition) throws Exception {
		HashMap<String, List> content = new HashMap<>();
		
		List<ProgramCall> programCalls = partition.getThreadInfo().parallelStream()
				.map(thread -> thread.getThreadCalls()).flatMap(List::parallelStream).distinct()
				.collect(Collectors.toList());
		
		List<EventInfo> eventInfos = partition.getThreadInfo().parallelStream().map(ProcessProperty::getEventInfo)
				.flatMap(List::parallelStream).distinct().collect(Collectors.toList());
		List<ProgramCall> eventProgramCalls = eventInfos.parallelStream().map(c -> c.getComputeEntrypoint().values())
				.flatMap(Collection::stream).distinct().collect(Collectors.toList());
		programCalls.addAll(eventProgramCalls);
		
		List<ProgramCall> threadProgramCalls = partition.getThreadInfo().parallelStream()
				.map(c -> c.getInitializeEntrypoint().values()).flatMap(Collection::stream).distinct()
				.collect(Collectors.toList());
		programCalls.addAll(threadProgramCalls);
		
		programCalls = programCalls.parallelStream().distinct().collect(Collectors.toList());
		
		List<String> sources = programCalls.parallelStream().map(ProgramCall::getSourceText)
				.flatMap(List::parallelStream).map(c -> c.replace("./", "")).distinct().collect(Collectors.toList());

		for (String fileName : sources) {
			String sourcePath = CodeGenConf.getAadlFilePath() + "/" + fileName;
			String fileContent = new String(Files.readAllBytes(Paths.get(sourcePath)), Charset.defaultCharset());
			FileUtils.createAndWriteFile(filePath, fileName, fileContent);
		}
		
		if (programCalls.size() > 0) {
			sources = sources.stream().filter(str -> str.endsWith(".h")).collect(Collectors.toList());
			programCalls.get(0).setSourceText(sources);
		}

		content.put(ParameterConf.getSubprogramsParameter(), programCalls);
		this.codeGenEngine = CodeGenEngineFactory.getHCodeGenEngine(TemplateConf.getSubprogramHSt(), content, filePath,
				CodeGenConf.getSubprogramsH());
		codeGenEngine.genCode();

		this.codeGenEngine = CodeGenEngineFactory.getHCodeGenEngine(TemplateConf.getSubprogramCSt(), content, filePath,
				CodeGenConf.getSubprogramsC());
		codeGenEngine.genCode();
	}

	private void genDeploymentCode(String filePath, PartitionProperty partition) throws Exception {

		ST deploymentH = new ST(new String(FileUtils.getTemplateByStream(TemplateConf.getDeploymentHSt())));
		deploymentH.add(ParameterConf.getPartitionParameter(), partition);
		FileUtils.createAndWriteFile(filePath, CodeGenConf.getDeploymentH(), deploymentH.render());
	}


	private void genDeploymentCode(String filePath, CommunicationProperty communication) throws Exception {
		HashMap<String, List> deploymentContent = new HashMap<>();

		List<CommunicationName> names = new LinkedList<>();
		CommunicationName name = new CommunicationName();

		if (communication.getBlackboardInfo().size() != 0) {
			name.setHasBlackboard(true);
			name.setBlackboardNames(communication.getBlackboardInfo().stream().map(c -> "\"" + c.getPortName() + "\"")
					.collect(Collectors.joining(",")));
		}
		if (communication.getBufferInfo().size() != 0) {
			name.setHasBuffer(true);
			name.setBufferNames(communication.getBufferInfo().stream().map(c -> "\"" + c.getPortName() + "\"")
					.collect(Collectors.joining(",")));
		}
		if (communication.getEventInfo().size() != 0) {
			name.setHasEvent(true);
			name.setEventNames(communication.getEventInfo().stream().map(c -> "\"" + c.getPortName() + "\"")
					.collect(Collectors.joining(",")));
		}
		if (communication.getSemaphoreInfo().size() != 0) {
			name.setHasSemaphore(true);
			name.setSemaphoreNames(communication.getSemaphoreInfo().stream().map(c -> "\"" + c.getPortName() + "\"")
					.collect(Collectors.joining(",")));
		}
		names.add(name);

		deploymentContent.put(ParameterConf.getCommunicationParameter(), names);
		deploymentContent.put(ParameterConf.getBlackboardParameter(), communication.getBlackboardInfo());
		deploymentContent.put(ParameterConf.getBufferParameter(), communication.getBufferInfo());
		deploymentContent.put(ParameterConf.getEventParameter(), communication.getEventInfo());
		deploymentContent.put(ParameterConf.getSemaphoreParameter(), communication.getSemaphoreInfo());

		this.codeGenEngine = CodeGenEngineFactory.getHCodeGenEngine(TemplateConf.getDeploymentCSt(), deploymentContent,
				filePath, CodeGenConf.getDeploymentC());
		codeGenEngine.genCode();
	}

	private void genGtypesCode(String filePath, HashMap dataType) throws IOException {
		ST gtypesH = new ST(new String(FileUtils.getTemplateByStream(TemplateConf.getGtypesHSt())));
		gtypesH.add(ParameterConf.getDatatypeParameter(), dataType);
		FileUtils.createAndWriteFile(filePath, CodeGenConf.getGtypesH(), gtypesH.render());

		
		ST gtypesC = new ST(new String(FileUtils.getTemplateByStream(TemplateConf.getGtypesCSt())));
		FileUtils.createAndWriteFile(filePath, CodeGenConf.getGtypesC(), gtypesC.render());
	}

	private void genGlobalsCode(String filePath) throws Exception {
		this.codeGenEngine = CodeGenEngineFactory.getHCodeGenEngine(TemplateConf.getGlobalsHSt(), null, filePath,
				CodeGenConf.getGlobalsH());
		codeGenEngine.genCode();

		this.codeGenEngine = CodeGenEngineFactory.getHCodeGenEngine(TemplateConf.getGlobalsCSt(), null, filePath,
				CodeGenConf.getGlobalsC());
		codeGenEngine.genCode();
	}

	private void genMainCode(String filePath, PartitionProperty partition) throws Exception {
		HashMap<String, List> content = new HashMap<>();
		content.put(ParameterConf.getThreadsParameter(), partition.getThreadInfo());
		this.codeGenEngine = CodeGenEngineFactory.getHCodeGenEngine(TemplateConf.getMainCSt(), content, filePath,
				CodeGenConf.getMainC());
		this.codeGenEngine.genCode();
	}

	public void genCoreXml(SystemInstanceImpl system) throws Exception {
		HashMap<String, List> coreXmlContent = new HashMap<>();
		SystemImplementationImpl systemImpl = (SystemImplementationImpl) system.getComponentImplementation();

		HashMap<String, PartitionSchedule> bindingProperty = SystemWalker.resolveProcessorBindingProperty(systemImpl);
		List<PartitionSchedule> infos = bindingProperty.values().parallelStream().collect(Collectors.toList());
		// parse partition information
		List<PartitionAttribute> partitionPerporties = getPartitionProperty(systemImpl);
	
		for (PartitionAttribute attribute : partitionPerporties) {
			for (PartitionSchedule partition : infos) {
				if (attribute.getPartitionName().equalsIgnoreCase(partition.getPartitionName())) {
					attribute.setSystemPartition(String.valueOf(partition.isSystemPartition()));
					break;
				}
			}
		}
		coreXmlContent.put(ParameterConf.getPartitionParameter(), partitionPerporties);


		List<Channel> channels = connectionWalker.walk4communication(systemImpl.getAllConnections());

		coreXmlContent.put(ParameterConf.getConnectionParameter(), channels);


		HashMap<String, MemoryProperty> memoryProperties = resolveMemoryBindingProperty(systemImpl);
		List<MemoryProperty> memories = memoryProperties.values().parallelStream()
				.sorted(Comparator.comparing(MemoryProperty::getPartitionId)).collect(Collectors.toList());

		coreXmlContent.put(ParameterConf.getMemoryParameter(), memories);


		List<PartitionSchedule> scheduleProperties = processorWalker.walk(systemImpl);

		List<PartitionSchedule> schedules = scheduleProperties.parallelStream()
				.sorted(Comparator.comparing(PartitionSchedule::getPartitionId)).collect(Collectors.toList());
		coreXmlContent.put(ParameterConf.getScheduleParameter(), schedules);

		coreXmlContent.put(ParameterConf.getHeaderParameter(), null);
		coreXmlContent.put(ParameterConf.getEndParameter(), null);
		this.codeGenEngine = CodeGenEngineFactory.getCCodeGenEngine(new STGroupFile(TemplateConf.getCoreXmlStg()),
				coreXmlContent, CodeGenConf.getCoreXml(system.getFullName().toLowerCase()), CodeGenConf.getModuleXml(),
				ParameterConf.getCoreParameter());
		codeGenEngine.genCode();

	}

	@Override
	public List<PartitionProperty> walk(SystemInstanceImpl system) throws ParameterException {
		List<PartitionProperty> partitionProperties = new LinkedList<>();
		SystemImplementationImpl systemImpl = (SystemImplementationImpl) system.getComponentImplementation();
		// parse process component
		EList<ProcessSubcomponent> processes = systemImpl.getOwnedProcessSubcomponents();
		for (ProcessSubcomponent process : processes) {
			CommunicationProperty communication = connectionWalker.walk((ProcessSubcomponentImpl) process);
			PartitionProperty partitionProperty = new PartitionProperty(process.getFullName());
			partitionProperty.getDatatype().putAll(communication.getDatatype());
			ProcessSubcomponentImpl processImpl = (ProcessSubcomponentImpl) process;
			ProcessImplementationImpl processImplementationImpl = (ProcessImplementationImpl) processImpl
					.getComponentImplementation();

			partitionProperty.setPartitionName(process.getFullName());
			
			List<ProcessProperty> processProperties = new LinkedList<>();

			processProperties = processWalker.walk(processImpl);

		
			HashMap<String, String> process2ThreadPortMap = processWalker.portMap(processImpl);

			Set<String> threadPorts = process2ThreadPortMap.keySet();

			for (ProcessProperty thread : processProperties) {
				List<ProgramCall> threadCalls = thread.getThreadCalls();
				for (ProgramCall program : threadCalls) {
					List<String[]> parameters = program.getThreadParameters();
					for (String[] strs : parameters) {
						if (threadPorts.contains(strs[0])) {
							program.getThreadVariables().add(process2ThreadPortMap.get(strs[0]));
						} else {
							program.getThreadVariables().add(strs[1]);
						}
					}
				
				}
			}

			partitionProperty.getThreadInfo().addAll(processProperties);
			partitionProperty.setThreadNumber(processProperties.size());

			
			partitionProperty.setQueueNumber(
					communication.getQueueInfo().size() == 0 ? null : communication.getQueueInfo().size());
			partitionProperty.setSampleNumber(
					communication.getSampleInfo().size() == 0 ? null : communication.getSampleInfo().size());
			partitionProperty.setEventNumber(
					communication.getEventInfo().size() == 0 ? null : communication.getEventInfo().size());
			partitionProperty.setBufferNumber(
					communication.getBufferInfo().size() == 0 ? null : communication.getBufferInfo().size());
			partitionProperty.setBlackboardNumber(
					communication.getBlackboardInfo().size() == 0 ? null : communication.getBlackboardInfo().size());
			partitionProperty.setSemaphoreNumber(
					communication.getSemaphoreInfo().size() == 0 ? null : communication.getSemaphoreInfo().size());
			partitionProperty.getDatatype().putAll(communication.getDatatype());
			// process port <-> thread port <-> subprogram port

			partitionProperties.add(partitionProperty);
		}
		return partitionProperties;
	}

	public List<PartitionAttribute> getPartitionProperty(SystemImplementationImpl systemImpl) {
		EList<ProcessSubcomponent> processSubcomponents = systemImpl.getOwnedProcessSubcomponents();
		List<PartitionAttribute> partitionAttributes = new LinkedList<>();
	
		AtomicInteger index = new AtomicInteger(1);
		for (ProcessSubcomponent processSubcomponent : processSubcomponents) {
			PartitionAttribute partitionAttribute = processWalker.getPartitionAttribute(processSubcomponent, index);
			HashMap processorBinding = SystemWalker.resolveProcessorBindingProperty(systemImpl);
			// System.out.println(processorBinding);
			partitionAttributes.add(partitionAttribute);
		}

		return partitionAttributes;
	}

	public HashMap resolveMemoryBindingProperty(SystemImplementationImpl systemImpl) {
		ProcessSubcomponentImpl processImpl = null;
		MemorySubcomponentImpl memoryImpl = null;
		
		HashMap<String, MemoryProperty> memoryProperty = new HashMap<>();
		EList<MemorySubcomponent> memorySubcomponents = systemImpl.getOwnedMemorySubcomponents();
		for (MemorySubcomponent memorySubcomponent : memorySubcomponents) {
			HashMap<String, MemoryProperty> subMemoryProperty = memoryWalker.walk(memorySubcomponent);
			memoryProperty.putAll(subMemoryProperty);
		}

	
		AtomicInteger index = new AtomicInteger(1);
		EList<PropertyAssociation> allPropertyAssociations = systemImpl.getAllPropertyAssociations();

		for (PropertyAssociation propertyAssociation : allPropertyAssociations) {

			PropertyImpl propertyImpl = (PropertyImpl) propertyAssociation.getProperty();
			if (propertyImpl.getName().equalsIgnoreCase(DeploymentProperties.ACTUAL_MEMORY_BINDING__NAME)) {
			
				if (propertyAssociation.getAppliesTos().get(0).getContainmentPathElements().get(0)
						.getNamedElement() instanceof ProcessSubcomponentImpl) {
					processImpl = (ProcessSubcomponentImpl) propertyAssociation.getAppliesTos().get(0)
							.getContainmentPathElements().get(0).getNamedElement();
					MemoryProperty processMemoryProperty = new MemoryProperty();
					processMemoryProperty.setPartitionId(index.getAndIncrement());
					processMemoryProperty.setPartitionName(processImpl.getFullName());

					
					ModalPropertyValueImpl modalvalueImpl = (ModalPropertyValueImpl) propertyAssociation
							.getOwnedValues().get(0);
					ListValueImpl listValueImpl = (ListValueImpl) modalvalueImpl.getOwnedValue();
					ReferenceValueImpl referenceValueImpl = (ReferenceValueImpl) listValueImpl.getOwnedListElements()
							.get(0);
					EList<ContainmentPathElement> containList = referenceValueImpl.getContainmentPathElements();
					
					memoryImpl = (MemorySubcomponentImpl) containList.get(1).getNamedElement();

					MemoryProperty tmpMemoryProperty = memoryProperty.get(memoryImpl.getName());
					tmpMemoryProperty.setPartitionId(processMemoryProperty.getPartitionId());
					tmpMemoryProperty.setPartitionName(processMemoryProperty.getPartitionName());
				}

			}
		}
		return memoryProperty;

	}

	public static HashMap resolveProcessorBindingProperty(SystemImplementationImpl systemImpl) {
		HashMap<String, PartitionSchedule> bindingRelations = new HashMap<>();
		ProcessSubcomponentImpl processImpl = null;
		ProcessorSubcomponent mainProcessor = systemImpl.getOwnedProcessorSubcomponents().get(0);
		ProcessorImplementationImpl processorImpl = (ProcessorImplementationImpl) mainProcessor
				.getComponentImplementation();
		EList<VirtualProcessorSubcomponent> subProcessors = processorImpl.getOwnedVirtualProcessorSubcomponents();
		EList<PropertyAssociation> allPropertyAssociations = systemImpl.getAllPropertyAssociations();

		AtomicInteger index = new AtomicInteger(1);

		for (PropertyAssociation propertyAssociation : allPropertyAssociations) {
			HashMap<String, PartitionSchedule> bindingRelation = new HashMap<>();
			PartitionSchedule scheduleProperty = new PartitionSchedule();
		
			PropertyImpl propertyImpl = (PropertyImpl) propertyAssociation.getProperty();
			if (propertyImpl.getName().equalsIgnoreCase(DeploymentProperties.ACTUAL_PROCESSOR_BINDING__NAME)) {
				if (propertyAssociation.getAppliesTos().get(0).getContainmentPathElements().get(0)
						.getNamedElement() instanceof ProcessSubcomponentImpl) {
					processImpl = (ProcessSubcomponentImpl) propertyAssociation.getAppliesTos().get(0)
							.getContainmentPathElements().get(0).getNamedElement();
					scheduleProperty.setPartitionId(index.getAndIncrement());
					scheduleProperty.setPartitionName(processImpl.getName());

				
					ModalPropertyValueImpl modalvalueImpl = (ModalPropertyValueImpl) propertyAssociation
							.getOwnedValues().get(0);
					ListValueImpl listValueImpl = (ListValueImpl) modalvalueImpl.getOwnedValue();
					ReferenceValueImpl referenceValueImpl = (ReferenceValueImpl) listValueImpl.getOwnedListElements()
							.get(0);
					EList<ContainmentPathElement> containList = referenceValueImpl.getContainmentPathElements();
					
					VirtualProcessorSubcomponentImpl virtualProcessorImpl = (VirtualProcessorSubcomponentImpl) containList
							.get(1).getNamedElement();
					for (VirtualProcessorSubcomponent processor : subProcessors) {
						if (processor.getName().equalsIgnoreCase(virtualProcessorImpl.getName())) {
							Optional<Boolean> systemPartition = Arinc653.getSystemPartition(processor);
							scheduleProperty.setSystemPartition(systemPartition.orElse(true));
						}
					}
					bindingRelation.put(virtualProcessorImpl.getName(), scheduleProperty);
				}
				bindingRelations.putAll(bindingRelation);
			}
		}
		return bindingRelations;
	}

}