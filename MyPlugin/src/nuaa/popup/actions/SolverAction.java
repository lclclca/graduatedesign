package nuaa.popup.actions;


import java.util.Arrays;
import java.util.List;

import org.chocosolver.solver.variables.IntVar;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IObjectActionDelegate;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;
import org.osate.aadl2.IntegerLiteral;
import org.osate.aadl2.ModalPropertyValue;
import org.osate.aadl2.Property;
import org.osate.aadl2.PropertyAssociation;
import org.osate.aadl2.RangeValue;
import org.osate.aadl2.impl.IntegerLiteralImpl;
import org.osate.aadl2.impl.ModalPropertyValueImpl;
import org.osate.aadl2.impl.ProcessImplementationImpl;
import org.osate.aadl2.impl.ProcessorImplementationImpl;
import org.osate.aadl2.impl.PropertyAssociationImpl;
import org.osate.aadl2.impl.PropertyImpl;
import org.osate.aadl2.impl.ThreadImplementationImpl;
import org.osate.aadl2.instance.impl.ComponentInstanceImpl;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;
import org.osate.aadl2.presentation.Aadl2ModelEditor;

import nuaa.aadlparser.CoreName;
import nuaa.aadlparser.GetInput;
import nuaa.aadlparser.MemoryName;
import nuaa.GUI.MainView;
import nuaa.aadlparser.Core;
import nuaa.aadlparser.TaskInfo;
import nuaa.aadlparser.Threads;
import nuaa.solver.Allocation;



public class SolverAction implements IObjectActionDelegate {




	/**
	 * Constructor for Action1.
	 */
	public SolverAction() {
		super();
	}

	/**
	 * @see IObjectActionDelegate#setActivePart(IAction, IWorkbenchPart)

	/**
	 * @see IActionDelegate#run(IAction) 
	 */
	public void run(IAction action) {
			SystemInstanceImpl obj = (SystemInstanceImpl) getSelection().getFirstElement();	
			MainView input=new MainView(obj);
	}
	/**
	 * @see IActionDelegate#selectionChanged(IAction, ISelection)
	 */
	public void selectionChanged(IAction action, ISelection selection) {
	}

	private IStructuredSelection getSelection(){
		Aadl2ModelEditor a=(Aadl2ModelEditor) PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage().getActiveEditor();
		return (IStructuredSelection) a.getSelection(); 
	}

	@Override
	public void setActivePart(IAction arg0, IWorkbenchPart arg1) {
		// TODO Auto-generated method stub
		
	}

	
}
