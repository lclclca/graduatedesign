package ima2c.entity.corexml;

public class FeatureProperty {
	private String featureName;
	private String directionType;
	private String dataType;
	private String portType;

	private ARINC653Attribute arinc653Attribute;

	public FeatureProperty() {
		super();
		arinc653Attribute = new ARINC653Attribute();
	}

	public String getFeatureName() {
		return featureName;
	}

	public void setFeatureName(String featureName) {
		this.featureName = featureName;
	}

	public String getDirectionType() {
		return directionType;
	}

	public void setDirectionType(String directionType) {
		this.directionType = directionType;
	}

	public String getDataType() {
		return dataType;
	}

	public void setDataType(String dataType) {
		this.dataType = dataType;
	}

	public String getPortType() {
		return portType;
	}

	public void setPortType(String portType) {
		this.portType = portType;
	}

	public ARINC653Attribute getArinc653Attribute() {
		return arinc653Attribute;
	}

	public void setArinc653Attribute(ARINC653Attribute arinc653Attribute) {
		this.arinc653Attribute = arinc653Attribute;
	}

	@Override
	public String toString() {
		return "FeatureProperty [featureName=" + featureName + ", directionType=" + directionType + ", dataType="
				+ dataType + ", portType=" + portType + ", arinc653Attribute=" + arinc653Attribute + "]";
	}

}
