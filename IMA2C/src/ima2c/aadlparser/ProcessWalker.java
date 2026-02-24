package ima2c.aadlparser;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;

import org.eclipse.emf.common.util.EList;
import org.osate.aadl2.Connection;
import org.osate.aadl2.ConnectionEnd;
import org.osate.aadl2.Context;
import org.osate.aadl2.DirectionType;
import org.osate.aadl2.Feature;
import org.osate.aadl2.PortCategory;
import org.osate.aadl2.PortConnection;
import org.osate.aadl2.ProcessSubcomponent;
import org.osate.aadl2.ThreadSubcomponent;
import org.osate.aadl2.impl.PortConnectionImpl;
import org.osate.aadl2.impl.ProcessImplementationImpl;
import org.osate.aadl2.impl.ProcessSubcomponentImpl;
import org.osate.aadl2.impl.ThreadSubcomponentImpl;
import org.osate.contribution.sei.arinc653.Arinc653;
import org.osate.contribution.sei.arinc653.SupportedDalType;

import ima2c.conf.FeatureNameConf;
import ima2c.entity.code.CommunicationProperty;
import ima2c.entity.code.ProcessProperty;
import ima2c.entity.corexml.FeatureProperty;
import ima2c.entity.corexml.PartitionAttribute;
import ima2c.entity.corexml.PortProperty;
import ima2c.exception.ParameterException;
import ima2c.utils.StringUtils;

@SuppressWarnings("all")
public class ProcessWalker {

	private ThreadWalker threadWalker;
	private FeatureWalker featureWalker;
	private ConnectionWalker connectionWalker;
	private PropertyWalker propertyWalker;

	public ProcessWalker() {
		super();
		threadWalker = new ThreadWalker();
		featureWalker = new FeatureWalker();
		propertyWalker = new PropertyWalker();
		connectionWalker = new ConnectionWalker();
	}

	public List<ProcessProperty> walk(ProcessSubcomponentImpl process) throws ParameterException {
		List<ProcessProperty> processProperties = new LinkedList<>();
		CommunicationProperty communication = connectionWalker.walk(process);
		ProcessImplementationImpl processImpl = (ProcessImplementationImpl) process.getComponentImplementation();
		EList<ThreadSubcomponent> threads = processImpl.getOwnedThreadSubcomponents();
		for (ThreadSubcomponent thread : threads) {
			ProcessProperty processProperty = threadWalker.walk(thread, communication);
			// System.out.println(processProperty);
			processProperties.add(processProperty);
		}

		return processProperties;
	}

	public PartitionAttribute getPartitionAttribute(ProcessSubcomponent processSubcomponent, AtomicInteger index) {
		ProcessSubcomponentImpl processImpl = (ProcessSubcomponentImpl) processSubcomponent;
		PartitionAttribute partitionAttribute = new PartitionAttribute();

		partitionAttribute.setPartitionIdentifier(String.valueOf(index.getAndIncrement()));
		partitionAttribute.setPartitionName(processImpl.getFullName());
		SupportedDalType dal = Arinc653.getDal(processImpl).orElse(null);
		if (dal != null) {
			partitionAttribute.setCriticality(dal.toString());
		}

		HashMap<String, FeatureProperty> partitionCommunicate = getPartitionCommunicate(processImpl);
		List<PortProperty> portProperties = new LinkedList<>();
		for (Map.Entry<String, FeatureProperty> entry : partitionCommunicate.entrySet()) {
			PortProperty portProperty = new PortProperty();
			portProperty.setPortName(entry.getKey());
			FeatureProperty value = entry.getValue();

			if (value.getPortType().equals(PortCategory.EVENT_DATA.getName())) {
				
				portProperty.setPortType(FeatureNameConf.getQueuingPort());
				portProperty.setMaxNbMessages(value.getArinc653Attribute().getQueueSize());
			} else if (value.getPortType().equals(PortCategory.DATA.getName())) {

				portProperty.setPortType(FeatureNameConf.getSamplingPort());

				portProperty
						.setRefreshRateSeconds(String.valueOf(value.getArinc653Attribute().getSamplingRefreshPeriod()));
			}


			if (value.getDirectionType().equals(DirectionType.OUT.getName())) {
				portProperty.setDirection(FeatureNameConf.getSource());
			} else if (value.getDirectionType().equals(DirectionType.IN.getName())) {
				portProperty.setDirection(FeatureNameConf.getDestination());
			}

			portProperties.add(portProperty);
		}
		partitionAttribute.setPortProperties(portProperties);
		return partitionAttribute;
	}


	public HashMap getPartitionCommunicate(ProcessSubcomponentImpl processImpl) {
		HashMap<String, FeatureProperty> partitionCommunicate = new HashMap<>();
		EList<Feature> allFeatures = processImpl.getAllFeatures();
		for (Feature feature : allFeatures) {
			HashMap<String, FeatureProperty> processFeatureContent = featureWalker.walk(feature);
			partitionCommunicate.put(feature.getName(), processFeatureContent.get(feature.getName()));
		}
		return partitionCommunicate;
	}


	public HashMap portMap(ProcessSubcomponentImpl processImpl) {
		ProcessImplementationImpl processImplementationImpl = (ProcessImplementationImpl) processImpl
				.getComponentImplementation();
		HashMap<String, String> process2ThreadPortMap = new HashMap<>();
		process2ThreadPortMap.clear();
		String processName = StringUtils.convert(processImpl.getFullName());
		for (Connection connection : processImplementationImpl.getAllConnections()) {

			if (connection instanceof PortConnection) {
				PortConnectionImpl portConnect = (PortConnectionImpl) connection;

				Context sourceContext = portConnect.getSource().getContext();
				Context destinationContext = portConnect.getDestination().getContext();

				ConnectionEnd sourceEnd = portConnect.getSource().getConnectionEnd();
				ConnectionEnd destinationEnd = portConnect.getDestination().getConnectionEnd();

				
				if (connectionWalker.communicationType(sourceContext, destinationContext)) {
					
					if (destinationContext instanceof ThreadSubcomponentImpl) {
						process2ThreadPortMap.put(destinationEnd.getName().toLowerCase() + "_in",
								processName + "_" + sourceEnd.getName().toLowerCase() + "_dvalue");
					} else {
						process2ThreadPortMap.put(sourceEnd.getName().toLowerCase() + "_out",
								processName + "_" + destinationEnd.getName().toLowerCase() + "_dvalue");
					}
				} else {

					process2ThreadPortMap.put(sourceEnd.getName().toLowerCase() + "_out",
							destinationEnd.getName().toLowerCase() + "_dvalue");
				}
			}

		}
		return process2ThreadPortMap;
	}

}
