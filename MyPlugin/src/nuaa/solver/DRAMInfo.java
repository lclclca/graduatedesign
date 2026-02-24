package nuaa.solver;


public class DRAMInfo {
//	OutputGUI test=new OutputGUI();
//	String[] DRAMpara=test.DRAMpara();
    public final static double tck=1.5*Math.pow(10.0, -9.0);
    public final static double trp=9;
    public final static double trcd=9;
    public final static double CL=9;
    public final static double WL=7;
    public final static double BL=8;
    public final static double twtr=5;
    public final static double twr=10;
    public final static double trrd=4;
    public final static double tfaw=20;
    //PRE预处理指令
    public final static double PRE=tck;
    //读写指令
    public final static double RW=Math.max(WL+BL/2+twtr,CL+BL/2+2-WL)*tck;
    //ACT激活指令
    public final static double ACT=Math.max(trrd,tfaw-3*trrd)*tck;
    //行命中服务时间
    public final static double Hit=Math.max(CL+BL/2+2,WL+BL/2+Math.max(twtr,twr))*tck;
    //行冲突服务时间
    public final static double Conf=(trp+trcd)*tck+Hit;
    //连续行命中服务时间
    public final static double Conhit=(Math.ceil(0.5)*(WL+BL/2+twtr)+Math.floor(0.5)*CL+(twr-twtr))*tck;




}
