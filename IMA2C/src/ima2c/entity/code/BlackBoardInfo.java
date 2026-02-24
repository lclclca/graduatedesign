package ima2c.entity.code;

import java.io.Serializable;
import java.util.LinkedList;
import java.util.List;

import org.apache.commons.lang3.SerializationUtils;

public class BlackBoardInfo implements Serializable {

	public BlackBoardInfo() {
		super();
	}


	private String portName;


	private String id;


	private String maxNbMessage = "4";


	private String dataType;


	private String timeOut = "-1";


	private String messageAddr;

	private Boolean direction;


	private String length;

	private String status;

	private List<String> threadName = new LinkedList<String>();

	private List<String> ports = new LinkedList<String>();

	public String getPortName() {
		return portName;
	}

	public void setPortName(String portName) {
		this.portName = portName;
	}

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public String getDataType() {
		return dataType;
	}

	public void setDataType(String dataType) {
		this.dataType = dataType;
	}

	public String getTimeOut() {
		return timeOut;
	}

	public void setTimeOut(String timeOut) {
		this.timeOut = timeOut;
	}

	public String getMessageAddr() {
		return messageAddr;
	}

	public void setMessageAddr(String messageAddr) {
		this.messageAddr = messageAddr;
	}

	public Boolean getDirection() {
		return direction;
	}

	public void setDirection(Boolean direction) {
		this.direction = direction;
	}

	public String getLength() {
		return length;
	}

	public void setLength(String length) {
		this.length = length;
	}

	public String getStatus() {
		return status;
	}

	public void setStatus(String status) {
		this.status = status;
	}

	public List<String> getThreadName() {
		return threadName;
	}

	public void setThreadName(List<String> threadName) {
		this.threadName = threadName;
	}

	public List<String> getPorts() {
		return ports;
	}

	public void setPorts(List<String> ports) {
		this.ports = ports;
	}

	public String getMaxNbMessage() {
		return maxNbMessage;
	}

	public void setMaxNbMessage(String maxNbMessage) {
		this.maxNbMessage = maxNbMessage;
	}

	@Override
	public Object clone() throws CloneNotSupportedException {
		return SerializationUtils.clone(this);
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((dataType == null) ? 0 : dataType.hashCode());
		result = prime * result + ((direction == null) ? 0 : direction.hashCode());
		result = prime * result + ((id == null) ? 0 : id.hashCode());
		result = prime * result + ((length == null) ? 0 : length.hashCode());
		result = prime * result + ((maxNbMessage == null) ? 0 : maxNbMessage.hashCode());
		result = prime * result + ((messageAddr == null) ? 0 : messageAddr.hashCode());
		result = prime * result + ((portName == null) ? 0 : portName.hashCode());
		result = prime * result + ((ports == null) ? 0 : ports.hashCode());
		result = prime * result + ((status == null) ? 0 : status.hashCode());
		result = prime * result + ((threadName == null) ? 0 : threadName.hashCode());
		result = prime * result + ((timeOut == null) ? 0 : timeOut.hashCode());
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		BlackBoardInfo other = (BlackBoardInfo) obj;
		if (dataType == null) {
			if (other.dataType != null)
				return false;
		} else if (!dataType.equals(other.dataType))
			return false;
		if (direction == null) {
			if (other.direction != null)
				return false;
		} else if (!direction.equals(other.direction))
			return false;
		if (id == null) {
			if (other.id != null)
				return false;
		} else if (!id.equals(other.id))
			return false;
		if (length == null) {
			if (other.length != null)
				return false;
		} else if (!length.equals(other.length))
			return false;
		if (maxNbMessage == null) {
			if (other.maxNbMessage != null)
				return false;
		} else if (!maxNbMessage.equals(other.maxNbMessage))
			return false;
		if (messageAddr == null) {
			if (other.messageAddr != null)
				return false;
		} else if (!messageAddr.equals(other.messageAddr))
			return false;
		if (portName == null) {
			if (other.portName != null)
				return false;
		} else if (!portName.equals(other.portName))
			return false;
		if (ports == null) {
			if (other.ports != null)
				return false;
		} else if (!ports.equals(other.ports))
			return false;
		if (status == null) {
			if (other.status != null)
				return false;
		} else if (!status.equals(other.status))
			return false;
		if (threadName == null) {
			if (other.threadName != null)
				return false;
		} else if (!threadName.equals(other.threadName))
			return false;
		if (timeOut == null) {
			if (other.timeOut != null)
				return false;
		} else if (!timeOut.equals(other.timeOut))
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "BlackBoardInfo [portName=" + portName + ", id=" + id + ", maxNbMessage=" + maxNbMessage + ", dataType="
				+ dataType + ", timeOut=" + timeOut + ", messageAddr=" + messageAddr + ", direction=" + direction
				+ ", length=" + length + ", status=" + status + ", threadName=" + threadName + ", ports=" + ports + "]";
	}

}
