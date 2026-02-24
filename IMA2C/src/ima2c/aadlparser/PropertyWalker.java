package ima2c.aadlparser;

import java.util.List;
import java.util.Optional;
import java.util.OptionalLong;

import org.osate.aadl2.Classifier;
import org.osate.aadl2.NamedElement;
import org.osate.aadl2.contrib.aadlproject.SizeUnits;
import org.osate.aadl2.contrib.aadlproject.SupportedDispatchProtocols;
import org.osate.aadl2.contrib.aadlproject.SupportedSourceLanguages;
import org.osate.aadl2.contrib.aadlproject.TimeUnits;
import org.osate.aadl2.contrib.communication.CommunicationProperties;
import org.osate.aadl2.contrib.memory.MemoryProperties;
import org.osate.aadl2.contrib.programming.ProgrammingProperties;
import org.osate.aadl2.contrib.thread.ThreadProperties;
import org.osate.aadl2.contrib.timing.TimingProperties;
import org.osate.aadl2.impl.SubprogramTypeImpl;
import org.osate.contribution.sei.arinc653.Arinc653;
import org.osate.contribution.sei.arinc653.DeadlineType;
import org.osate.contribution.sei.arinc653.QueueingDisciplineType;
import org.osate.pluginsupport.properties.IntegerRangeWithUnits;
import org.osate.pluginsupport.properties.IntegerWithUnits;

import ima2c.exception.ParameterException;
import ima2c.utils.ConvertorUtils;

@SuppressWarnings("all")
public class PropertyWalker {
	public PropertyWalker() {
		super();
	}

	public String walk(NamedElement component, String propertyName) {
		String result = null;

		try {
			switch (propertyName) {

			case TimingProperties.PERIOD__NAME:
				Optional<IntegerWithUnits<TimeUnits>> period = TimingProperties.getPeriod(component);
				result = ConvertorUtils.convertTime(String.valueOf(period.orElse(null)));
				break;
			case TimingProperties.DEADLINE__NAME:
				Optional<IntegerWithUnits<TimeUnits>> deadline = TimingProperties.getDeadline(component);
				result = ConvertorUtils.convertTime(String.valueOf(deadline.orElse(null)));
				break;
			case TimingProperties.DISPATCH_OFFSET__NAME:
				Optional<IntegerWithUnits<TimeUnits>> dispatchOffset = TimingProperties.getDispatchOffset(component);
				result = String.valueOf(dispatchOffset.orElse(null));
				break;
			case ThreadProperties.DISPATCH_PROTOCOL__NAME:
				Optional<SupportedDispatchProtocols> dispatchProtocol = ThreadProperties.getDispatchProtocol(component);
				result = String.valueOf(dispatchProtocol.orElse(null));
				break;
			case ThreadProperties.PRIORITY__NAME:
				OptionalLong priority = ThreadProperties.getPriority(component);
				result = String.valueOf(priority.getAsLong());
				break;
			case MemoryProperties.STACK_SIZE__NAME:
				Optional<IntegerWithUnits<SizeUnits>> stackSize = MemoryProperties.getStackSize(component);
				result = String.valueOf(stackSize.orElse(null));
				break;
			case TimingProperties.COMPUTE_EXECUTION_TIME__NAME:
				Optional<IntegerRangeWithUnits<TimeUnits>> computeExecutionTime = TimingProperties
						.getComputeExecutionTime(component);
				result = ConvertorUtils.convertTime(String.valueOf(computeExecutionTime.get().getMaximum()));
				break;
			case ProgrammingProperties.INITIALIZE_ENTRYPOINT__NAME:
				Optional<Classifier> initializeEntrypoint = ProgrammingProperties.getInitializeEntrypoint(component);
				Classifier classifier = initializeEntrypoint.orElse(null);
				if (classifier == null) {
					return null;
				} else if (classifier instanceof SubprogramTypeImpl) {
					SubprogramTypeImpl typeImpl = (SubprogramTypeImpl) classifier;
					result = typeImpl.getName();
				}
				break;
			case Arinc653.TIME_CAPACITY__NAME:
				Optional<IntegerWithUnits<TimeUnits>> timeCapacity = Arinc653.getTimeCapacity(component);
				result = ConvertorUtils.convertTime(String.valueOf(timeCapacity.orElse(null)));
				break;
			case Arinc653.DEADLINE_TYPE__NAME:
				Optional<DeadlineType> deadlineType = Arinc653.getDeadlineType(component);
				result = deadlineType.orElse(DeadlineType.SOFT).toString().toUpperCase();
				break;
			/* subprogram componnet property */
			case ProgrammingProperties.SOURCE_LANGUAGE__NAME:
				Optional<List<SupportedSourceLanguages>> sourceLanguage = ProgrammingProperties
						.getSourceLanguage(component);
				SupportedSourceLanguages supportedSourceLanguages = sourceLanguage.get().get(0);
				result = supportedSourceLanguages.toString();
				break;
			case ProgrammingProperties.SOURCE_NAME__NAME:
				Optional<String> sourceName = ProgrammingProperties.getSourceName(component);
				result = sourceName.get();
				break;
			case ProgrammingProperties.SOURCE_TEXT__NAME:
				Optional<List<String>> sourceText = ProgrammingProperties.getSourceText(component);
				result = sourceText.get().get(0);
				break;

			case "Timeout":
				Optional<IntegerWithUnits<TimeUnits>> timeout = Arinc653.getTimeout(component);
				result = ConvertorUtils.convertTime(String.valueOf(timeout.orElse(null)));
				break;
			case "Queueing_Discipline":
				Optional<QueueingDisciplineType> queueingDiscipline = Arinc653.getQueueingDiscipline(component);
				if (queueingDiscipline.orElse(null) != null) {
					if (queueingDiscipline.get().toString().equals("By_Priority")) {
						result = "PRIORITY";
					}
				} else {
					result = String
							.valueOf(queueingDiscipline.orElse(QueueingDisciplineType.FIFO).toString().toUpperCase());
				}
				break;
			case "Queue_Size":
				OptionalLong queueSize = CommunicationProperties.getQueueSize(component);
				
				result = String.valueOf(queueSize.orElse(4l));
				break;
			case "Sampling_Refresh_Period":
				Optional<IntegerWithUnits<TimeUnits>> samplingRefreshPeriod = Arinc653
						.getSamplingRefreshPeriod(component);
				result = String.valueOf(samplingRefreshPeriod.or(null));
				break;
			default:
				break;
			}
		} catch (Exception e) {
			// e.printStackTrace();
			System.out.println("Componnet property parsing exception!");
			throw new ParameterException("Componnet property parsing exception!");
		}
		return result;
	}

}
