package nuaa.aadlparser;

import org.osate.aadl2.instance.impl.SystemInstanceImpl;

public class CoreName {
	
	public String[] coreName(SystemInstanceImpl SystemInstanceImpl) {
		Core pro=new Core();
		String[] core=pro.coreArray(SystemInstanceImpl);
		String[] coreName=new String[pro.coreindex];
		for(int i=0;i<pro.coreindex;i++) {
			coreName[i]=pro.processorName()+"."+core[i];
		}
		return coreName;
		}
		
	public String[] core(SystemInstanceImpl SystemInstanceImpl) {
		Core pro=new Core();
		String[] core=pro.coreArray(SystemInstanceImpl);
		return core;
		}
		
}
