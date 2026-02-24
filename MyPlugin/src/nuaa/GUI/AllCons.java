package nuaa.GUI;



import org.jb2011.lnf.beautyeye.ch4_scroll.BEScrollBarUI;
import org.jb2011.lnf.beautyeye.ch4_scroll.BEScrollPaneUI;
import org.jb2011.lnf.beautyeye.ch6_textcoms.BETextAreaUI;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;


public class AllCons extends JPanel {
    
    public AllCons() throws HeadlessException {

        setLayout(null);
        setBackground(Color.WHITE);
        //参数定义面板
        JPanel panel1=new JPanel();
        panel1.setBounds(10,10,400,500);
        panel1.setBackground(Color.WHITE);
        panel1.setBorder(BorderFactory.createTitledBorder("参数定义"));
        add(panel1);
        ImageIcon bg1=new ImageIcon("C:\\images\\allpara.png");
        JLabel label1=new JLabel(bg1);
        label1.setBorder(BorderFactory.createEtchedBorder());
        panel1.add(label1);
        //决策变量面板
        ImageIcon bg2=new ImageIcon("C:\\images\\alldecide.png");
        JLabel label2=new JLabel(bg2);
        JScrollPane panel2=new JScrollPane(label2,ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED,ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
        panel2.getVerticalScrollBar().setUI(new BEScrollBarUI());
        panel2.setBounds(420,10,470,100);
        panel2.setBackground(Color.white);
        panel2.setBorder(BorderFactory.createTitledBorder("决策变量"));
        add(panel2);
        //约束面板
        ImageIcon bg3=new ImageIcon("C:\\images\\allcons.png");
        JLabel label3=new JLabel(bg3);
        JScrollPane panel3=new JScrollPane(label3,ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED,ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
        panel3.getVerticalScrollBar().setUI(new BEScrollBarUI());
        panel3.setBounds(420,110,470,300);
        panel3.setBackground(Color.white);
        panel3.setBorder(BorderFactory.createTitledBorder("约束公式"));
        add(panel3);
        //目标函数
        ImageIcon bg4=new ImageIcon("C:\\images\\allobj.png");
        JLabel label4=new JLabel(bg4);
        JScrollPane panel4=new JScrollPane(label4,ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED,ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
        panel4.getVerticalScrollBar().setUI(new BEScrollBarUI());
        panel4.setBounds(420,410,470,100);
        panel4.setBackground(Color.white);
        panel4.setBorder(BorderFactory.createTitledBorder("目标函数"));
        add(panel4);
    }
}
