package ima2c.exception;

public class ParameterException extends RuntimeException {

	/**
	 * @Fields serialVersionUID : TODO
	 */
	private static final long serialVersionUID = 1465465456456465L;

	public ParameterException() {

	}

	public ParameterException(String errorMessage) {
		super(errorMessage);
	}

}
