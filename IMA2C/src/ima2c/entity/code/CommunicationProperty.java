package ima2c.entity.code;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

public class CommunicationProperty {

	public CommunicationProperty() {
		super();
	}

	private Map<String, String> datatype = new HashMap<>();

	private List<BlackBoardInfo> blackboardInfo = new LinkedList<BlackBoardInfo>();

	private List<EventInfo> eventInfo = new LinkedList<EventInfo>();

	private List<BufferInfo> bufferInfo = new LinkedList<BufferInfo>();

	private List<QueueInfo> queueInfo = new LinkedList<QueueInfo>();

	private List<SampleInfo> sampleInfo = new LinkedList<SampleInfo>();

	private List<SemaphoreInfo> semaphoreInfo = new LinkedList<SemaphoreInfo>();

	public Map<String, String> getDatatype() {
		return datatype;
	}

	public void setDatatype(Map<String, String> datatype) {
		this.datatype = datatype;
	}

	public List<BlackBoardInfo> getBlackboardInfo() {
		return blackboardInfo;
	}

	public void setBlackboardInfo(List<BlackBoardInfo> blackboardInfo) {
		this.blackboardInfo = blackboardInfo;
	}

	public List<EventInfo> getEventInfo() {
		return eventInfo;
	}

	public void setEventInfo(List<EventInfo> eventInfo) {
		this.eventInfo = eventInfo;
	}

	public List<BufferInfo> getBufferInfo() {
		return bufferInfo;
	}

	public void setBufferInfo(List<BufferInfo> bufferInfo) {
		this.bufferInfo = bufferInfo;
	}

	public List<QueueInfo> getQueueInfo() {
		return queueInfo;
	}

	public void setQueueInfo(List<QueueInfo> queueInfo) {
		this.queueInfo = queueInfo;
	}

	public List<SampleInfo> getSampleInfo() {
		return sampleInfo;
	}

	public void setSampleInfo(List<SampleInfo> sampleInfo) {
		this.sampleInfo = sampleInfo;
	}

	public List<SemaphoreInfo> getSemaphoreInfo() {
		return semaphoreInfo;
	}

	public void setSemaphoreInfo(List<SemaphoreInfo> semaphoreInfo) {
		this.semaphoreInfo = semaphoreInfo;
	}

}
