package ima2c.aadlparser;

import java.util.Optional;
import java.util.OptionalLong;

import org.eclipse.emf.common.util.EList;
import org.osate.aadl2.Feature;
import org.osate.aadl2.PropertyAssociation;
import org.osate.aadl2.contrib.aadlproject.TimeUnits;
import org.osate.aadl2.contrib.communication.CommunicationProperties;
import org.osate.aadl2.contrib.programming.ProgrammingProperties;
import org.osate.aadl2.impl.PropertyImpl;
import org.osate.contribution.sei.arinc653.Arinc653;
import org.osate.contribution.sei.arinc653.QueueingDisciplineType;
import org.osate.pluginsupport.properties.IntegerWithUnits;

import ima2c.entity.corexml.ARINC653Attribute;
import ima2c.exception.ParameterException;
import ima2c.utils.ConvertorUtils;

public class PortWalker implements AadlWalker<ARINC653Attribute, Feature> {

	public PortWalker() {
		super();
	}


	@Override
	public ARINC653Attribute walk(Feature feature) {
		ARINC653Attribute attribute = new ARINC653Attribute();
		EList<PropertyAssociation> propertyList = feature.getOwnedPropertyAssociations();
		for (PropertyAssociation propertyAssociation : propertyList) {
			PropertyImpl tmpPropertyImpl = (PropertyImpl) propertyAssociation.getProperty();
			try {
				switch (tmpPropertyImpl.getName()) {
				case "Timeout":
					Optional<IntegerWithUnits<TimeUnits>> timeout = Arinc653.getTimeout(feature);
					attribute.setTimeOut(String.valueOf(String.valueOf(timeout.get())));
					break;
				case "Queueing_Discipline":
					Optional<QueueingDisciplineType> queueingDiscipline = Arinc653.getQueueingDiscipline(feature);
					attribute.setQueueingDiscipline(String.valueOf(queueingDiscipline.get()));
					break;
				case "Queue_Size":
					OptionalLong queueSize = CommunicationProperties.getQueueSize(feature);
					attribute.setQueueSize(String.valueOf(queueSize.getAsLong()));
					break;
				case "Sampling_Refresh_Period":
					Optional<IntegerWithUnits<TimeUnits>> samplingRefreshPeriod = Arinc653
							.getSamplingRefreshPeriod(feature);
					attribute.setSamplingRefreshPeriod(String.valueOf(
							ConvertorUtils.convertTime(String.valueOf(samplingRefreshPeriod.get().getValue()))));
					break;
				case "Source_Name":
					Optional<String> sourceName = ProgrammingProperties.getSourceName(feature);
					attribute.setSourceName(sourceName.get());
					break;
				default:
					break;
				}

			} catch (Exception e) {
				// e.printStackTrace();
				System.out.println("An exception occurred in port attribute parsing!");
				throw new ParameterException("An exception occurred in port attribute parsing!");

			}
		}
		return attribute;
	}
}
