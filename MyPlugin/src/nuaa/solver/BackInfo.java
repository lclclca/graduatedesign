package nuaa.solver;

import org.osate.aadl2.impl.ThreadImplementationImpl;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;

import nuaa.aadlparser.Core;
import nuaa.aadlparser.CoreName;
import nuaa.aadlparser.MemoryName;
import nuaa.aadlparser.Threads;

public class BackInfo {
	public static String path1="C:\\CASE\\System_model\\DIMA_Models.aadl";
	public static String path2="C:\\CASE\\System_model\\DIMA_Processor.aadl";
	
	
	public String sysName(SystemInstanceImpl systemInstanceImpl) {
		String name= systemInstanceImpl.getName();
		StringBuffer sb=new StringBuffer();
		sb.append("end ");
		int index=0;
		while(name.charAt(index)!='_') {
			sb.append(name.charAt(index));
			index++;
		}
		sb.append(name.charAt(index));
		index++;
		while(name.charAt(index)!='_') {
			sb.append(name.charAt(index));
			index++;
		}
		sb.append(".impl");
		return sb.toString();
	}
	
	
	public String[] coreName(SystemInstanceImpl systemInstanceImpl) {
		CoreName co=new CoreName();
		String[] core=co.coreName(systemInstanceImpl);
//		String[] core=new String[] {"cpu.part1","cpu.part2"};
		return core;
	}
	public String[] taskName(SystemInstanceImpl systemInstanceImpl) {
		Threads thread=new Threads();
    	ThreadImplementationImpl[] threadArray=thread.threadArray(systemInstanceImpl);
    	String[] task=thread.taskName();
//		String[] task=new String[] {"partition1_pr.temperature","partition1_pr.order","partition2_pr.receiver","partition2_pr.printer"};
		return task;
	}
	
	public String[] memoryName(SystemInstanceImpl systemInstanceImpl) {
		MemoryName mem=new MemoryName();
		String[] memName=mem.memoryName(systemInstanceImpl);
		return memName;
	}
	public String[] core(SystemInstanceImpl systemInstanceImpl) {
		CoreName co=new CoreName();
		String[] core=co.core(systemInstanceImpl);
//		String[] core=new String[] {"cpu.part1","cpu.part2"};
		return core;
	}
	}