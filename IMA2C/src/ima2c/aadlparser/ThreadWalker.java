package ima2c.aadlparser;

import static java.util.stream.Collectors.toList;

import java.util.List;
import java.util.Optional;

import org.eclipse.emf.common.util.EList;
import org.osate.aadl2.Classifier;
import org.osate.aadl2.Feature;
import org.osate.aadl2.SubprogramCall;
import org.osate.aadl2.ThreadSubcomponent;
import org.osate.aadl2.contrib.memory.MemoryProperties;
import org.osate.aadl2.contrib.programming.ProgrammingProperties;
import org.osate.aadl2.contrib.thread.ThreadProperties;
import org.osate.aadl2.contrib.timing.TimingProperties;
import org.osate.aadl2.impl.SubprogramTypeImpl;
import org.osate.aadl2.impl.ThreadImplementationImpl;
import org.osate.aadl2.impl.ThreadSubcomponentImpl;
import org.osate.contribution.sei.arinc653.Arinc653;
import org.osate.xtext.aadl2.properties.util.AadlProject;

import ima2c.entity.code.BlackBoardInfo;
import ima2c.entity.code.BufferInfo;
import ima2c.entity.code.CommunicationProperty;
import ima2c.entity.code.EventInfo;
import ima2c.entity.code.ProcessProperty;
import ima2c.entity.code.ProgramCall;
import ima2c.entity.code.QueueInfo;
import ima2c.entity.code.SampleInfo;
import ima2c.entity.code.SemaphoreInfo;
import ima2c.exception.ParameterException;
import ima2c.utils.CloneUtils;
import ima2c.utils.ConvertorUtils;
import ima2c.utils.StringUtils;

@SuppressWarnings("all")
public class ThreadWalker {

	private ConnectionWalker connectionWalker;
	private PropertyWalker propertyWalker;
	private FeatureWalker featureWalker;
	private SubprogramWalker subprogramWalker;

	public ThreadWalker() {
		super();
		featureWalker = new FeatureWalker();
		propertyWalker = new PropertyWalker();
		subprogramWalker = new SubprogramWalker();
		connectionWalker = new ConnectionWalker();
	}

