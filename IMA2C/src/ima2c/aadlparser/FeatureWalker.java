package ima2c.aadlparser;

import java.util.HashMap;
import java.util.Optional;

import org.eclipse.emf.common.util.EList;
import org.osate.aadl2.DirectionType;
import org.osate.aadl2.Feature;
import org.osate.aadl2.PropertyAssociation;
import org.osate.aadl2.contrib.memory.AccessRights;
import org.osate.aadl2.contrib.memory.MemoryProperties;
import org.osate.aadl2.impl.DataAccessImpl;
import org.osate.aadl2.impl.DataPortImpl;
import org.osate.aadl2.impl.EventDataPortImpl;
import org.osate.aadl2.impl.EventPortImpl;
import org.osate.aadl2.impl.PortImpl;

import ima2c.conf.FeatureNameConf;
import ima2c.entity.corexml.ARINC653Attribute;
import ima2c.entity.corexml.FeatureProperty;


@SuppressWarnings("all")
public class FeatureWalker implements AadlWalker<HashMap, Feature> {

	private PortWalker portWalker;
	private HashMap<String, FeatureProperty> content;
	// private ;

	public FeatureWalker() {
		super();
		content = new HashMap<>();
		portWalker = new PortWalker();
	}

	@Override
	public HashMap walk(Feature feature) {
		content.clear();
		FeatureProperty featureProperty = new FeatureProperty();

		PortImpl port = null;
		if (feature instanceof DataAccessImpl) {
			DataAccessImpl accessImpl = (DataAccessImpl) feature;
			featureProperty.setFeatureName(accessImpl.getName());
			featureProperty.setDirectionType(FeatureNameConf.getRequires());
			featureProperty.setDataType(accessImpl.getDataFeatureClassifier().getName());
			featureProperty.setPortType(FeatureNameConf.getDataAccess());
		} else {
			port = (PortImpl) feature;
			featureProperty.setFeatureName(port.getName());
			featureProperty.setDirectionType(getPortDirection(port));
			if (port instanceof EventPortImpl) {
				featureProperty.setDataType(null);
			} else {
				featureProperty.setDataType(port.getClassifier().getName());
			}
			featureProperty.setPortType(port.getCategory().getName());

		}
		EList<PropertyAssociation> ownedPropertyAssociations = feature.getOwnedPropertyAssociations();
		if (ownedPropertyAssociations.size() > 0) {
			ARINC653Attribute attribute = portWalker.walk(feature);
			featureProperty.setArinc653Attribute(attribute);
		}

		content.put(feature.getName(), featureProperty);
		return content;
	}

	private String getPortDirection(PortImpl port) {
		String portDirection = "";

		if (port.isIn() && !port.isOut()) {
			portDirection = DirectionType.IN.getName();
		} else if (port.isOut() && !port.isIn()) {
			portDirection = DirectionType.OUT.getName();
		} else if (port.isIn() && port.isOut()) {
			portDirection = DirectionType.IN_OUT.getName();
		}

		return portDirection;
	}

	public boolean getDirection(Feature feature) {
		boolean flag = false;
		if (feature instanceof DataPortImpl) {
			DataPortImpl dataPortImpl = (DataPortImpl) feature;
			if (dataPortImpl.isIn() && !dataPortImpl.isOut()) {
				flag = true;
			} else if (dataPortImpl.isOut() && !dataPortImpl.isIn()) {
				flag = false;
			}
		} else if (feature instanceof EventPortImpl) {
			EventPortImpl eventPortImpl = (EventPortImpl) feature;
			if (eventPortImpl.isIn() && !eventPortImpl.isOut()) {
				flag = true;
			} else if (eventPortImpl.isOut() && !eventPortImpl.isIn()) {
				flag = false;
			}
		} else if (feature instanceof EventDataPortImpl) {
			EventDataPortImpl eventDataPortImpl = (EventDataPortImpl) feature;
			if (eventDataPortImpl.isIn() && !eventDataPortImpl.isOut()) {
				flag = true;
			} else if (eventDataPortImpl.isOut() && !eventDataPortImpl.isIn()) {
				flag = false;
			}
		} else {
			DataAccessImpl dataAccessImpl = (DataAccessImpl) feature;
			Optional<AccessRights> accessRight = MemoryProperties.getAccessRight(dataAccessImpl);
			if (accessRight.orElse(null) != null) {
				if (accessRight.get().equals(AccessRights.READ_ONLY)) {
					flag = true;
				} else {
					flag = false;
				}
			}
		}
		return flag;
	}

}
