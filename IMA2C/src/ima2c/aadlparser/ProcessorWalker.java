package ima2c.aadlparser;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;

import org.eclipse.emf.common.util.EList;
import org.osate.aadl2.BasicPropertyAssociation;
import org.osate.aadl2.ProcessorSubcomponent;
import org.osate.aadl2.PropertyAssociation;
import org.osate.aadl2.PropertyExpression;
import org.osate.aadl2.UnitLiteral;
import org.osate.aadl2.contrib.aadlproject.TimeUnits;
import org.osate.aadl2.impl.BooleanLiteralImpl;
import org.osate.aadl2.impl.IntegerLiteralImpl;
import org.osate.aadl2.impl.ListValueImpl;
import org.osate.aadl2.impl.ModalPropertyValueImpl;
import org.osate.aadl2.impl.ProcessorImplementationImpl;
import org.osate.aadl2.impl.PropertyAssociationImpl;
import org.osate.aadl2.impl.PropertyImpl;
import org.osate.aadl2.impl.RecordValueImpl;
import org.osate.aadl2.impl.ReferenceValueImpl;
import org.osate.aadl2.impl.SystemImplementationImpl;
import org.osate.aadl2.impl.VirtualProcessorSubcomponentImpl;
import org.osate.contribution.sei.arinc653.Arinc653;
import org.osate.pluginsupport.properties.IntegerWithUnits;
import org.osate.xtext.aadl2.properties.util.PropertyUtils;

import ima2c.entity.corexml.ARINC653ScheduleWindow;
import ima2c.entity.corexml.PartitionSchedule;
import ima2c.entity.corexml.WindowSchedule;
import ima2c.exception.ParameterException;
import ima2c.utils.ConvertorUtils;

@SuppressWarnings("all")
public class ProcessorWalker implements AadlWalker<List, SystemImplementationImpl> {

	@Override
	public List<PartitionSchedule> walk(SystemImplementationImpl systemImpl) {
		EList<ProcessorSubcomponent> processorSubcomponents = systemImpl.getOwnedProcessorSubcomponents();
		List<PartitionSchedule> scheduleProperties = new LinkedList<>();

		HashMap<String, PartitionSchedule> bindingRelations = SystemWalker.resolveProcessorBindingProperty(systemImpl);
		for (ProcessorSubcomponent processorSubcomponent : processorSubcomponents) {
			scheduleProperties = walk4processor(processorSubcomponent, bindingRelations);
		}
		return scheduleProperties;
	}

