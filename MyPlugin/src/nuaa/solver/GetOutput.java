package nuaa.solver;

import java.util.Arrays;

import org.osate.aadl2.impl.ThreadImplementationImpl;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;

import nuaa.aadlparser.Threads;

public class GetOutput {
	public String[][] taskall(SystemInstanceImpl SystemInstanceImpl){
		Allocation all=new Allocation();
		Threads thread=new Threads();
		ThreadImplementationImpl[] threadArray=thread.threadArray(SystemInstanceImpl);
		all.modelandsolve(SystemInstanceImpl);
		
		String[] taskName=thread.taskName();
		//分配矩阵
		int[][] preoutput1=all.task_core;
		String[][] outputInfo1=new String[preoutput1.length][thread.threadNum()];		
		for(int i=0;i<preoutput1.length;i++) {
			outputInfo1[i] = Arrays.stream(preoutput1[i])
		              .mapToObj(String::valueOf)
		              .toArray(String[]::new);
			}
		
		int[][] preoutput2=all.task_mc;
		String[][] outputInfo2=new String[preoutput2.length][thread.threadNum()];
		for(int i=0;i<preoutput2.length;i++) {
			outputInfo2[i] = Arrays.stream(preoutput2[i])
		              .mapToObj(String::valueOf)
		              .toArray(String[]::new);
		}
		
		String[][] outputInfo=new String[preoutput1.length+preoutput2.length+1][thread.threadNum()];
		for(int i=1;i<outputInfo.length;i++) {
			for(int j=0;j<thread.threadNum();j++) {
				if(i<outputInfo1.length+1) outputInfo[i][j]=outputInfo1[i-1][j];
				else outputInfo[i][j]=outputInfo2[i-preoutput1.length-1][j];
			}			
		}
		outputInfo[0]=taskName;
		String[][] finoutputInfo=new String[thread.threadNum()][preoutput1.length+preoutput2.length+1];
	    for (int i = 0; i < thread.threadNum(); i++) {
	        for (int j = 0; j < preoutput1.length+preoutput2.length+1; j++) {
	        	finoutputInfo[i][j]=outputInfo[j][i];
	        }
	    }
		
		return finoutputInfo;
	
	}
}
