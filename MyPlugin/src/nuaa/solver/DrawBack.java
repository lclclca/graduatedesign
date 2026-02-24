package nuaa.solver;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;
import java.util.Comparator;

import org.osate.aadl2.instance.impl.SystemInstanceImpl;

import nuaa.GUI.SchResult;
import nuaa.aadlparser.CoreName;
import nuaa.aadlparser.Threads;

public class DrawBack {
		BackInfo info=new BackInfo();
		public void allocation(SystemInstanceImpl systemInstanceImpl) {
			Allocation all=new Allocation();
			all.modelandsolve(systemInstanceImpl);
			int[][] task_core=all.task_core;
			int[][] task_mc=all.task_mc;
			System.out.println(info.sysName(systemInstanceImpl));
			replaceContentToFile(info.path1,info.sysName(systemInstanceImpl),"		properties"+"\n");
			for(int i=0;i<task_core.length;i++) {
				for(int j=0;j<task_core[0].length;j++) {
					if(task_core[i][j]==1) {
					replaceContentToFile(info.path1,info.sysName(systemInstanceImpl),"			Actual_Processor_Binding => (reference ("+
											info.coreName(systemInstanceImpl)[i]+
											")) applies to "+info.taskName(systemInstanceImpl)[j]+
											";\n");
				}
				}
			}
			
			for(int i=0;i<task_mc.length;i++) {
				for(int j=0;j<task_mc[0].length;j++) {
					if(task_mc[i][j]==1) {
					replaceContentToFile(info.path1,info.sysName(systemInstanceImpl),"			Actual_Memory_Binding => (reference ("+
											info.memoryName(systemInstanceImpl)[i]+
											")) applies to "+info.taskName(systemInstanceImpl)[j]+
											";\n");
				}
				}
			}
			
		}
		
		public void schedule(SystemInstanceImpl systemInstanceImpl) {
			Sch_Part sp=new Sch_Part();
    		sp.modelandsolve(systemInstanceImpl);
    		double[][] data=sp.Data();
    		int MAF=sp.MAF();
			Arrays.sort(data,new Comparator<double[]>(){
			    @Override
			    public int compare(double[] s1, double[] s2) {
			        return (int) (s1[2]-s2[2]);
			    }
			});
			int[] part_sch=new int[data.length];
			for(int i=0;i<data.length;i++) {
				part_sch[i]=(int) (data[i][3]-data[i][2]);
			}

			replaceContentToFile(info.path2,"	end DPU_CPU1.c4impl","		properties"+"\n");
			replaceContentToFile(info.path2,"	end DPU_CPU1.c4impl","			ARINC653::Module_Major_Frame =>"+ MAF+ "ms;\n");
			replaceContentToFile(info.path2,"	end DPU_CPU1.c4impl","			ARINC653::Module_Schedule =>( \n");
//			String[] part=info.core(systemInstanceImpl);
			for(int i=0;i<part_sch.length;i++) {
				if(i==part_sch.length-1) {
					replaceContentToFile(info.path2,"	end DPU_CPU1.c4impl","				[Partition => reference ("+
							"core1"+
							");\n"+"					Duration => "+part_sch[i]+"ms;\n"+
							"					Periodic_Processing_Start => true;]\n"
							);
					break;
					}
				replaceContentToFile(info.path2,"	end DPU_CPU1.c4impl","				[Partition => reference ("+
						"core1"+
						");\n"+"					Duration => "+part_sch[i]+"ms;\n"+
						"					Periodic_Processing_Start => true;],\n"
						);
				} 	
			replaceContentToFile(info.path2,"	end DPU_CPU1.c4impl","			);\n");
			}
	
		//path：文件路径 str：插入的地方 con：插入的内容
		public  void replaceContentToFile(String path, String str ,String con){
			  try {   
			   FileReader read = new FileReader(path);
			   BufferedReader br = new BufferedReader(read);
			   StringBuilder content = new StringBuilder();
			   
			   String behind = new String();
			   
			   while(br.ready() != false){
			    content.append(br.readLine());
			    content.append("\r\n");
			   }
//			   System.out.println(content.toString());
			   int dex = content.indexOf(str);
			   if( dex != -1){
//			    System.out.println(content.substring(dex, content.length()));
			   behind = content.substring(dex, content.length());
			     content.delete(dex, content.length());
			      }
			   content.append(con);
			   content.append(behind);
			    br.close();
			    read.close();
			     FileOutputStream fs = new FileOutputStream(path);
			     fs.write(content.toString().getBytes());
			              fs.close();
			   
			  } catch (FileNotFoundException e) {
			    e.printStackTrace();
			   
			  } catch (IOException e){
			    e.printStackTrace();
			  }
			 } 
}
