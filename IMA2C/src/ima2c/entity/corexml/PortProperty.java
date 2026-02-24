package ima2c.entity.corexml;

public class PortProperty {
	
	private String portType;

	private String portName;

	private String maxMessageSize;


	private String direction;


	private String maxNbMessages;


	private String refreshRateSeconds;

	public PortProperty() {
		maxMessageSize = "30";
	}

	public PortProperty(String portName, String portType, String maxMessageSize, String direction, String maxNbMessages,
			String refreshRateSeconds) {
		this.portName = portName;
		this.portType = portType;
		this.maxMessageSize = maxMessageSize;
		this.direction = direction;
		this.maxNbMessages = maxNbMessages;
		this.refreshRateSeconds = refreshRateSeconds;
	}

	public String getPortType() {
		return portType;
	}

	public void setPortType(String portType) {
		this.portType = portType;
	}

	public String getMaxMessageSize() {
		return maxMessageSize;
	}

	public void setMaxMessageSize(String maxMessageSize) {
		this.maxMessageSize = maxMessageSize;
	}

	public String getDirection() {
		return direction;
	}

	public void setDirection(String direction) {
		this.direction = direction;
	}

	public String getMaxNbMessages() {
		return maxNbMessages;
	}

	public void setMaxNbMessages(String maxNbMessages) {
		this.maxNbMessages = maxNbMessages;
	}

	public String getRefreshRateSeconds() {
		return refreshRateSeconds;
	}

	public void setRefreshRateSeconds(String refreshRateSeconds) {
		this.refreshRateSeconds = refreshRateSeconds;
	}

	public String getPortName() {
		return portName;
	}

	public void setPortName(String portName) {
		this.portName = portName;
	}

	@Override
	public String toString() {
		return "PortProperty [portType=" + portType + ", portName=" + portName + ", maxMessageSize=" + maxMessageSize
				+ ", direction=" + direction + ", maxNbMessages=" + maxNbMessages + ", refreshRateSeconds="
				+ refreshRateSeconds + "]";
	}

}
