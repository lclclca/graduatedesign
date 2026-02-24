package ima2c.exception;

public class PropertyParseException extends RuntimeException {

	/**
	 * @Fields serialVersionUID : TODO
	 */
	private static final long serialVersionUID = 6837823095507133642L;

	public PropertyParseException() {

	}

	public PropertyParseException(String errorMessage) {
		super(errorMessage);
	}
}
