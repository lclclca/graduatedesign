package ima2c.log;

import org.eclipse.swt.widgets.Shell;

public class LogFactory {
	public static Log4Dialog getLog4Dialog(Shell shell, String name) {
		return new Log4Dialog(shell, name);
	}
}
