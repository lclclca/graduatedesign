package nuaa.aadlparser;

import java.util.ArrayList;
import java.util.List;

import org.osate.aadl2.impl.MemoryImplementationImpl;
import org.osate.aadl2.impl.ProcessImplementationImpl;
import org.osate.aadl2.impl.ProcessorImplementationImpl;
import org.osate.aadl2.impl.ThreadImplementationImpl;
import org.osate.aadl2.instance.impl.ComponentInstanceImpl;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;

public class Memory {
	String memName=null;
	int memindex=0;
	
	public String[] memoryArray(SystemInstanceImpl SystemInstanceImpl) {
		List<String> memarray=new ArrayList<>();
	//取到系统中主存
	for(int i=0;i<SystemInstanceImpl.getComponentInstances().size();i++) {
		ComponentInstanceImpl Componentinstanceimpl=(ComponentInstanceImpl)SystemInstanceImpl.getComponentInstances().get(i);
		if(Componentinstanceimpl.getClassifier() instanceof MemoryImplementationImpl) {
			memName=Componentinstanceimpl.getName();
			//取到主存中各个内存
			for (int j = 0; j < Componentinstanceimpl.getComponentInstances().size(); j++) {
				ComponentInstanceImpl componentinstanceimpl=(ComponentInstanceImpl)Componentinstanceimpl.getComponentInstances().get(j);
				String coreName=componentinstanceimpl.getName();
				memarray.add(coreName);
				memindex++;
			}
		}
	}
	String[] memArray=new String[memarray.size()];
	for(int i=0;i<memArray.length;i++) {
		memArray[i]=memarray.get(i);
	}
	return memArray;
}
	public int memNum() {
		return memindex;
	}
	public String processorName() {
		return memName;
	}
	}
