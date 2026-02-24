package ima2c.entity.corexml;

import java.util.LinkedList;
import java.util.List;

public class PartitionSchedule {

	private Integer partitionId;

	private String partitionName;

	private Double periodSeconds;

	private Double periodDurationSeconds;


	private List<WindowSchedule> windows = new LinkedList<>();


	private Double majorFrameTime;

	private boolean systemPartition;

	public PartitionSchedule() {
		super();
		systemPartition = true;
	}

	public PartitionSchedule(Integer partitionId, String partitionName, Double periodSeconds,
			Double periodDurationSeconds) {
		super();
		this.partitionId = partitionId;
		this.partitionName = partitionName;
		this.periodSeconds = periodSeconds;
		this.periodDurationSeconds = periodDurationSeconds;
	}

	public Integer getPartitionId() {
		return partitionId;
	}

	public void setPartitionId(Integer partitionId) {
		this.partitionId = partitionId;
	}

	public String getPartitionName() {
		return partitionName;
	}

	public void setPartitionName(String partitionName) {
		this.partitionName = partitionName;
	}

	public Double getPeriodSeconds() {
		return periodSeconds;
	}

	public void setPeriodSeconds(Double periodSeconds) {
		this.periodSeconds = periodSeconds;
	}

	public Double getPeriodDurationSeconds() {
		return periodDurationSeconds;
	}

	public void setPeriodDurationSeconds(Double periodDurationSeconds) {
		this.periodDurationSeconds = periodDurationSeconds;
	}

	public Double getMajorFrameTime() {
		return majorFrameTime;
	}

	public void setMajorFrameTime(Double majorFrameTime) {
		this.majorFrameTime = majorFrameTime;
	}

	public List<WindowSchedule> getWindows() {
		return windows;
	}

	public void setWindows(List<WindowSchedule> windows) {
		this.windows = windows;
	}

	public boolean isSystemPartition() {
		return systemPartition;
	}

	public void setSystemPartition(boolean systemPartition) {
		this.systemPartition = systemPartition;
	}

	@Override
	public String toString() {
		return "PartitionSchedule [partitionId=" + partitionId + ", partitionName=" + partitionName + ", periodSeconds="
				+ periodSeconds + ", periodDurationSeconds=" + periodDurationSeconds + ", windows=" + windows
				+ ", majorFrameTime=" + majorFrameTime + ", systemPartition=" + systemPartition + "]";
	}

}
