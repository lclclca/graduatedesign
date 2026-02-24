package nuaa.GUI;


/*
import com.mikey.design.entity.Admin;
import com.mikey.design.entity.Student;
import com.mikey.design.entity.Teacher;
import com.mikey.design.service.AdminService;
import com.mikey.design.service.StudentService;
import com.mikey.design.service.TeacherService;
import com.mikey.design.utils.SpringUtil;
import com.mikey.design.utils.ThreadLocalUtil;*/

import javax.swing.*;

import org.jb2011.lnf.beautyeye.ch3_button.BEButtonUI;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;

import nuaa.solver.DrawBack;

import java.awt.*;
import java.awt.event.ActionEvent;


public class WriteBackPanel extends JPanel {


    public  WriteBackPanel(SystemInstanceImpl SystemInstanceImpl) {
    	
    	//写回模型
    	DrawBack back=new DrawBack();

        //边缘布局
        setLayout(new BorderLayout());
        setBorder(BorderFactory.createTitledBorder("AADL架构图"));
        setBackground(Color.white);
        //banner
        JPanel titleJpanel = new JPanel();
        titleJpanel.setBorder(BorderFactory.createEtchedBorder());
        JLabel title = new JLabel("修改登入密码");
        title.setFont(new Font("宋体", Font.BOLD, 20));
        titleJpanel.add(title);
//        add(titleJpanel, BorderLayout.NORTH);

        //AADL架构展示面板
        JPanel panel=new JPanel();
        panel.setBackground(Color.white);
        panel.setBorder(BorderFactory.createEtchedBorder());
        add(panel,BorderLayout.CENTER);
        ImageIcon bg=new ImageIcon("C:\\images\\aadl3.png");
        JLabel label=new JLabel(bg);
        label.setBackground(Color.white);
        panel.add(label);
        
        //确认写回
        JPanel commit=new JPanel();
        commit.setBorder(BorderFactory.createEtchedBorder());
        JButton button=new JButton("确认");
        button.setUI(new BEButtonUI().setNormalColor(BEButtonUI.NormalColor.lightBlue));
        button.addActionListener(new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
            	back.allocation(SystemInstanceImpl);
            	back.schedule(SystemInstanceImpl);
                JOptionPane.showMessageDialog(null,"AADL模型更改成功！");
            }
        });
        commit.add(button);
        add(commit,BorderLayout.SOUTH);
    }
}