	public List<PartitionSchedule> walk4processor(ProcessorSubcomponent processorSubcomponent,
			HashMap<String, PartitionSchedule> bindingRelations) {
		List<PartitionSchedule> scheduleProperties = new LinkedList<>();

		ProcessorImplementationImpl processorImpl = (ProcessorImplementationImpl) processorSubcomponent
				.getComponentImplementation();

		Optional<IntegerWithUnits<TimeUnits>> moduleMajorFrame = Arinc653.getModuleMajorFrame(processorImpl);
		String mainFrame = String.valueOf(moduleMajorFrame.orElse(null));
		
		EList<PropertyAssociation> processorProperties = processorImpl.getOwnedPropertyAssociations();

		List<ARINC653ScheduleWindow> scheduleWindows = new LinkedList<>();

		for (PropertyAssociation propertyAssociation : processorProperties) {
			PropertyAssociationImpl tmpPropertyAssociationImpl = (PropertyAssociationImpl) propertyAssociation;
			PropertyImpl tmpPropertyImpl = (PropertyImpl) tmpPropertyAssociationImpl.getProperty();
			ModalPropertyValueImpl modalvalueImpl = (ModalPropertyValueImpl) tmpPropertyAssociationImpl.getOwnedValues()
					.get(0);
			try {
				if (tmpPropertyImpl.getName().equalsIgnoreCase("Module_Schedule")) {
					scheduleWindows = resolveModuleSchedule(modalvalueImpl);
				}
			} catch (Exception e) {
				// TODO Auto-generated catch block
				System.out.println("An exception occurred in the attribute parsing of the processor component!");
				throw new ParameterException(
						"An exception occurred in the attribute parsing of the processor component!");
				// e.printStackTrace();
			}
		}
		List<WindowSchedule> windowSchedules = new LinkedList<>();
		// 填充WindowSchedule
		for (ARINC653ScheduleWindow scheduleWindow : scheduleWindows) {
			WindowSchedule window = new WindowSchedule();
			window.setPartitionName(scheduleWindow.getPartition());
			List<Double> data = windowSchedules.stream().map(WindowSchedule::getWindowDurationSeconds)
					.collect(Collectors.toList());
	
			window.setWindowStartSeconds(ConvertorUtils.addDouble(data));
			window.setWindowDurationSeconds(ConvertorUtils.timeConvert(scheduleWindow.getDuration()));
			window.setPartitionPeriodStart(scheduleWindow.getPeriodicProcessingStart());
			windowSchedules.add(window);
		}

		for (String key : bindingRelations.keySet()) {
			for (WindowSchedule window : windowSchedules) {
				if (key.equalsIgnoreCase(window.getPartitionName())) {
					bindingRelations.get(key).getWindows().add(window);
				}
			}
		}


		for (String key : bindingRelations.keySet()) {
			PartitionSchedule schedule = bindingRelations.get(key);
			schedule.setMajorFrameTime(ConvertorUtils.timeConvert(mainFrame));
			schedule.setPeriodSeconds(ConvertorUtils.timeConvert(mainFrame));
			schedule.setPeriodDurationSeconds(schedule.getWindows().get(0).getWindowDurationSeconds());

			for (int i = 0; i < schedule.getWindows().size(); i++) {
				schedule.getWindows().get(i).setWindowId(Integer.valueOf(schedule.getPartitionId()) * 100 + i + 1);
			}
			scheduleProperties.add(schedule);
		}

		return scheduleProperties;

	}

	public List<ARINC653ScheduleWindow> resolveModuleSchedule(ModalPropertyValueImpl modalvalueImpl) {
	
		List<ARINC653ScheduleWindow> schedules = new LinkedList<ARINC653ScheduleWindow>();

		ListValueImpl listValueImpl = (ListValueImpl) modalvalueImpl.getOwnedValue();
		EList<PropertyExpression> moduleSchedules = listValueImpl.getOwnedListElements();

		for (PropertyExpression propertyExpression : moduleSchedules) {

			RecordValueImpl recordValueImpl = (RecordValueImpl) propertyExpression;

			EList<BasicPropertyAssociation> containsValueList = recordValueImpl.getOwnedFieldValues();
			ARINC653ScheduleWindow scheduleWindow = new ARINC653ScheduleWindow();

			ReferenceValueImpl partition = (ReferenceValueImpl) PropertyUtils.getRecordFieldValue(recordValueImpl,
					"Partition");
			IntegerLiteralImpl duration = (IntegerLiteralImpl) PropertyUtils.getRecordFieldValue(recordValueImpl,
					"Duration");
			BooleanLiteralImpl periodic_Processing_Start = (BooleanLiteralImpl) PropertyUtils
					.getRecordFieldValue(recordValueImpl, "Periodic_Processing_Start");
			VirtualProcessorSubcomponentImpl referencePart = (VirtualProcessorSubcomponentImpl) partition
					.getContainmentPathElements().get(0).getNamedElement();
			String partitionValue = referencePart.getName();
			scheduleWindow.setPartition(partitionValue);
			String durationoValue = duration.getValue() + duration.getUnit().getName();
			scheduleWindow.setDuration(durationoValue);
			scheduleWindow.setPeriodicProcessingStart(periodic_Processing_Start.getValue());

			schedules.add(scheduleWindow);
		}
		return schedules;
	}

	public String resolveModuleMajorFrame(ModalPropertyValueImpl modalvalueImpl) {
		IntegerLiteralImpl integerLiteralImpl = (IntegerLiteralImpl) modalvalueImpl.getOwnedValue();
		UnitLiteral unit = integerLiteralImpl.getUnit();
		String unitName = unit.getName();
		String moduleMajorFrame = String.valueOf(Float.valueOf(integerLiteralImpl.getValue())) + unitName;
		return moduleMajorFrame;
	}

}
