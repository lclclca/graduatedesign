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

public class Aadl2CoreAction implements IObjectActionDelegate {
	private Shell shell;
	private Log log;
	private SystemWalker systemWalker;

	public Aadl2CoreAction() {
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
			systemWalker.genCoreXml(system);
			this.log.showLogMsg(LogMsg.EVERYTHING_OK_IN_AADL2XML);
		} catch (Exception e) {
			e.printStackTrace();
			this.log.showLogMsg(LogMsg.INTERNAL_BUGS_IN_MODEL);
		}
		// long end = System.currentTimeMillis();

		// System.out.println(system.getName()+" project execution time: " + (end -
		// start) + "ms");

	}

	@Override
	public void selectionChanged(IAction action, ISelection selection) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setActivePart(IAction action, IWorkbenchPart targetPart) {
		shell = targetPart.getSite().getShell();
		this.log = LogFactory.getLog4Dialog(shell, "DIMA2C");
	}

	private IStructuredSelection getSelection() {
		Aadl2ModelEditor a = (Aadl2ModelEditor) PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage()
				.getActiveEditor();
		return (IStructuredSelection) a.getSelection();
	}

}
