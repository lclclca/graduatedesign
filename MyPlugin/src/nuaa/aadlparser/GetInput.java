package nuaa.aadlparser;

import java.util.Arrays;

import org.osate.aadl2.impl.ThreadImplementationImpl;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;

import nuaa.GUI.TaskWcet;

public class GetInput {
	public String[][] inputInfo(SystemInstanceImpl systemInstanceImpl) {
	TaskInfo parser=new TaskInfo();
	Threads thread=new Threads();
	ThreadImplementationImpl[] threadArray=thread.threadArray(systemInstanceImpl);
	int Nt=thread.threadNum();
	int[] priority=parser.priority(threadArray);
	double[] excutime= parser.execution(threadArray);
	int[] period=parser.period(threadArray);
	int[] deadline=parser.deadline(threadArray);
	TaskWcet tw=new TaskWcet();
	double[] wcet=tw.taskWcet(systemInstanceImpl);
	//将int型数组转换为String数组，方便输出在窗口上
     String strArray1[] = thread.taskName();
     String strArray2[] = Arrays.stream(excutime)
             .mapToObj(String::valueOf)
             .toArray(String[]::new);
     String strArray3[] = Arrays.stream(period)
             .mapToObj(String::valueOf)
             .toArray(String[]::new);
     String strArray4[] = Arrays.stream(deadline)
             .mapToObj(String::valueOf)
             .toArray(String[]::new);
     String strArray5[] = Arrays.stream(priority)
             .mapToObj(String::valueOf)
             .toArray(String[]::new);
     String strArray6[] = Arrays.stream(wcet)
             .mapToObj(String::valueOf)
             .toArray(String[]::new);
    //进行矩阵的转置
	String[][] preinput=new String[][]{strArray1,strArray2,strArray3,strArray4,strArray5,strArray6};
	String[][] inputInfo=new String[Nt][6];
    for (int i = 0; i < Nt; i++) {
        for (int j = 0; j < 6; j++) {
            inputInfo[i][j]=preinput[j][i];
        }
    }
	return inputInfo;
}
	}
