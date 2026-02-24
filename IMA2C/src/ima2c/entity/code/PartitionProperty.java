package ima2c.entity.code;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;


public class PartitionProperty {
	public PartitionProperty(String prefix) {
		this.prefix = prefix;
	}

	PartitionProperty() {
		throw new UnsupportedOperationException("TODO: auto-generated method stub");
	}

	private String prefix = "";

	private Map<String, String> datatype = new HashMap<>();

	private Integer threadNumber = null;
	private Integer queueNumber = null;
	private Integer sampleNumber = null;
	private Integer bufferNumber = null;
	private Integer blackboardNumber = null;
	private Integer eventNumber = null;
	private Integer semaphoreNumber = null;

	private String partitionName = null;

	private List<ProcessProperty> threadInfo = new LinkedList<ProcessProperty>();

	public String getPrefix() {
		return prefix;
	}

	public void setPrefix(String prefix) {
		this.prefix = prefix;
	}

	public Map<String, String> getDatatype() {
		return datatype;
	}

	public void setDatatype(Map<String, String> datatype) {
		this.datatype = datatype;
	}

	public String getPartitionName() {
		return partitionName;
	}

	public void setPartitionName(String partitionName) {
		this.partitionName = partitionName;
	}

	public List<ProcessProperty> getThreadInfo() {
		return threadInfo;
	}

	public void setThreadInfo(List<ProcessProperty> threadInfo) {
		this.threadInfo = threadInfo;
	}

	public Integer getThreadNumber() {
		return threadNumber;
	}

	public void setThreadNumber(Integer threadNumber) {
		this.threadNumber = threadNumber;
	}

	public Integer getQueueNumber() {
		return queueNumber;
	}

	public void setQueueNumber(Integer queueNumber) {
		this.queueNumber = queueNumber;
	}

	public Integer getSampleNumber() {
		return sampleNumber;
	}

	public void setSampleNumber(Integer sampleNumber) {
		this.sampleNumber = sampleNumber;
	}

	public Integer getBufferNumber() {
		return bufferNumber;
	}

	public void setBufferNumber(Integer bufferNumber) {
		this.bufferNumber = bufferNumber;
	}

	public Integer getBlackboardNumber() {
		return blackboardNumber;
	}

	public void setBlackboardNumber(Integer blackboardNumber) {
		this.blackboardNumber = blackboardNumber;
	}

	public Integer getEventNumber() {
		return eventNumber;
	}

	public void setEventNumber(Integer eventNumber) {
		this.eventNumber = eventNumber;
	}

	public Integer getSemaphoreNumber() {
		return semaphoreNumber;
	}

	public void setSemaphoreNumber(Integer semaphoreNumber) {
		this.semaphoreNumber = semaphoreNumber;
	}

	@Override
	public String toString() {
		return "PartitionProperty [prefix=" + prefix + ", datatype=" + datatype + ", threadNumber=" + threadNumber
				+ ", queueNumber=" + queueNumber + ", sampleNumber=" + sampleNumber + ", bufferNumber=" + bufferNumber
				+ ", blackboardNumber=" + blackboardNumber + ", eventNumber=" + eventNumber + ", semaphoreNumber="
				+ semaphoreNumber + ", partitionName=" + partitionName + ", threadInfo=" + threadInfo + "]";
	}

}
