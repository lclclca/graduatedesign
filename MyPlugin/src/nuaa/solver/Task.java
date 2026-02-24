package nuaa.solver;
import org.chocosolver.solver.Model;
import org.chocosolver.solver.Solver;
import org.chocosolver.solver.search.loop.monitors.IMonitorSolution;
import org.chocosolver.solver.search.strategy.Search;
import org.chocosolver.solver.variables.IntVar;

public class Task {
    public void modelandsolve() {

        //分区信息：每行代表一个分区，每列为一个分区信息
        int[][] P ={{1,100,100},
                {1,200,200},
                {2,200,200},
                {2,100,100}};


        //任务信息：每行代表一个任务，每列为一个任务信息
        int[][] T={{1,1,20,100,100},
                {1,5,5,100,100},
                {1,3,10,100,100},
                {2,3,20,200,200},
                {2,5,10,200,200},
                {2,4,15,200,200},
                {3,4,15,200,200},
                {3,5,10,200,200},
                {3,3,20,200,200},
                {4,5,5,100,100},
                {4,2,15,100,100},
                {4,3,10,100,100},
        };


        Model model = new Model("Task");

        //声明变量
        IntVar[] PDura = model.intVarArray(4, 0, 100);
        IntVar[] start1 = model.intVarArray(3, 0, 100);
        IntVar[] start2 = model.intVarArray(3, 0, 100);
        IntVar[] start3 = model.intVarArray(3, 0, 100);
        IntVar[] start4 = model.intVarArray(3, 0, 100);
        IntVar obj=model.intVar(0,200);

        //构造约束

        //分区级
        //计算每个分区内任务的执行总时长
        int temp=0;
        int[] p=new int[4];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 12; j++) {
                if (T[j][0]==i+1){
                    temp=temp+T[j][2];
                }
            }
            p[i]=temp;
            temp=0;
        }

        //分区持续时长要大于等于分区内任务总时长
        for (int i = 0; i < 4; i++) {
            model.arithm(PDura[i],">=",p[i]).post();
        }

        //分区时长和小于系统主时间框架
        model.arithm(PDura[0],"+",PDura[1],"<=",100).post();
        model.arithm(PDura[2],"+",PDura[3],"<=",100).post();




        //任务级
        //不能超过截止时间
        for (int i = 0; i < 3; i++) {
            model.arithm(start1[i],"<=",PDura[0]).post();
        }
        for (int i = 0; i < 3; i++) {
            model.arithm(start2[i],">=",PDura[0]).post();
        }
        for (int i = 0; i < 3; i++) {
            model.arithm(start3[i],"<=",PDura[2]).post();
        }
        for (int i = 0; i < 3; i++) {
            model.arithm(start4[i],">=",PDura[2]).post();
        }




        //不能同时开始两个分区
        model.allDifferent(start1).post();
        model.allDifferent(start2).post();
        model.allDifferent(start3).post();
        model.allDifferent(start4).post();
        
//         优先级约束
        for (int i = 0; i < 3; i++) {
            for (int j =i+1; j <3 ; j++) {
                if(T[i][1]<T[j][1]){
                    model.arithm(start1[i],">",start1[j]).post();
                }

            }
        }
        
//        同一时间内不能运行两个任务
        for (int i = 0; i < 3; i++) {
            for(int j=i+1;j<3;j++){
                if(start1[i].getValue()>start1[j].getValue()){
                    model.arithm(start1[i], "-", start1[j], ">=", T[j][2]).post();
                }
                else model.arithm(start1[j], "-", start1[i], ">=", T[i][2]).post();
            }
        }
        
        for (int i = 0; i < 3; i++) {
            for(int j=i+1;j<3;j++){
                if(start2[i].getValue()>start2[j].getValue()){
                    model.arithm(start2[i], "-", start2[j], ">=", T[j+3][2]).post();
                }
                else model.arithm(start2[j], "-", start2[i], ">=", T[i+3][2]).post();
            }
        }
        for (int i = 0; i < 3; i++) {
            for(int j=i+1;j<3;j++){
                if(start3[i].getValue()>start3[j].getValue()){
                    model.arithm(start3[i], "-", start3[j], ">=", T[j+6][2]).post();
                }
                else model.arithm(start3[j], "-", start3[i], ">=", T[i+6][2]).post();
            }
        }
        for (int i = 0; i < 3; i++) {
            for(int j=i+1;j<3;j++){
                if(start4[i].getValue()>start4[j].getValue()){
                    model.arithm(start4[i], "-", start4[j], ">=", T[j+9][2]).post();
                }
                else model.arithm(start4[j], "-", start4[i], ">=", T[i+9][2]).post();
            }
        }




        //目标变量
        model.arithm(PDura[0],"+",PDura[2],"=",obj).post();




        Solver solver = model.getSolver();
//        solver.plugMonitor((IMonitorSolution) () -> 
        if(solver.solve()){
            for (int i = 0; i < 4; i++) {
                System.out.printf("%d",PDura[i].getValue());
                System.out.printf("\t");
            }
            for (int i = 0; i < 3; i++) {
                System.out.printf("%d",start1[i].getValue());
                System.out.printf("\t");
            }
            for (int i = 0; i < 3; i++) {
                System.out.printf("%d",start2[i].getValue());
                System.out.printf("\t");
            }
            for (int i = 0; i < 3; i++) {
                System.out.printf("%d",start3[i].getValue());
                System.out.printf("\t");
            }
            for (int i = 0; i < 3; i++) {
                System.out.printf("%d",start4[i].getValue());
                System.out.printf("\t");
            }
        }else {
            System.out.println("The solver has proved the problem has no solution");
        }


//        solver.setSearch(Search.inputOrderLBSearch(obj));
//        solver.showShortStatistics();
//        solver.printStatistics();
//        solver.findOptimalSolution(obj, true);

    }

    public static void main(String[] args){new Task().modelandsolve();}
}

