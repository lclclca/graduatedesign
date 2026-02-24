package ima2c.entity.corexml;

public class ARINC653Attribute {
	/**
	 * 默认取4
	 */
	private String queueSize;
	/**
	 * 排队规则
	 */
	private String queueingDiscipline;
	private String timeOut;
	private String samplingRefreshPeriod;
	private String sourceName;

	public ARINC653Attribute() {
		super();
		this.queueSize = "30";
		this.queueingDiscipline = "FIFO";
	}

	public ARINC653Attribute(String queueSize, String queueingDiscipline, String timeOut, String samplingRefreshPeriod,
			String sourceName) {
		super();
		this.queueSize = queueSize;
		this.queueingDiscipline = queueingDiscipline;
		this.timeOut = timeOut;
		this.samplingRefreshPeriod = samplingRefreshPeriod;
		this.sourceName = sourceName;
	}

	public String getQueueSize() {
		return queueSize;
	}

	public void setQueueSize(String queueSize) {
		this.queueSize = queueSize;
	}

	public String getQueueingDiscipline() {
		return queueingDiscipline;
	}

	public void setQueueingDiscipline(String queueingDiscipline) {
		this.queueingDiscipline = queueingDiscipline;
	}

	public String getTimeOut() {
		return timeOut;
	}

	public void setTimeOut(String timeOut) {
		this.timeOut = timeOut;
	}

	public String getSamplingRefreshPeriod() {
		return samplingRefreshPeriod;
	}

	public void setSamplingRefreshPeriod(String samplingRefreshPeriod) {
		this.samplingRefreshPeriod = samplingRefreshPeriod;
	}

	public String getSourceName() {
		return sourceName;
	}

	public void setSourceName(String sourceName) {
		this.sourceName = sourceName;
	}

	@Override
	public String toString() {
		return "ARINC653Attribute [queueSize=" + queueSize + ", queueingDiscipline=" + queueingDiscipline + ", timeOut="
				+ timeOut + ", samplingRefreshPeriod=" + samplingRefreshPeriod + ", sourceName=" + sourceName + "]";
	}

}
