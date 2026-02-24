package ima2c.entity.corexml;

public class ConnectPort {
	private String direction;
	private String portName;
	private String partitionName;
	private Integer partitionId;

	public ConnectPort() {
		super();
	}

	public ConnectPort(String direction, String portName, String partitionName) {
		super();
		this.direction = direction;
		this.portName = portName;
		this.partitionName = partitionName;
	}

	public String getDirection() {
		return direction;
	}

	public void setDirection(String direction) {
		this.direction = direction;
	}

	public String getPortName() {
		return portName;
	}

	public void setPortName(String portName) {
		this.portName = portName;
	}

	public String getPartitionName() {
		return partitionName;
	}

	public void setPartitionName(String partitionName) {
		this.partitionName = partitionName;
	}

	public Integer getPartitionId() {
		return partitionId;
	}

	public void setPartitionId(Integer partitionId) {
		this.partitionId = partitionId;
	}

	@Override
	public String toString() {
		return "ConnectPort [direction=" + direction + ", portName=" + portName + ", partitionName=" + partitionName
				+ ", partitionId=" + partitionId + "]";
	}

}
