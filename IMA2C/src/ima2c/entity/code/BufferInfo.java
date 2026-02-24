package ima2c.entity.code;

import java.io.Serializable;
import java.util.LinkedList;
import java.util.List;

import org.apache.commons.lang3.SerializationUtils;

public class BufferInfo implements Serializable {

	public BufferInfo() {
		super();
	}


	private String id;


	private String portName;

	private String timeOut = "-1";


	private String dataType;


	private Boolean direction;

	private String status;


	private String length;

	private String maxNbMessage = "6";


	private String queuingDiscipline = "FIFO";

	private String messageAddr;

	private List<String> threadName = new LinkedList<String>();

	private List<String> ports = new LinkedList<String>();

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public String getPortName() {
		return portName;
	}

	public void setPortName(String portName) {
		this.portName = portName;
	}

	public String getTimeOut() {
		return timeOut;
	}

	public void setTimeOut(String timeout) {
		this.timeOut = timeout;
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

	public String getMaxNbMessage() {
		return maxNbMessage;
	}

	public void setMaxNbMessage(String maxNbMessage) {
		this.maxNbMessage = maxNbMessage;
	}

	public String getQueuingDiscipline() {
		return queuingDiscipline;
	}

	public void setQueuingDiscipline(String queuingDiscipline) {
		this.queuingDiscipline = queuingDiscipline;
	}

	public String getMessageAddr() {
		return messageAddr;
	}

	public void setMessageAddr(String messageAddr) {
		this.messageAddr = messageAddr;
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

	@Override
	public Object clone() throws CloneNotSupportedException {
		return SerializationUtils.clone(this);
	}

	@Override
	public String toString() {
		return "BufferInfo [id=" + id + ", portName=" + portName + ", timeout=" + timeOut + ", dataType=" + dataType
				+ ", direction=" + direction + ", status=" + status + ", length=" + length + ", maxNbMessage="
				+ maxNbMessage + ", queuingDiscipline=" + queuingDiscipline + ", messageAddr=" + messageAddr
				+ ", threadName=" + threadName + ", ports=" + ports + "]";
	}

}