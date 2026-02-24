package nuaa.aadlparser;

import org.osate.aadl2.instance.impl.SystemInstanceImpl;

public class MemoryName {
	public String[] memoryName(SystemInstanceImpl SystemInstanceImpl) {
		Memory mem=new Memory();
		String[] memory=mem.memoryArray(SystemInstanceImpl);
		String[] memName=new String[mem.memindex];
		for(int i=0;i<mem.memindex;i++) {
			memName[i]=mem.memName+"."+memory[i];
		}
		return memName;
		}
}
