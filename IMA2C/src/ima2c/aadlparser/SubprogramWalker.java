package ima2c.aadlparser;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Optional;

import org.eclipse.emf.common.util.EList;
import org.osate.aadl2.Connection;
import org.osate.aadl2.ConnectionEnd;
import org.osate.aadl2.Context;
import org.osate.aadl2.Feature;
import org.osate.aadl2.contrib.aadlproject.SupportedSourceLanguages;
import org.osate.aadl2.contrib.programming.ProgrammingProperties;
import org.osate.aadl2.impl.ParameterConnectionImpl;
import org.osate.aadl2.impl.ParameterImpl;
import org.osate.aadl2.impl.PortImpl;
import org.osate.aadl2.impl.SubprogramCallImpl;
import org.osate.aadl2.impl.SubprogramTypeImpl;
import org.osate.aadl2.impl.ThreadImplementationImpl;

import ima2c.entity.code.ProgramCall;
import ima2c.exception.ParameterException;
import ima2c.utils.StringUtils;

@SuppressWarnings("all")
public class SubprogramWalker {

	public SubprogramWalker() {
		super();

	}

	public ProgramCall walk(EList<Connection> allConnections, SubprogramTypeImpl calledSubprogram) {
		ProgramCall program = new ProgramCall();
		EList<Feature> allFeatures = calledSubprogram.getAllFeatures();
		for (Feature feature : allFeatures) {
			HashMap<String, String> functionParameter = new HashMap<>();

			ParameterImpl parameterImpl = (ParameterImpl) feature;
			if (parameterImpl.isIn()) {
				
				functionParameter.put(StringUtils.convert(parameterImpl.getFullName()).toLowerCase(),
						parameterImpl.getClassifier().getName().toLowerCase());

			} else {
				
				functionParameter.put(StringUtils.convert(parameterImpl.getFullName()).toLowerCase(),
						parameterImpl.getClassifier().getName().toLowerCase() + "*");

			}
			program.getSubprogramParameters().add(functionParameter);
			// program.getThreadVariables().add(threadVariables);
		}

		Optional<List<SupportedSourceLanguages>> sourceLanguage = ProgrammingProperties
				.getSourceLanguage(calledSubprogram);
		SupportedSourceLanguages supportedSourceLanguage = sourceLanguage.get().get(0);
		program.setSourceLanguage(supportedSourceLanguage.toString());
		String sourceName = ProgrammingProperties.getSourceName(calledSubprogram).get();
		program.setCalledMethodName(sourceName);
		Optional<List<String>> sourceText = ProgrammingProperties.getSourceText(calledSubprogram);
		program.getSourceText().addAll(sourceText.get());
		program.setMethodName("subprograms_" + StringUtils.convert(calledSubprogram.getFullName()));

		return program;
	}


