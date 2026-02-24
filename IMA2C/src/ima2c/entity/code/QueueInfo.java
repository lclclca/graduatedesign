package ima2c.entity.code;

import java.util.LinkedList;
import java.util.List;

public class QueueInfo {

	public QueueInfo() {
		super();
	}


	private String portName;


	private String timeOut = "-1";


	private String id;


	private String queueSize = "30";


	private String dataType;


	private Boolean direction;


	private String port_direction;


	private String queuingDiscipline = "FIFO";

	private String messageAddr;


	private String status;


	private String length;

	private List<String> ports = new LinkedList<String>();


	public List<String> threadName = new LinkedList<String>();

	public String getPortName() {
		return portName;
	}

	public void setPortName(String portName) {
		this.portName = portName;
	}

	public String getTimeOut() {
		return timeOut;
	}

	public void setTimeOut(String timeOut) {
		this.timeOut = timeOut;
	}

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public String getQueueSize() {
		return queueSize;
	}

	public void setQueueSize(String queueSize) {
		this.queueSize = queueSize;
	}

	public String getDataType() {
		return dataType;
	}

	public void setDataType(String dataType) {
		this.dataType = dataType;
	}

	public Boolean getDirection() {
		return direction;
	}

	public void setDirection(Boolean direction) {
		this.direction = direction;
	}

	public String getPort_direction() {
		return port_direction;
	}

	public void setPort_direction(String port_direction) {
		this.port_direction = port_direction;
	}

	public String getMessageAddr() {
		return messageAddr;
	}

	public void setMessageAddr(String messageAddr) {
		this.messageAddr = messageAddr;
	}

	public String getStatus() {
		return status;
	}

	public void setStatus(String status) {
		this.status = status;
	}

	public String getLength() {
		return length;
	}

	public void setLength(String length) {
		this.length = length;
	}

	public List<String> getThreadName() {
		return threadName;
	}

	public void setThreadName(List<String> threadName) {
		this.threadName = threadName;
	}

	public String getQueuingDiscipline() {
		return queuingDiscipline;
	}

	public void setQueuingDiscipline(String queuingDiscipline) {
		this.queuingDiscipline = queuingDiscipline;
	}

	public List<String> getPorts() {
		return ports;
	}

	public void setPorts(List<String> ports) {
		this.ports = ports;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((dataType == null) ? 0 : dataType.hashCode());
		result = prime * result + ((direction == null) ? 0 : direction.hashCode());
		result = prime * result + ((id == null) ? 0 : id.hashCode());
		result = prime * result + ((length == null) ? 0 : length.hashCode());
		result = prime * result + ((messageAddr == null) ? 0 : messageAddr.hashCode());
		result = prime * result + ((portName == null) ? 0 : portName.hashCode());
		result = prime * result + ((port_direction == null) ? 0 : port_direction.hashCode());
		result = prime * result + ((ports == null) ? 0 : ports.hashCode());
		result = prime * result + ((queueSize == null) ? 0 : queueSize.hashCode());
		result = prime * result + ((queuingDiscipline == null) ? 0 : queuingDiscipline.hashCode());
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
		QueueInfo other = (QueueInfo) obj;
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
		if (port_direction == null) {
			if (other.port_direction != null)
				return false;
		} else if (!port_direction.equals(other.port_direction))
			return false;
		if (ports == null) {
			if (other.ports != null)
				return false;
		} else if (!ports.equals(other.ports))
			return false;
		if (queueSize == null) {
			if (other.queueSize != null)
				return false;
		} else if (!queueSize.equals(other.queueSize))
			return false;
		if (queuingDiscipline == null) {
			if (other.queuingDiscipline != null)
				return false;
		} else if (!queuingDiscipline.equals(other.queuingDiscipline))
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
		return "QueueInfo [portName=" + portName + ", timeOut=" + timeOut + ", id=" + id + ", queueSize=" + queueSize
				+ ", dataType=" + dataType + ", direction=" + direction + ", port_direction=" + port_direction
				+ ", queuingDiscipline=" + queuingDiscipline + ", messageAddr=" + messageAddr + ", status=" + status
				+ ", length=" + length + ", ports=" + ports + ", threadName=" + threadName + "]";
	}

}