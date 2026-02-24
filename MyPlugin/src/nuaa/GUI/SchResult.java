package nuaa.GUI;


import org.jb2011.lnf.beautyeye.ch12_progress.BEProgressBarUI;
import org.jb2011.lnf.beautyeye.ch3_button.BEButtonUI;
import org.jb2011.lnf.beautyeye.ch5_table.BETableUI;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;

import nuaa.solver.Sch_Part;

import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableColumn;
import javax.swing.table.TableModel;
import java.awt.*;
import java.awt.event.ActionEvent;


public class SchResult extends JPanel {
    //service接口


    public static double[][] data={{1.0,1.0,0.0,10.0,1.0}, {1.0,1.0,50,80,1.0},{1.0,1.0,100,110,1.0},{1.0,1.0,150,180,1.0},
    		{5.0,5.0,150,180,5.0},{8.0,8.0,100,120,8.0},
            {2.0,2.0,30.0,50.0,2.0},{4.0,4.0,130,150,4.0},
            {3.0,3.0,110,130,3.0}};
    public static double[][] data1={{1.0,1.0,0,0,1.0},{2.0,2,0,0,2},{3,3,0,0,3},{4,4,0,0,4},{8,8,0,0,8}};

    public void getData(){

    }

    private class Progress extends Thread
    {
        JProgressBar progressBar;
        JButton button;
        //进度条上的数字
        int[] progressValues={6,18,27,39,51,66,81,100};
        Progress(JProgressBar progressBar,JButton button)
        {
            this.progressBar=progressBar;
            this.button=button;
        }
        public void run()
        {
            for(int i=0;i<progressValues.length;i++)
            {
                try
                {
                    Thread.sleep(500);
                }
                catch(InterruptedException e)
                {
                    e.printStackTrace();
                }
                //设置进度条的值
                progressBar.setValue(progressValues[i]);
            }
            progressBar.setIndeterminate(false);
            progressBar.setString("求解完成！");
            button.setEnabled(true);
        }
    }



    public SchResult(SystemInstanceImpl SystemInstanceImpl) {
    	
    	Sch_Part sp=new Sch_Part();
    	sp.modelandsolve(SystemInstanceImpl);
    	double[][] schdata=sp.Data(); 
    	boolean flag=sp.isSch();
    	
    	
        setLayout(new BorderLayout());
        /**
         * banner
         */
        JPanel titleJpanel=new JPanel(new BorderLayout());
        //进度条
        JProgressBar progressBar=new JProgressBar();
        progressBar.setStringPainted(true);
        progressBar.setUI(new BEProgressBarUI());
        JButton done=new JButton("完成");
        done.setUI(new BEButtonUI().setNormalColor(BEButtonUI.NormalColor.normal));
        done.setEnabled(false);

        titleJpanel.add(progressBar,BorderLayout.CENTER);
        titleJpanel.add(done,BorderLayout.EAST);
        add(titleJpanel,BorderLayout.NORTH);

        //调度结果展示面板
        JPanel panel=new JPanel(null);
        panel.setBackground(Color.WHITE);

        //甘特图展示面板
        CardLayout cardLayout1=new CardLayout();
        JPanel gantepanel=new JPanel(cardLayout1);
        gantepanel.setBounds(20, 10, 800, 410);
        //初始状态
        CanvasGante canvas=new CanvasGante(SystemInstanceImpl);
        canvas.setBorder(BorderFactory.createTitledBorder("分区甘特图"));
        canvas.setBackground(Color.white);
        canvas.setData(data1);
        canvas.repaint();
        gantepanel.add(canvas,"canvas");
        //更新状态
        CanvasGante newcanvas=new CanvasGante(SystemInstanceImpl);
        newcanvas.setBorder(BorderFactory.createTitledBorder("分区甘特图"));
        newcanvas.setBackground(Color.white);
        //判断是否可调度
        if(flag) newcanvas.setData(schdata);
        else newcanvas.setData(data1);
        newcanvas.repaint();
        gantepanel.add(newcanvas,"newcanvas");
        
        
        //提示信息
        JLabel tip=new JLabel();
        tip.setText("<a,b>:<开始时间，结束时间>");
        tip.setBounds(20,460,800,40);

        //信息展示面板
        CardLayout cardLayout2=new CardLayout();
        JPanel mespanel=new JPanel(cardLayout2);
        mespanel.setBackground(Color.WHITE);
        mespanel.setBounds(20, 420, 800, 50);
        //初始状态
        JLabel label=new JLabel();
        label.setBorder(BorderFactory.createTitledBorder("调度信息"));
        label.setBackground(Color.WHITE);
        //更新状态
        JLabel newlabel=new JLabel();
        String   strMsg   =   "系统可调度";
        String   strMsg1   =   "系统不可调度";
        
        if(flag) newlabel.setText(strMsg);
        else newlabel.setText(strMsg1);
        Font f=new Font("宋体", Font.BOLD, 18);
        newlabel.setFont(f);
        newlabel.setForeground(Color.GREEN);
        newlabel.setBorder(BorderFactory.createTitledBorder("调度信息"));
        newlabel.setBackground(Color.WHITE);
        mespanel.add(label,"label");
        mespanel.add(newlabel,"newlabel");

        panel.add(gantepanel);
//        panel.add(tip);
        panel.add(mespanel);

        done.addActionListener(new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                cardLayout1.show(gantepanel,"newcanvas");
                cardLayout2.show(mespanel,"newlabel");
            }
        });

        JPanel commit=new JPanel();
        commit.setBorder(BorderFactory.createEtchedBorder());
        JButton button=new JButton("求解调度问题");
        button.setUI(new BEButtonUI().setNormalColor(BEButtonUI.NormalColor.lightBlue));
        button.addActionListener(new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                progressBar.setString("求解进行中...");
                new Progress(progressBar,done).start();
            }
        });
        commit.add(button);

        add(panel,BorderLayout.CENTER);
        add(commit,BorderLayout.SOUTH);
    }
}
