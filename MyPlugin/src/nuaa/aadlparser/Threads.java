package nuaa.aadlparser;

import java.util.ArrayList;
import java.util.List;

import org.osate.aadl2.impl.ProcessImplementationImpl;
import org.osate.aadl2.impl.ThreadImplementationImpl;
import org.osate.aadl2.instance.impl.ComponentInstanceImpl;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;

public class Threads {
		int processindex=0;		//进程总数
		int threadindex=0;		//线程总数
		List<String> partname=new ArrayList<>();    //[进程名,...]
		List<String> taskname=new ArrayList<>();	//[进程名.线程名,...]
		List<Integer> taskpart=new ArrayList<>();	//[每个线程的进程编号，从0开始]
		
		
		public ThreadImplementationImpl[] threadArray(SystemInstanceImpl SystemInstanceImpl) {
			List<ThreadImplementationImpl> threadarray=new ArrayList<>();	//线程列表
			List<ComponentInstanceImpl> componentarray=new ArrayList<>();	//进程列表
		
		//取到系统中进程
		for(int i=0;i<SystemInstanceImpl.getComponentInstances().size();i++) {
			ComponentInstanceImpl componentinstanceimpl=(ComponentInstanceImpl)SystemInstanceImpl.getComponentInstances().get(i);
			if(componentinstanceimpl.getClassifier() instanceof ProcessImplementationImpl) {
				partname.add(componentinstanceimpl.getName());
				componentarray.add(componentinstanceimpl);
				processindex++;
			}
		}
		//取到进程中线程
		for(int j=0;j<processindex;j++) {
			for (int i = 0; i < componentarray.get(j).getComponentInstances().size(); i++) {
				ComponentInstanceImpl componentinstanceimpl=(ComponentInstanceImpl)componentarray.get(j).getComponentInstances().get(i);
				if(componentinstanceimpl.getClassifier() instanceof ThreadImplementationImpl) {
				ThreadImplementationImpl threadImplementImpl = (ThreadImplementationImpl) componentinstanceimpl.getClassifier();
				taskname.add(partname.get(j)+"."+componentinstanceimpl.getName());
				threadarray.add(threadImplementImpl);
				threadindex++;
				}
				taskpart.add(j);
			}
		}
		//把threadarray的线程 复制到 threadArray
		ThreadImplementationImpl[] threadArray=new ThreadImplementationImpl[threadarray.size()];
		for(int i=0;i<threadArray.length;i++) {
			threadArray[i]=threadarray.get(i);
		}

		
		return threadArray;
	}

	//获取线程数
	public int threadNum() {
		return threadindex;
	}
	//获取进程数
	public int processNum() {
		return processindex;
	}
	//获取任务名列表
	public String[] taskName() {
		String[] taskName=new String[taskname.size()];
		for(int i=0;i<taskName.length;i++) {
			taskName[i]=taskname.get(i);
		}
		return taskName;
	}

	//获取每个线程的进程编号列表
	public int[] taskpart() {
		int[] taskPart=new int[taskpart.size()];
		for(int i=0;i<taskPart.length;i++) {
			taskPart[i]=taskpart.get(i);
		}
		return taskPart;
	}
	
}
