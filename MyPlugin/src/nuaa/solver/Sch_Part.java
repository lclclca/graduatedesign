package nuaa.solver;

import java.util.Arrays;

import org.chocosolver.solver.Model;
import org.chocosolver.solver.Solver;
import org.chocosolver.solver.variables.IntVar;
import org.osate.aadl2.impl.ThreadImplementationImpl;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;

import nuaa.aadlparser.TaskInfo;
import nuaa.aadlparser.Threads;

public class Sch_Part {

	 int[] Part_Sch;       
    //分区信息
	 int Num;
	 int[][] Exec;
	 int[] Period;
	 int[] pri= {2,1,3,4,5,7,6,8};
    //主时间框架MAF
	 int MAF;
	 int MIF;
	 int nFrame;
	 boolean flag;
    
    public void modelandsolve(SystemInstanceImpl systemInstanceImpl){
    	
    	TaskInfo taskinfo=new TaskInfo();
    	Threads thread=new Threads();
    	ThreadImplementationImpl[] threadArray=thread.threadArray(systemInstanceImpl);

    	//任务信息
        //任务数量
        int Nt=thread.threadNum();
        //分区数量
        int Np=thread.processNum();
        //任务-分区关系
        int[] task_part= thread.taskpart();

        //分区通信
        int[][] pmsg= {{1,0},{0,0}};
       
        //任务执行时间
        int[] exec_time=taskinfo.execution_time(threadArray);
    	//任务优先级
    	int[] task_pri=taskinfo.priority(threadArray);
    	//任务周期
    	int[] T=taskinfo.period(threadArray);
    	//任务截止时间
    	int[] Deadline=taskinfo.deadline(threadArray);
    	
    	for(int i=0;i<task_part.length;i++) {
    		System.out.print(task_part[i]);
    	}
    	
    	//分区信息
    	//分区数量
        Num=thread.processNum();
        //分区周期
        int[] period=new int[Num];
        Period=period;
        for(int i=0;i<Num;i++) {
        	int p=Integer.MAX_VALUE;
        	int cur=0;
        	for(int j=0;j<Nt;j++) {
        		if(task_part[j]==i) {
        			p=Math.min(p, T[j]);
        		}
        	}
        	Period[i]=p;
        }
        
        int maf=Arrays.stream(T).max().getAsInt();
        int mif=Arrays.stream(T).min().getAsInt();
        int nframe=maf/mif;
        MAF=maf;
        MIF=mif;
        nFrame=nframe;
        for(int i=0;i<period.length;i++) {
        	System.out.println(period[i]);
        }
        System.out.println(MAF);
        System.out.println(MIF);
        System.out.println(nFrame);
        int[][] exec=new int[nFrame][Num];
        for(int i=0;i<nFrame;i++) {
        	for(int j=0;j<Num;j++) {
        		int cur=0;
        		for(int k=0;k<Nt;k++) {
        			if(task_part[k]==j && (Period[j]*i)%T[k]==0) cur+=exec_time[k];
        		}
        		exec[i][j]=cur;
        	}
        }
        Exec=exec;
        
        Model model=new Model("Part");
        IntVar[][] pStart=model.intVarMatrix(nFrame,Num,0,MAF,false);

        int[] Sch=new int[Num*nFrame];
        Part_Sch=Sch;

        //调度生成
        for(int i=0;i<nFrame;i++){
            for(int j=0;j<Num;j++){
//                System.out.println(i*MIF%P[j][2]);
                if(i*MIF%Period[j]!=0){
                    model.arithm(pStart[i][j],"=",0).post();
                }
                else{
                    model.arithm(pStart[i][j],">=",i*MIF).post();
                    model.arithm(pStart[i][j],"<",(i+1)*Period[j]-Exec[i][j]).post();
                }
            }
        }
        //分区时间框架不重叠
        for (int i = 0; i < nFrame; i++) {
            for (int j = 0; j < Num; j++) {
                for (int k = 0; k < Num; k++) {
                    if(j==k){
                        continue;
                    }
                    if(i*MIF%Period[j]==0 && i*MIF%Period[k]==0){
                        if(pri[j]<pri[k]){
                            model.arithm(pStart[i][k],"-",pStart[i][j],">=",Exec[i][j]).post();
                        }
                        else model.arithm(pStart[i][j],"-",pStart[i][k],">=",Exec[i][k]).post();
                    }
                }
            }
        }
        //通信约束
        boolean Flag=false;
        Solver solver=model.getSolver();
        if(solver.solve()){
        	Flag=true;
            int index=0;
            for(int i=0;i<nFrame;i++) {
                for (int j = 0; j < Num; j++) {
                    System.out.printf("%d\t", pStart[i][j].getValue());
                    Part_Sch[index]=pStart[i][j].getValue();
                    index++;
                }
            }
        }else{
            System.out.println("no");
        }
        flag=Flag;
        System.out.print(flag);
    }
    public  int[] Sch(){
        return Part_Sch;
    }
    
    public  int MAF(){
        return MAF;
    }
    
    public boolean isSch() {
    	return flag;
    }
    
    public  double[][] Data(){
        double[][] data=new double[Num*nFrame][5];
//        Sch_Part part=new Sch_Part();
//        part.modelandsolve();
        int[] start= Part_Sch;

        for(int i=0;i<data.length;i++){
            data[i][0]=(i%Num)+1;
            data[i][1]=(i%Num)+1;
            data[i][2]=start[i];
            data[i][3]=start[i]+Exec[i/Num][i%Num];
            data[i][4]=(i%Num)+1;
        }
//        System.out.println(part.Sch()[0]);
        return data;
    }

}
