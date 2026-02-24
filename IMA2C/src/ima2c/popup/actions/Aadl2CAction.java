package ima2c.popup.actions;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IObjectActionDelegate;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;
import org.osate.aadl2.impl.SystemImplementationImpl;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;
import org.osate.aadl2.presentation.Aadl2ModelEditor;

import ima2c.aadlparser.SystemWalker;
import ima2c.log.Log;
import ima2c.log.LogFactory;
import ima2c.log.LogMsg;

@SuppressWarnings("all")
public class Aadl2CAction implements IObjectActionDelegate {
	private Shell shell;
	private Log log;
	private SystemWalker systemWalker;

	public Aadl2CAction() {
		super();
		systemWalker = new SystemWalker();
	}


	@Override
	public void run(IAction action) {
		// long start = System.currentTimeMillis();


		SystemInstanceImpl system = (SystemInstanceImpl) getSelection().getFirstElement();
		SystemImplementationImpl systemImpl = (SystemImplementationImpl) system.getComponentImplementation();
		SystemWalker.getPartitionBinding(system);
		try {
			systemWalker.genCode(system);
			this.log.showLogMsg(LogMsg.EVERYTHING_OK_IN_AADL2C);
		} catch (Exception e) {
			String msg = e.getMessage();
			// System.out.println("异常信息:" + msg);
			e.printStackTrace();
			if (!msg.equals(LogMsg.INTERNAL_BUGS_IN_MODEL)) {
				this.log.showLogMsg(msg);
			} else {
				this.log.showLogMsg(LogMsg.INTERNAL_BUGS_IN_MODEL);
			}

		}
		// long end = System.currentTimeMillis();

	}

	private IStructuredSelection getSelection() {
		Aadl2ModelEditor a = (Aadl2ModelEditor) PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage()
				.getActiveEditor();
		return (IStructuredSelection) a.getSelection();
	}

	@Override
	public void selectionChanged(IAction action, ISelection selection) {

	}

	@Override
	public void setActivePart(IAction action, IWorkbenchPart targetPart) {
		shell = targetPart.getSite().getShell();
		this.log = LogFactory.getLog4Dialog(shell, "IMA2C");
	}

}
