package ima2c.aadlparser;

public interface AadlWalker<RESULT, COMPONENT> {
	public RESULT walk(COMPONENT component);

}