	public ProcessProperty walk(ThreadSubcomponent thread, CommunicationProperty communication)
			throws ParameterException {
		ProcessProperty processProperty = new ProcessProperty();
		ThreadSubcomponentImpl threadImpl = (ThreadSubcomponentImpl) thread;
		
		String threadName = thread.getName();
		processProperty.setThreadName(StringUtils.convert(threadName));

		ThreadImplementationImpl threadImplementationImpl = (ThreadImplementationImpl) threadImpl
				.getComponentImplementation();

		switch (propertyWalker.walk(threadImpl, ThreadProperties.DISPATCH_PROTOCOL__NAME)) {
		case AadlProject.PERIODIC_LITERAL:
			processProperty.setIs_Periodic(true);
			break;
		case AadlProject.APERIODIC_LITERAL:
			processProperty.setIs_Aperiodic(true);
			break;
		case AadlProject.SPORADIC_LITERAL:
			processProperty.setIs_Sporadic(true);
			break;
		default:
			break;
		}
		processProperty.setBasePriority(propertyWalker.walk(threadImpl, ThreadProperties.PRIORITY__NAME));
		processProperty.setDeadline(propertyWalker.walk(threadImpl, TimingProperties.DEADLINE__NAME));
		processProperty.setPeriod(propertyWalker.walk(threadImpl, TimingProperties.PERIOD__NAME));
		
		processProperty.setTimeCapacity(propertyWalker.walk(threadImpl, Arinc653.TIME_CAPACITY__NAME));

		processProperty.setEntryPoint(StringUtils.convert(threadName + "_job"));
		processProperty.setStackSize(
				ConvertorUtils.convertSize(propertyWalker.walk(threadImpl, MemoryProperties.STACK_SIZE__NAME)));
		processProperty.setDeadlineType(propertyWalker.walk(threadImpl, Arinc653.DEADLINE_TYPE__NAME));

		if (propertyWalker.walk(threadImpl, ProgrammingProperties.INITIALIZE_ENTRYPOINT__NAME) != null) {
			processProperty.setHasSubprogram(true);
			Optional<Classifier> initializeEntrypoint = ProgrammingProperties.getInitializeEntrypoint(threadImpl);
			Classifier classifier = initializeEntrypoint.get();
			SubprogramTypeImpl subprogramImpl = (SubprogramTypeImpl) classifier;
			Optional<String> sourceName = ProgrammingProperties.getSourceName(subprogramImpl);
			
			ProgramCall programCall = subprogramWalker.walk(threadImplementationImpl.getAllConnections(),
					subprogramImpl);
			

			processProperty.getInitializeEntrypoint().put(threadName, programCall);
		}

		
		EList<Feature> features = threadImplementationImpl.getAllFeatures();


		EList<SubprogramCall> subprogramCalls = threadImplementationImpl.getSubprogramCalls();
		processProperty.setHasCalls(subprogramCalls.size() > 0);
		for (SubprogramCall subprogramCall : subprogramCalls) {
			SubprogramTypeImpl calledSubprogram = (SubprogramTypeImpl) subprogramCall.getCalledSubprogram();
			ProgramCall programCall = subprogramWalker.walk4Calls(threadImplementationImpl, calledSubprogram);
			// System.out.println(programCall);
			processProperty.getThreadCalls().add(programCall);

		}

	

		
		List<SampleInfo> sampleInfos = communication.getSampleInfo();
		for (SampleInfo info : sampleInfos) {
			if (info.getThreadName().contains(threadName)) {
				processProperty.setHasSample(true);
				processProperty.getSampleInfo().add(info);
			}
		}

	

		List<QueueInfo> queueInfos = communication.getQueueInfo();
		for (QueueInfo info : queueInfos) {
			if (info.getThreadName().contains(threadName)) {
				processProperty.setHasQueue(true);
				processProperty.getQueueInfo().add(info);
			}
		}
	

		
		List<BlackBoardInfo> blackBoardInfos = communication.getBlackboardInfo();
	
		for (BlackBoardInfo info : blackBoardInfos) {
			if (info.getThreadName().contains(threadName)) {
				processProperty.setHasBlackboard(true);
				BlackBoardInfo blackBoardInfo = CloneUtils.clone(info);
				
				List<String> portName = info.getPorts();
				Feature port = features.parallelStream().filter(feature -> portName.contains(feature.getFullName()))
						.collect(toList()).get(0);
				boolean direction = featureWalker.getDirection(port);
				blackBoardInfo.setDirection(direction);
			
				processProperty.getBlackboardInfo().add(blackBoardInfo);
			}
		}
		

		List<EventInfo> eventInfos = communication.getEventInfo();
		// Event Port
		for (EventInfo info : eventInfos) {
			if (info.getThreadName().contains(threadName)) {
				processProperty.setHasEvent(true);
				EventInfo eventInfo = CloneUtils.clone(info);
				List<String> portName = info.getPorts();
				Feature port = features.parallelStream().filter(feature -> portName.contains(feature.getFullName()))
						.collect(toList()).get(0);
				boolean direction = featureWalker.getDirection(port);
				eventInfo.setDirection(direction);
				processProperty.getEventInfo().add(eventInfo);
			}
		}
	

		List<BufferInfo> bufferInfos = communication.getBufferInfo();
		for (BufferInfo info : bufferInfos) {
			if (info.getThreadName().contains(threadName)) {
				processProperty.setHasBuffer(true);
				BufferInfo bufferInfo = CloneUtils.clone(info);
				List<String> portName = info.getPorts();
				Feature port = features.parallelStream().filter(feature -> portName.contains(feature.getFullName()))
						.collect(toList()).get(0);
				boolean direction = featureWalker.getDirection(port);
				bufferInfo.setDirection(direction);
				processProperty.getBufferInfo().add(bufferInfo);
			}
		}
	

		List<SemaphoreInfo> semaphoreInfos = communication.getSemaphoreInfo();
		for (SemaphoreInfo info : semaphoreInfos) {
			if (info.getThreadName().contains(threadName)) {
				processProperty.setHasSemaphore(true);
				SemaphoreInfo semaphoreInfo = CloneUtils.clone(info);
				List<String> portName = info.getPorts();
				Feature port = features.parallelStream().filter(feature -> portName.contains(feature.getFullName()))
						.collect(toList()).get(0);
				boolean direction = featureWalker.getDirection(port);
				semaphoreInfo.setDirection(direction);
			
				processProperty.getSemaphoreInfo().add(semaphoreInfo);
			}
		}
	

		return processProperty;
	}

}
