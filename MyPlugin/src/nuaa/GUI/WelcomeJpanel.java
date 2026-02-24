package nuaa.GUI;



import javax.swing.*;
import java.awt.*;


public class WelcomeJpanel extends JPanel {
    //登入用户
    public String LOGINUSERNAME="";

    public WelcomeJpanel() {



        //边缘布局
        setLayout(new BorderLayout());
        //banner
        JPanel titleJpanel=new JPanel();
//        titleJpanel.setBackground(Color.LIGHT_GRAY);
        JLabel title=new JLabel("欢迎");
        title.setFont(new Font("宋体",Font.BOLD, 30));
        titleJpanel.add(title);
        titleJpanel.setBorder(BorderFactory.createEtchedBorder());
        add(titleJpanel,BorderLayout.NORTH);

        //个人信息
        JPanel mainJpanel=new JPanel();
        mainJpanel.setBorder(BorderFactory.createEtchedBorder());
        ImageIcon bg=new ImageIcon("C:\\images\\bg.jpg");
        JLabel bglabel=new JLabel(bg);
        mainJpanel.add(bglabel);

        add(mainJpanel,BorderLayout.CENTER);
    }
}
