package ima2c.entity.corexml;

public class WindowSchedule {

	private String partitionName;

	private Integer windowId;

	private Double windowStartSeconds;

	private Double windowDurationSeconds;

	private Boolean partitionPeriodStart;

	public WindowSchedule() {
		super();
	}

	public String getPartitionName() {
		return partitionName;
	}

	public void setPartitionName(String partitionName) {
		this.partitionName = partitionName;
	}

	public Integer getWindowId() {
		return windowId;
	}

	public void setWindowId(Integer windowId) {
		this.windowId = windowId;
	}

	public Double getWindowStartSeconds() {
		return windowStartSeconds;
	}

	public void setWindowStartSeconds(Double windowStartSeconds) {
		this.windowStartSeconds = windowStartSeconds;
	}

	public Double getWindowDurationSeconds() {
		return windowDurationSeconds;
	}

	public void setWindowDurationSeconds(Double windowDurationSeconds) {
		this.windowDurationSeconds = windowDurationSeconds;
	}

	public Boolean getPartitionPeriodStart() {
		return partitionPeriodStart;
	}

	public void setPartitionPeriodStart(Boolean partitionPeriodStart) {
		this.partitionPeriodStart = partitionPeriodStart;
	}

	@Override
	public String toString() {
		return "WindowSchedule [windowId=" + windowId + ", windowStartSeconds=" + windowStartSeconds
				+ ", windowDurationSeconds=" + windowDurationSeconds + ", partitionPeriodStart=" + partitionPeriodStart
				+ "]";
	}

}
