package ima2c.conf;

public class FeatureNameConf {
	private static String DIRECTION_TYPE = "Direction_Type";
	private static String PORT_DATA_TYPE = "Data_Type";
	private static String PORT_TYPE = "Port_Type";
	private static String PORT_NAME = "Port_Name";

	private static String IN_PORT_TYPE = "In_Port";
	private static String OUT_PORT_TYPE = "Out_Port";
	private static String IN_OUT_PORT_TYPE = "In_Out_Port";

	private static String DATA_PORT = "data port";
	private static String EVENT_DATA_PORT = "event data port";
	private static String EVENT_PORT = "event port";
	private static String REQUIRES = "requires";

	private static String DATA_ACCESS = "data access";
	private static String SOURCE = "Source";

	private static String DESTINATION = "Destination";
	private static String QUEUING_PORT = "Queuing_Port";
	private static String SAMPLING_PORT = "Sampling_Port";

	public static String getQueuingPort() {
		return QUEUING_PORT;
	}

	public static String getSamplingPort() {
		return SAMPLING_PORT;
	}

	public static String getSource() {
		return SOURCE;
	}

	public static String getDestination() {
		return DESTINATION;
	}

	public static String getEventPort() {
		return EVENT_PORT;
	}

	public static String getRequires() {
		return REQUIRES;
	}

	public static String getDataAccess() {
		return DATA_ACCESS;
	}

	public static String getDirectionYype() {
		return DIRECTION_TYPE;
	}

	public static String getPortType() {
		return PORT_TYPE;
	}

	public static String getPortName() {
		return PORT_NAME;
	}

	public static String getDataPort() {
		return DATA_PORT;
	}

	public static String getEventDataPort() {
		return EVENT_DATA_PORT;
	}

	public static String getDirectionType() {
		return DIRECTION_TYPE;
	}

	public static String getPortDataType() {
		return PORT_DATA_TYPE;
	}

	public static String getInPortType() {
		return IN_PORT_TYPE;
	}

	public static String getOutPortType() {
		return OUT_PORT_TYPE;
	}

	public static String getInOutPortType() {
		return IN_OUT_PORT_TYPE;
	}

}
