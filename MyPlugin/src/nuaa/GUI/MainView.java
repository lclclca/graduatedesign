package nuaa.GUI;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;

import org.jb2011.lnf.beautyeye.ch3_button.BEButtonUI;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;

public class MainView extends JFrame {

    public MainView(SystemInstanceImpl SystemInstanceImpl) {

        JPanel Menu=new JPanel();
        Menu.setBorder(BorderFactory.createEtchedBorder());
        Menu.setLayout(new GridLayout(1,6));
        JButton menuChild1 = new JButton("读取任务参数");
        menuChild1.setUI(new BEButtonUI().setNormalColor(BEButtonUI.NormalColor.normal));
//        menuChild1.setBorder(BorderFactory.createRaisedBevelBorder());
        JButton menuChild2 = new JButton("分配约束展示");
        menuChild2.setUI(new BEButtonUI().setNormalColor(BEButtonUI.NormalColor.normal));
//        menuChild2.setBorder(BorderFactory.createLoweredBevelBorder());
        JButton menuChild3 = new JButton("调度约束展示");
        menuChild3.setUI(new BEButtonUI().setNormalColor(BEButtonUI.NormalColor.normal));
        JButton menuChild4 = new JButton("分配结果");
        menuChild4.setUI(new BEButtonUI().setNormalColor(BEButtonUI.NormalColor.normal));
        JButton menuChild5 = new JButton("调度结果");
        menuChild5.setUI(new BEButtonUI().setNormalColor(BEButtonUI.NormalColor.normal));
        JButton menuChild6 = new JButton("写回模型");
        menuChild6.setUI(new BEButtonUI().setNormalColor(BEButtonUI.NormalColor.normal));

        Menu.add(menuChild1);
        Menu.add(menuChild2);
        Menu.add(menuChild3);
        Menu.add(menuChild4);
        Menu.add(menuChild5);
        Menu.add(menuChild6);


        Menu.setVisible(true);
        /**
         * 菜单栏
         */
        JMenuBar menuBar = new JMenuBar();
        JMenu fileMenu = new JMenu("文件");
        JMenu editMenu = new JMenu("编辑");
        JMenu viewMenu = new JMenu("视图");
        JMenu aboutMenu = new JMenu("关于");
        // 一级菜单添加到菜单栏
        menuBar.add(fileMenu);
        menuBar.add(editMenu);
        menuBar.add(viewMenu);
        menuBar.add(aboutMenu);
        /**
         * 中间布局
         */
        JPanel center=new JPanel();
        center.setVisible(true);
        CardLayout cardLayout=new CardLayout();
        center.setLayout(cardLayout);

        JPanel allConstraint=new AllCons();//分配约束
        allConstraint.setBorder(BorderFactory.createEtchedBorder());

        JPanel schConstraint=new SchCons();//调度约束
        schConstraint.setBorder(BorderFactory.createEtchedBorder());

        JPanel taskPara  = new TaskPara(SystemInstanceImpl);//任务参数
        taskPara.setBorder(BorderFactory.createEtchedBorder());

        JPanel allResult=new AllResult(SystemInstanceImpl);//分配结果
        allResult.setBorder(BorderFactory.createEtchedBorder());

        JPanel schResult=new SchResult(SystemInstanceImpl);//调度结果
        schResult.setBorder(BorderFactory.createEtchedBorder());

        WriteBackPanel writeBackPanel=new WriteBackPanel(SystemInstanceImpl);//修改密码

        menuChild1.addActionListener(new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                cardLayout.show(center,"taskPara");
            }
        });
        menuChild2.addActionListener(new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                cardLayout.show(center,"allConstraint");
            }
        });

        menuChild3.addActionListener(new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                cardLayout.show(center,"schConstraint");
            }
        });

        menuChild4.addActionListener(new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                cardLayout.show(center,"allResult");
            }
        });
        menuChild5.addActionListener(new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                cardLayout.show(center,"schResult");
            }
        });

        menuChild6.addActionListener(new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                cardLayout.show(center,"writeBackPanel");
            }
        });


        center.add(new WelcomeJpanel(),"welcomeJpanel");
        center.add(taskPara,"taskPara");
        center.add(allConstraint,"allConstraint");
        center.add(schConstraint,"schConstraint");
        center.add(allResult,"allResult");
        center.add(schResult,"schResult");
        center.add(writeBackPanel,"writeBackPanel");

        this.add(center, BorderLayout.CENTER);  //中间
//        this.add(menuBar, BorderLayout.NORTH);  //北边
        this.add(Menu, BorderLayout.NORTH);   //西边

        //设置窗体属性
        this.setTitle("航电任务分配与调度工具");
        this.setSize(900, 600);
        this.setLocationRelativeTo(null);//窗口在屏幕中间显示
        this.setVisible(true);
        setResizable(false);//禁用最大化
//        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);


    }

}