package ima2c.entity.code;

import java.io.Serializable;
import java.util.LinkedList;
import java.util.List;

/**
 * @Description: Semaphore通信信息,自带方向
 * @author LSX
 */
public class SemaphoreInfo implements Serializable {

	public SemaphoreInfo() {
		super();
	}


	private String id;



	private String dataType;


	private String timeOut = "-1";


	private String initCounter = "0";

	private String maxCounter = "6";


	private String queuingDiscipline = "FIFO";


	private String status;


	private Boolean direction;


	private String portName;

	private List<String> ports = new LinkedList<String>();

	private List<String> threadName = new LinkedList<String>();

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

	public String getInitCounter() {
		return initCounter;
	}

	public void setInitCounter(String initCounter) {
		initCounter = initCounter;
	}

	public String getMaxCounter() {
		return maxCounter;
	}

	public void setMaxCounter(String maxCounter) {
		maxCounter = maxCounter;
	}

	public String getQueuingDiscipline() {
		return queuingDiscipline;
	}

	public void setQueuingDiscipline(String queuingDiscipline) {
		this.queuingDiscipline = queuingDiscipline;
	}

	public String getStatus() {
		return status;
	}

	public void setStatus(String status) {
		this.status = status;
	}

	public Boolean getDirection() {
		return direction;
	}

	public void setDirection(Boolean direction) {
		this.direction = direction;
	}

	public String getPortName() {
		return portName;
	}

	public void setPortName(String portName) {
		this.portName = portName;
	}

	public List<String> getPorts() {
		return ports;
	}

	public void setPorts(List<String> ports) {
		this.ports = ports;
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
		result = prime * result + ((initCounter == null) ? 0 : initCounter.hashCode());
		result = prime * result + ((maxCounter == null) ? 0 : maxCounter.hashCode());
		result = prime * result + ((portName == null) ? 0 : portName.hashCode());
		result = prime * result + ((ports == null) ? 0 : ports.hashCode());
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
		SemaphoreInfo other = (SemaphoreInfo) obj;
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
		if (initCounter == null) {
			if (other.initCounter != null)
				return false;
		} else if (!initCounter.equals(other.initCounter))
			return false;
		if (maxCounter == null) {
			if (other.maxCounter != null)
				return false;
		} else if (!maxCounter.equals(other.maxCounter))
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
		return "SemaphoreInfo [id=" + id + ", dataType=" + dataType + ", timeOut=" + timeOut + ", initCounter="
				+ initCounter + ", maxCounter=" + maxCounter + ", queuingDiscipline=" + queuingDiscipline + ", status="
				+ status + ", direction=" + direction + ", portName=" + portName + ", ports=" + ports + ", threadName="
				+ threadName + "]";
	}

}
