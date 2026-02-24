package ima2c.entity.corexml;

public class MemoryProperty {

	private Integer partitionId;

	private String partitionName;

	private String memorySize;

	private String physicalAddress;

	private String memoryType;

	private String memoryAccess;

	public MemoryProperty() {
		super();
	}

	public Integer getPartitionId() {
		return partitionId;
	}

	public void setPartitionId(Integer partitionId) {
		this.partitionId = partitionId;
	}

	public String getPartitionName() {
		return partitionName;
	}

	public void setPartitionName(String partitionName) {
		this.partitionName = partitionName;
	}

	public String getMemorySize() {
		return memorySize;
	}

	public void setMemorySize(String memorySize) {
		this.memorySize = memorySize;
	}

	public String getPhysicalAddress() {
		return physicalAddress;
	}

	public void setPhysicalAddress(String physicalAddress) {
		this.physicalAddress = physicalAddress;
	}

	public String getMemoryType() {
		return memoryType;
	}

	public void setMemoryType(String memoryType) {
		this.memoryType = memoryType;
	}

	public String getMemoryAccess() {
		return memoryAccess;
	}

	public void setMemoryAccess(String memoryaccess) {
		this.memoryAccess = memoryaccess;
	}

	@Override
	public String toString() {
		return "MemoryProperty [partitionId=" + partitionId + ", partitionName=" + partitionName + ", memorySize="
				+ memorySize + ", physicalAddress=" + physicalAddress + ", memoryType=" + memoryType + ", memoryaccess="
				+ memoryAccess + "]";
	}

}