	public ProgramCall walk4Calls(ThreadImplementationImpl threadImpl, SubprogramTypeImpl calledSubprogram)
			throws ParameterException {
		ProgramCall program = new ProgramCall();
		
		EList<Feature> allFeatures = calledSubprogram.getAllFeatures();
	
		List<String> portList = new LinkedList<>();
		for (Feature feature : allFeatures) {
			HashMap<String, String> functionParameter = new HashMap<>();

			ParameterImpl parameterImpl = (ParameterImpl) feature;
			if (parameterImpl.isIn()) {
		
				functionParameter.put(StringUtils.convert(parameterImpl.getFullName()).toLowerCase(),
						parameterImpl.getClassifier().getName().toLowerCase());
				portList.add(StringUtils.convert(parameterImpl.getFullName()).toLowerCase());
			} else {
			
				functionParameter.put(StringUtils.convert(parameterImpl.getFullName()).toLowerCase(),
						parameterImpl.getClassifier().getName().toLowerCase() + "*");
				portList.add(StringUtils.convert(parameterImpl.getFullName()).toLowerCase());
			}
			program.getSubprogramParameters().add(functionParameter);
			// program.getThreadVariables().add(threadVariables);
		}

		EList<Connection> connections = threadImpl.getAllConnections();
	
		HashMap<String, String> threadPort = new HashMap<>();

		HashMap<String, String> thread2thread = new HashMap<>();
		threadPort.clear();
		if ((connections == null) || (connections.size() == 0)) {

		} else {

			for (Connection connection : connections) {
				ParameterConnectionImpl connectionImpl = (ParameterConnectionImpl) connection;

				Context sourceContext = connectionImpl.getSource().getContext();
				Context destinationContext = connectionImpl.getDestination().getContext();

				ConnectionEnd sourceEnd = connectionImpl.getSource().getConnectionEnd();
				ConnectionEnd destinationEnd = connectionImpl.getDestination().getConnectionEnd();
		

				HashMap<String, String> threadVariable = new HashMap<>();
				
				if (getDataTransferDirection(sourceContext, destinationContext)) {
				
					ParameterImpl parameter = (ParameterImpl) destinationEnd;
					PortImpl portImpl = (PortImpl) sourceEnd;
				
					portDirectionCheck(parameter, portImpl);
					parameterTypeCheck(parameter, portImpl);
					threadPort.put(parameter.getName().toLowerCase(), portImpl.getFullName() + "_in");
					thread2thread.put(portImpl.getFullName() + "_in",
							(portImpl.getFullName() + "_dvalue").toLowerCase());


				} else {
					
					SubprogramCallImpl subprogram = (SubprogramCallImpl) sourceContext;
					calledSubprogram = (SubprogramTypeImpl) subprogram.getCalledSubprogram();
					ParameterImpl parameter = (ParameterImpl) sourceEnd;
					PortImpl portImpl = (PortImpl) destinationEnd;
				
					portDirectionCheck(parameter, portImpl);
					parameterTypeCheck(parameter, portImpl);


					threadPort.put(parameter.getName().toLowerCase(), portImpl.getFullName() + "_out");
					thread2thread.put(portImpl.getFullName() + "_out",
							(portImpl.getFullName() + "_dvalue").toLowerCase());


				}

			}


		}

		for (String port : portList) {
			// program.getThreadVariables().add(threadPort.get(port));
			String[] strs = new String[2];
			strs[0] = threadPort.get(port).toLowerCase();
			strs[1] = thread2thread.get(threadPort.get(port)).toLowerCase();
			program.getThreadParameters().add(strs);

		}

		Optional<List<SupportedSourceLanguages>> sourceLanguage = ProgrammingProperties
				.getSourceLanguage(calledSubprogram);
		SupportedSourceLanguages supportedSourceLanguage = sourceLanguage.get().get(0);
		program.setSourceLanguage(supportedSourceLanguage.toString());

		String sourceName = ProgrammingProperties.getSourceName(calledSubprogram).get();
		program.setCalledMethodName(sourceName);

		Optional<List<String>> sourceText = ProgrammingProperties.getSourceText(calledSubprogram);
		program.getSourceText().addAll(sourceText.get());

		program.setMethodName("subprograms_" + StringUtils.convert(calledSubprogram.getFullName()));

		return program;
	}

	public boolean parameterTypeCheck(ParameterImpl parameterImpl, PortImpl portImpl) throws ParameterException {
		if ((parameterImpl.isIn() && portImpl.isIn()) || (parameterImpl.isOut() && portImpl.isOut())) {
			return true;
		} else {
			throw new ParameterException("Port directions are inconsistent!");
		}
	}

	public boolean portDirectionCheck(ParameterImpl parameterImpl, PortImpl portImpl) throws ParameterException {
		String parameterType = parameterImpl.getClassifier().getName().toLowerCase();
		String portType = portImpl.getClassifier().getName().toLowerCase();

		if (parameterType.equalsIgnoreCase(portType)
				|| DataTypeWalker.dealWithType(parameterType).equalsIgnoreCase(DataTypeWalker.dealWithType(portType))) {
			return true;
		} else {
			throw new ParameterException("Parameter type match error!");
		}
	}

	public Boolean getDataTransferDirection(Context sourceContext, Context destinationContext) {
		Boolean result = false;
		if ((sourceContext == null) && (destinationContext instanceof SubprogramCallImpl)) {
			result = true;
		}
		return result;
	}
}
