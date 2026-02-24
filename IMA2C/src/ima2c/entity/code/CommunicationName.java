package ima2c.entity.code;

public class CommunicationName {
	private Boolean hasBuffer = false;
	private Boolean hasBlackboard = false;
	private Boolean hasSemaphore = false;
	private Boolean hasEvent = false;

	private String bufferNames = null;
	private String blackboardNames = null;
	private String semaphoreNames = null;
	private String eventNames = null;

	public Boolean getHasBuffer() {
		return hasBuffer;
	}

	public void setHasBuffer(Boolean hasBuffer) {
		this.hasBuffer = hasBuffer;
	}

	public Boolean getHasBlackboard() {
		return hasBlackboard;
	}

	public void setHasBlackboard(Boolean hasBlackboard) {
		this.hasBlackboard = hasBlackboard;
	}

	public Boolean getHasSemaphore() {
		return hasSemaphore;
	}

	public void setHasSemaphore(Boolean hasSemaphore) {
		this.hasSemaphore = hasSemaphore;
	}

	public Boolean getHasEvent() {
		return hasEvent;
	}

	public void setHasEvent(Boolean hasEvent) {
		this.hasEvent = hasEvent;
	}

	public String getBufferNames() {
		return bufferNames;
	}

	public void setBufferNames(String bufferNames) {
		this.bufferNames = bufferNames;
	}

	public String getBlackboardNames() {
		return blackboardNames;
	}

	public void setBlackboardNames(String blackboardNames) {
		this.blackboardNames = blackboardNames;
	}

	public String getSemaphoreNames() {
		return semaphoreNames;
	}

	public void setSemaphoreNames(String semaphoreNames) {
		this.semaphoreNames = semaphoreNames;
	}

	public String getEventNames() {
		return eventNames;
	}

	public void setEventNames(String eventNames) {
		this.eventNames = eventNames;
	}

}
