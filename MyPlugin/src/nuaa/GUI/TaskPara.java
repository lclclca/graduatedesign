package nuaa.GUI;

/*import com.github.pagehelper.PageInfo;*/
/*import com.mikey.design.entity.Design;
import com.mikey.design.entity.Teacher;
import com.mikey.design.service.DesignService;
import com.mikey.design.views.renderer.MyTableCellRenderer;
import com.mikey.design.utils.SpringUtil;
import com.mikey.design.utils.ThreadLocalUtil;*/

import org.jb2011.lnf.beautyeye.ch3_button.BEButtonUI;
import org.jb2011.lnf.beautyeye.ch4_scroll.BEScrollBarUI;
import org.jb2011.lnf.beautyeye.ch5_table.BETableUI;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;

import nuaa.aadlparser.GetInput;

import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableColumn;
import javax.swing.table.TableModel;

import java.awt.*;
import java.awt.event.ActionEvent;


public class TaskPara extends JPanel {

    //表头（列名）
    private Object[] columnNames = {"任务名称", "执行时间","周期","截止时间","优先级","最坏执行时间"};
    //列表内容
    private Object[][] rowData=new Object[30][6];

    public TaskPara(SystemInstanceImpl SystemInstanceImpl) {

    	TaskWcet tw=new TaskWcet();
        // 获取每个任务按优先级执行结束的时间
    	double[] wcet=tw.taskWcet(SystemInstanceImpl);
    	for(int i=0;i<wcet.length;i++) {
    		System.out.println(wcet[i]);
    	}



        setLayout(new BorderLayout());
        setBackground(Color.GRAY);

        JPanel titleJpanel=new JPanel();
        JLabel title=new JLabel("任务信息列表");
        title.setFont(new Font("微软雅黑",Font.BOLD, 20));
        titleJpanel.add(title);
        titleJpanel.setBorder(BorderFactory.createEtchedBorder());
        add(titleJpanel,BorderLayout.NORTH);
        /**
         * 毕业设计题目列表
         */
        JPanel teach=new JPanel(new BorderLayout());

        teach.setBackground(Color.red);

        //表格
        GetInput getinput=new GetInput();
        String[][] heros = getinput.inputInfo(SystemInstanceImpl);
        String[][] hero=new String[heros.length][6];
        for(int i=0;i<heros.length;i++) {
        	for(int j=0;j<5;j++) {
        		hero[i][j]=heros[i][j];
        	}
        }
        
        JTable table=new JTable(hero,columnNames);
        table.setEnabled(false);//设置表格不可编辑
        table.getTableHeader().setReorderingAllowed(false);//设置表格不可移动
        table.setUI(new BETableUI());

        ParaTableCellRenderer renderer=new ParaTableCellRenderer();//进行渲染

        if (rowData.length>0){//判断是否有值
            for (int i=0;i<columnNames.length;i++){
                TableColumn tableColumn=table.getColumn(columnNames[i]);
                tableColumn.setCellRenderer(renderer);
            }
        }

        JScrollPane sp = new JScrollPane(table);
        sp.getVerticalScrollBar().setUI(new BEScrollBarUI());

        JPanel commit=new JPanel();
        commit.setBorder(BorderFactory.createEtchedBorder());
        JButton button=new JButton("确认");
        button.setUI(new BEButtonUI().setNormalColor(BEButtonUI.NormalColor.lightBlue));
        button.addActionListener(new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                JOptionPane.showMessageDialog(null,"参数确认完成！");
            }
        });
        JButton button2=new JButton("任务WCRT计算");
        button2.setUI(new BEButtonUI().setNormalColor(BEButtonUI.NormalColor.lightBlue));
        button2.addActionListener(new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                //创建一个TableModel对象，并传入表头和表内容
                TableModel tableModel=new DefaultTableModel(heros,columnNames);
//				将TableModel对象传入Table表格
                table.setModel(tableModel);
                ParaTableCellRenderer renderer=new ParaTableCellRenderer();//进行渲染
                if (rowData.length>0){//判断是否有值
                    for (int i=0;i<columnNames.length;i++){
                        TableColumn tableColumn=table.getColumn(columnNames[i]);
                        tableColumn.setCellRenderer(renderer);
                    }
                }
            }
        });
        commit.add(button);
        commit.add(button2);



//        teach.add(sp.getTableHeader(),BorderLayout.NORTH);
        teach.add(sp,BorderLayout.CENTER);

        add(teach,BorderLayout.CENTER);
        add(commit,BorderLayout.SOUTH);

    }

}
