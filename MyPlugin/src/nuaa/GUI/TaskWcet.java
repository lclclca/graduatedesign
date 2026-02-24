package nuaa.GUI;

import org.osate.aadl2.impl.ThreadImplementationImpl;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;

import nuaa.aadlparser.TaskInfo;
import nuaa.aadlparser.Threads;

public class TaskWcet {
	public double[] taskWcet(SystemInstanceImpl systemInstanceImpl) {	
		TaskInfo parser=new TaskInfo();
		Threads thread=new Threads();
		//线程列表
		ThreadImplementationImpl[] threadArray=thread.threadArray(systemInstanceImpl);
		//线程数
		int Nt=thread.threadNum();
		//任务优先级列表
		int[] priority=parser.priority(threadArray);
		//任务执行时间列表
		double[] excutime= parser.execution(threadArray);
		//每个线程的进程编号列表
		int[] task_part= thread.taskpart();
		//每个任务按优先级执行结束的时间
		double[] wcet=new double[Nt];
		for(int i=0;i<Nt;i++) {
			double cur=0;
			for(int j=0;j<Nt;j++) {
				if(task_part[j]==task_part[i] && priority[j]>priority[i]){
					cur+=excutime[j];
                }
            }
			wcet[i]=cur+excutime[i];
			//小数点后的第二位，四舍五入
			wcet[i] = (double) Math.round(wcet[i] * 10) / 10;
		}
		return wcet;
	}		
}

