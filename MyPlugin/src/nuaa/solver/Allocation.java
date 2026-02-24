package nuaa.solver;

import org.chocosolver.solver.Model;
import org.chocosolver.solver.Solver;
import org.chocosolver.solver.search.loop.monitors.IMonitorSolution;
import org.chocosolver.solver.search.strategy.Search;
import org.chocosolver.solver.variables.IntVar;
import org.osate.aadl2.impl.ThreadImplementationImpl;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;

import nuaa.aadlparser.Core;
import nuaa.aadlparser.Memory;
import nuaa.aadlparser.TaskInfo;
import nuaa.aadlparser.Threads;

import java.util.Arrays;

public class Allocation { 
    int[][] task_core;
    int[][] task_mc;
    boolean flag;
    public void modelandsolve(SystemInstanceImpl systemInstanceImpl){
    	TaskInfo taskinfo=new TaskInfo();
    	Threads thread=new Threads();
    	ThreadImplementationImpl[] threadArray=thread.threadArray(systemInstanceImpl);
    	Core core=new Core();
    	core.coreArray(systemInstanceImpl);
    	Memory memory=new Memory();
    	memory.memoryArray(systemInstanceImpl);
    	
    	//核的数量
        int Nc=core.coreNum();
        //内存管理器数量
        int Nmc=memory.memNum();
        //分区数量
        int Np=thread.processNum();
        //任务数量
        int Nt=thread.threadNum();
        //内存管理器内存
        int[] Mc_size= {1000,1000};
        //任务占用内存
        int[] task_size=taskinfo.tasksize(threadArray);
        //任务-分区关系
        int[] task_part= thread.taskpart();
        int[][] part=new int[Np][Nt];
        for(int i=0;i<Np;i++) {
        	for(int j=0;j<Nt;j++) {
        		if(task_part[j]==i) part[i][j]=1;
        		else part[i][j]=0;
        	}
        }
        
        //任务之间通信关系
//        int[][] msg= {{0,1,0,0,0},
//        			  {0,0,0,0,0},
//        			  {0,0,0,0,0},
//        			  {0,0,0,0,0},
//        			  {0,0,0,1,0}};


        //任务执行时间
    	int[] exec_time=taskinfo.execution_time(threadArray);
    	//任务优先级
    	int[] task_pri=taskinfo.priority(threadArray);
    	//任务周期
    	int[] T=taskinfo.period(threadArray);
    	//任务截止时间
    	int[] Deadline=taskinfo.deadline(threadArray);
    	
        
        
        //准备输出矩阵
    	int[][] Na=new int[Nc][Nt];
    	int[][] T2mc=new int[Nmc][Nt];
    	task_core=Na;
    	task_mc=T2mc;

        //创建模型实例
        Model model=new Model("allocation");

        //定义变量
        IntVar[][] na=model.intVarMatrix(Nc,Nt,0,1);
        IntVar[][] t2mc=model.intVarMatrix(Nmc,Nt,0,1);
       
        //wcet变量
        IntVar[] wcet=model.intVarArray(Nt,0,1600);
        //周期
        IntVar[] t=model.intVarArray(Nt,T);
        //wcet计算公式累加部分,下标表示每一个任务
        IntVar[] temp=model.intVarArray(Nt,0,1600);
        //wcet除部分
        IntVar[][] nums=model.intVarMatrix(Nt,Nt,0,10);


        
        //约束
        //WCET约束
        //构建wcet公式
        WcetCount.wcet(Nt,task_part,task_pri,exec_time,model,wcet,t,nums,temp);

        
        //wcet小于截止时间
        for (int i = 0; i < Nt ; i++) {
            model.arithm(wcet[i],"<", Deadline[i]).post();
        }
        
        
        //分配约束
        int[] cs=new int[Nc];
        for (int i = 0; i < Nc; i++) {
            cs[i]=1;
            }
        for (int i = 0; i <Nt ; i++) {
            model.scalar(transposed(model,na,Nt,Nc)[i],cs,"=",1).post();
        }

//        int[] cs2=new int[Nmc];
//        for (int i = 0; i < Nmc; i++) {
//            cs2[i]=1;
//        }
//        for (int i = 0; i <Nt ; i++) {
//            model.scalar(transposed(model,t2mc,Nt,Nmc)[i],cs2,">=",1).post();
//        }

//        for (int i = 0; i < Nmc; i++) {
//            model.scalar(t2mc[i],part[0],">=",1).post();
//        }
//        for (int i = 0; i < Nmc; i++) {
//            model.scalar(t2mc[i],part[1],">=",1).post();
//        }
//        for (int i = 0; i < Nmc; i++) {
//            model.scalar(t2mc[i],task_size,"<=",Mc_size[i]).post();
//        }
        
//通信约束
//	        for(int i=0;i<Nt;i++) {
//	        	for(int j=0;j<Nt;j++) {
//	        		if(msg[i][j]==1)
//	        		model.scalar;
//	        	}
//	        }
        boolean Flag=false;

        //求解
        Solver solver = model.getSolver();
        
                if(solver.solve()){
//                    solver.printStatistics();
                	Flag=true;
                    System.out.print("\ntask-core allocation\n");
                    for (int i = 0; i < Nc; i++) {
                        for (int j = 0; j < Nt; j++) {
                        	task_core[i][j]=na[i][j].getValue();
                            System.out.printf("%d",na[i][j].getValue());
                            System.out.printf("\t");
                        }
                        System.out.printf("\n");
                    }
                    System.out.print("task-mc allocation\n");
                    for (int i = 0; i < Nmc; i++) {
                        for (int j = 0; j < Nt; j++) {
                        	task_mc[i][j]=t2mc[i][j].getValue();
                            System.out.printf("%d",t2mc[i][j].getValue());
                            System.out.printf("\t");
                        }
                        System.out.printf("\n");
                    }
        }else {
            System.out.println("The solver has proved the problem has no solution");
        }
                flag=Flag;
    }

    //矩阵转置
    public IntVar[][] transposed(Model model,IntVar[][] a,int row,int column){
        IntVar[][] b=model.intVarMatrix(row,column,0,1);
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < column; j++) {
                b[i][j]=a[j][i];
            }
        }
        return  b;
    }
    
    public int[][] getTask_Core(){
		return task_core;
    }
    public int[][] getTask_Mc(){
    	return task_mc;
    }

    public boolean isAll() {
    	return flag;
    }

    public static void main(String[] args){
  
    }

}
