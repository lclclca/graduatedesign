package ima2c.entity.corexml;

import java.util.List;

public class PartitionAttribute {

	private String partitionIdentifier;

	private String partitionName;

	private String criticality;

	private String systemPartition;

	private String entryPoint;

	private List<PortProperty> portProperties;

	public PartitionAttribute() {
		criticality = "LEVEL_A";
		systemPartition = "true";
		entryPoint = "entryPoint";
	}

	public PartitionAttribute(String partitionIdentifier, String partitionName, String criticality,
			String systemPartition, String entryPoint) {
		this.partitionIdentifier = partitionIdentifier;
		this.partitionName = partitionName;
		this.criticality = criticality;
		this.systemPartition = systemPartition;
		this.entryPoint = entryPoint;
	}

	public PartitionAttribute(String partitionIdentifier, String partitionName, String criticality,
			String systemPartition, String entryPoint, List<PortProperty> portProperties) {
		this.partitionIdentifier = partitionIdentifier;
		this.partitionName = partitionName;
		this.criticality = criticality;
		this.systemPartition = systemPartition;
		this.entryPoint = entryPoint;
		this.portProperties = portProperties;
	}

	public String getPartitionIdentifier() {
		return partitionIdentifier;
	}

	public void setPartitionIdentifier(String partitionIdentifier) {
		this.partitionIdentifier = partitionIdentifier;
	}

	public String getPartitionName() {
		return partitionName;
	}

	public void setPartitionName(String partitionName) {
		this.partitionName = partitionName;
	}

	public String getCriticality() {
		return criticality;
	}

	public void setCriticality(String criticality) {
		this.criticality = criticality;
	}

	public String getSystemPartition() {
		return systemPartition;
	}

	public void setSystemPartition(String systemPartition) {
		this.systemPartition = systemPartition;
	}

	public String getEntryPoint() {
		return entryPoint;
	}

	public void setEntryPoint(String entryPoint) {
		this.entryPoint = entryPoint;
	}

	public List<PortProperty> getPortProperties() {
		return portProperties;
	}

	public void setPortProperties(List<PortProperty> portProperties) {
		this.portProperties = portProperties;
	}

	@Override
	public String toString() {
		return "PartitionPerporty [partitionIdentifier=" + partitionIdentifier + ", partitionName=" + partitionName
				+ ", criticality=" + criticality + ", systemPartition=" + systemPartition + ", entryPoint=" + entryPoint
				+ ", portProperties=" + portProperties + "]";
	}

}
