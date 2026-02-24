package ima2c.conf;

public enum SimpleDataSet {
	Array("Array"), Boolean("Boolean"), Character("Character"), Enum("Enum"), Float("Float"), Fixed("Fixed"),
	Integer("Integer"), String("String"), Struct("Struct"), Union("Union");

	private final String originalName;

	private SimpleDataSet(String originalName) {
		this.originalName = originalName;
	}

	@Override
	public String toString() {
		return this.originalName;
	}
}
