package nuaa.solver;

import org.chocosolver.solver.Model;
import org.chocosolver.solver.variables.IntVar;

public class WcetCount {
    /*
    task_part:任务所属分区
    task_pri:任务优先级
    exec_time:任务执行时间
    model:约束规划建模实例
    wcet:wcet变量
    t:周期变量
    nums:公式中累加部分	
    temp:公式中除部分
    */
    public static void wcet(int Nt,int[] task_part, int[] task_pri, int[] exec_time,Model model, IntVar[] wcet,IntVar[] t,IntVar[][] nums,IntVar[] temp){
        //构建wcet公式
        for(int i=0;i<Nt;i++){
            for (int j = 0; j < Nt; j++) {
                if(task_part[j]==task_part[i] && task_pri[j]>task_pri[i]){
                    model.div(wcet[i],t[j],nums[i][j]).post();
                    nums[i][j]=model.intOffsetView(nums[i][j],1);
                }
            }
        }
        for (int i = 0; i < Nt; i++) {
            model.scalar(nums[i],exec_time,"=",temp[i]).post();
        }
        for (int i = 0; i < Nt; i++) {
            model.arithm(wcet[i],"-",temp[i],"=",exec_time[i]).post();
        }

    }
    
    
}
