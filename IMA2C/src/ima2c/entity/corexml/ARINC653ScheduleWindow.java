package ima2c.entity.corexml;

public class ARINC653ScheduleWindow {
	private String partition;
	private String duration;
	private Boolean periodicProcessingStart;

	public ARINC653ScheduleWindow() {
		super();
	}

	public ARINC653ScheduleWindow(String partition, String duration, Boolean periodicProcessingStart) {
		super();
		this.partition = partition;
		this.duration = duration;
		this.periodicProcessingStart = periodicProcessingStart;
	}

	public String getPartition() {
		return partition;
	}

	public void setPartition(String partition) {
		this.partition = partition;
	}

	public String getDuration() {
		return duration;
	}

	public void setDuration(String duration) {
		this.duration = duration;
	}

	public Boolean getPeriodicProcessingStart() {
		return periodicProcessingStart;
	}

	public void setPeriodicProcessingStart(Boolean periodicProcessingStart) {
		this.periodicProcessingStart = periodicProcessingStart;
	}

	@Override
	public String toString() {
		return "ScheduleWindow [partition=" + partition + ", duration=" + duration + ", Periodic_Processing_Start="
				+ periodicProcessingStart + "]";
	}

}
