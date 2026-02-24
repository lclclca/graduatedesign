package ima2c.log;

public interface Log {
	public void showLogMsg(String logMsg);

	public void setLogName(String logName);

	public void setOutputLogEngine(Object logEngine);
}
