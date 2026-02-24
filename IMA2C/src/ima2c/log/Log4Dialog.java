package ima2c.log;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Shell;

public class Log4Dialog implements Log {
	private Shell logEngine;
	private String logName;

	public Log4Dialog(Shell shell, String name) {
		logEngine = shell;
		logName = name;
	}

	@Override
	public void showLogMsg(String logMsg) {
		// TODO Auto-generated method stub
		if (this.logEngine == null || this.logName == null) {
			// Throw An Exception, TODO: zong
			System.out.println("An Internal Bus !!!!");
		}
		MessageDialog.openInformation(this.logEngine, this.logName, logMsg);

	}

	@Override
	public void setLogName(String logName) {
		// TODO Auto-generated method stub
		this.logName = logName;
	}

	@Override
	public void setOutputLogEngine(Object logEngine) {
		// TODO Auto-generated method stub
		this.logEngine = (Shell) logEngine;
	}

}
