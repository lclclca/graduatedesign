package ima2c.entity.code;

import java.util.LinkedList;
import java.util.List;

public class SampleInfo {

	public SampleInfo() {
		super();
	}


	private String id;


	private String refresh = "100";


	private String dataType;

	private Boolean direction;

	private String port_direction;


	private String portName;


	private String messageAddr = null;


	private String validityType = null;


	private String length = null;


	private String status = null;

	private List<String> ports = new LinkedList<String>();

	private List<String> threadName = new LinkedList<String>();

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public String getRefresh() {
		return refresh;
	}

	public void setRefresh(String refresh) {
		this.refresh = refresh;
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

	public String getPortName() {
		return portName;
	}

	public void setPortName(String portName) {
		this.portName = portName;
	}

	public String getMessageAddr() {
		return messageAddr;
	}

	public void setMessageAddr(String messageAddr) {
		this.messageAddr = messageAddr;
	}

	public String getValidityType() {
		return validityType;
	}

	public void setValidityType(String validityType) {
		this.validityType = validityType;
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
		result = prime * result + ((refresh == null) ? 0 : refresh.hashCode());
		result = prime * result + ((status == null) ? 0 : status.hashCode());
		result = prime * result + ((threadName == null) ? 0 : threadName.hashCode());
		result = prime * result + ((validityType == null) ? 0 : validityType.hashCode());
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
		SampleInfo other = (SampleInfo) obj;
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
		if (refresh == null) {
			if (other.refresh != null)
				return false;
		} else if (!refresh.equals(other.refresh))
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
		if (validityType == null) {
			if (other.validityType != null)
				return false;
		} else if (!validityType.equals(other.validityType))
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "SampleInfo [id=" + id + ", refresh=" + refresh + ", dataType=" + dataType + ", direction=" + direction
				+ ", port_direction=" + port_direction + ", portName=" + portName + ", messageAddr=" + messageAddr
				+ ", validityType=" + validityType + ", length=" + length + ", status=" + status + ", ports=" + ports
				+ ", threadName=" + threadName + "]";
	}

}
