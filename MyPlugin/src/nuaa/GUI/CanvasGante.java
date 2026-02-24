package nuaa.GUI;

import org.ejml.data.DenseMatrix64F;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;

import nuaa.solver.Sch_Part;

import javax.swing.*;
import java.awt.*;

public class CanvasGante extends JPanel {
    private static final long serialVersionUID = 1L;

    private final static int margin_left = 20;// 图像左边距
    private final static int margin_top = 25;// 图像上边距
    private final static int margin_right = 20;// 图像右边距
    private final static int margin_buttom = 0;// 图像下边距

    private final static int label_width = 40;// 任务标签
    private final static int block_width = 40;// 块高度
    private final static int block_height = 10;// 块高度
    private final static int num_x_divide = 20;// x轴刻度个数
    private int MAF;

    private double[][] data;

    public double[][] getData() {
        return data;
    }

    public void setData(double[][] data) {
        this.data = data;
    }

    // 颜色数组，以区分不同的原油类型
    //33-3、35-1、36-1、
    private final static Color[] colors = {new Color(63, 72, 204),new Color(8, 193, 194),new Color(255, 127, 39),new Color(163, 73, 164),
            new Color(195, 195, 195), new Color(136, 0, 21), new Color(185, 122, 87), new Color(237, 28, 36),
            new Color(255, 174, 201),  new Color(255, 242, 0), new Color(239, 228, 176),
            new Color(34, 131, 83), new Color(181, 230, 29), new Color(0, 162, 232), new Color(153, 217, 234),
             new Color(112, 146, 190),  new Color(200, 191, 231),
            new Color(89, 173, 154),  new Color(225, 18, 18), new Color(153, 217, 234),
            new Color(199, 73, 4) };
    
    public CanvasGante(SystemInstanceImpl SystemInstanceImpl) {
    	Sch_Part sp=new Sch_Part();
    	sp.modelandsolve(SystemInstanceImpl);
    	MAF=sp.MAF();
    }
    
    
    public void paint(Graphics g) {
        super.paint(g);
        g.setPaintMode();

        // 切记，这里要获取Graphics的长和宽，而不是Form的长和宽
        int width = 800;
        int height = 200;

        DenseMatrix64F dataMatrix = new DenseMatrix64F(data);
        //col_max返回的是矩阵数据中每一列的最大值
        double[] col_max = MatrixHelper.getColMax(dataMatrix).data;
        int numOfPartType = (int) col_max[4];// 分区数
//        int maxTime = (int) col_max[3];// 最后的任务的完成时间
        int maxTime = MAF;// 最后的分区的完成时间
        int numOfCores = (int) col_max[0];// 核心数

        double scale_x = 1.0 * (width - margin_left - margin_right - label_width) / maxTime;// 图像大小变化尺度
        double scale_y = 1.0 * (600 - margin_top - margin_buttom - label_width) / 600 * 50;// 图像大小变化尺度【参考原始比例】

        // 绘制详细的调度数据
        //分区运行情况
        for (int i = 0; i < data.length; i++) {
            // 使用不同的颜色填充封闭的矩形区域
            int color = (int) data[i][0]-1;
            // 不显示停运
            if (i<=numOfPartType || data[i][2]!=0) {
                // 计算矩形区域所在位置和宽度
                int data_x = (int) (margin_left + label_width + data[i][2] * scale_x);
                int data_y = (int) (margin_top + 1.0 * (data[i][0] - 1) * scale_y);
                int data_width = (int) (1.0 * (data[i][3] - data[i][2]) * scale_x);
                // 绘制矩形区域，并设置矩形区域的标 注
                g.setColor(colors[color]);
                g.fill3DRect(data_x, data_y, data_width, block_height, false);
                int tank = (int) data[i][0];
                g.setColor(Color.black);
                g.drawString("<" + data[i][2]+","+data[i][3] + ">", data_x, data_y);
           }
        }

        //cpu工作情况
        for (int i = 0; i < data.length; i++) {
            // 使用不同的颜色填充封闭的矩形区域
            int color = (int) data[i][0]-1;
            // 不显示停运
            if (i<=numOfPartType || data[i][2]!=0) {
                // 计算矩形区域所在位置和宽度
                int data_x = (int) (margin_left + label_width + data[i][2] * scale_x);
                int data_y =(int) (margin_top + numOfCores * scale_y-block_height/2);
                int data_width = (int) (1.0 * (data[i][3] - data[i][2]) * scale_x);
                // 绘制矩形区域，并设置矩形区域的标 注
                g.setColor(colors[color]);
                g.fill3DRect(data_x, data_y, data_width, block_height, false);
                int tank = (int) data[i][4];
                g.setColor(Color.black);
            }
        }

        // 标识x轴刻度
        for(int j=0;j<numOfCores;j++){
            for (int i = 0; i < num_x_divide; i++) {
                int x1 = (int) (margin_left + label_width
                        + (i + 1) * (width - margin_left - margin_right - label_width) / num_x_divide);
                int y1 = (int) (margin_top + j * scale_y +block_height/2);
                int x2 = (int) (margin_left + label_width
                        + (i + 1) * (width - margin_left - margin_right - label_width) / num_x_divide);
                int y2 = (int) (margin_top + j * scale_y +block_height/2);

                int x3 = margin_left + label_width;
                int x4 = width - margin_right;
                g.setColor(Color.black);
                g.drawLine(x3,y1,x4,y2);
                g.drawLine(x1, y1-5, x2, y2+5);
                g.drawString((i + 1) * maxTime / num_x_divide + "", x2 - 10, y2 + 15);
                g.setFont(new Font("Arial", Font.PLAIN,10));
            }
        }

        for (int i = 0; i < num_x_divide; i++) {
            int x1 = (int) (margin_left + label_width
                    + (i + 1) * (width - margin_left - margin_right - label_width) / num_x_divide);
            int y1 = (int) (margin_top + numOfCores * scale_y -5);
            int x2 = (int) (margin_left + label_width
                    + (i + 1) * (width - margin_left - margin_right - label_width) / num_x_divide);
            int y2 = (int) (margin_top + numOfCores * scale_y+5);

            g.setColor(Color.black);
            g.drawLine(x1, y1, x2, y2);
            g.drawString((i + 1) * maxTime / num_x_divide + "", x2 - 15, y2 + 15);
        }

        // 标识y轴刻度
        for (int i = 0; i < numOfCores; i++) {
            int x = margin_left;
            int y = (int) (margin_top + 1.0 * (numOfCores - i - 1) * scale_y + block_height / 1.5);
            g.drawString("P" + (numOfCores-i) + "", x, y);
        }

        // 绘制坐标轴
        int x1 = margin_left + label_width;
        int y1 = (int) (margin_top + numOfCores * scale_y);
        int x2 = width - margin_right;
        int y2 = (int) (margin_top + numOfCores * scale_y);
        g.setColor(Color.black);
        g.drawString("CPU",margin_left,y1);
        g.drawLine(x1, y1, x2, y2);// x轴
        int x3 = margin_left + label_width;
        int y3 = margin_top;
        int x4 = margin_left + label_width;
        int y4 = (int) (margin_top + numOfCores * scale_y);
        g.drawLine(x3, y3, x4, y4);// y轴

//         绘制legend
//        for (int i = 0; i < numOfPartType; i++) {
//            // 计算矩形区域所在位置和宽度
//            int x = (int) (margin_left + label_width + i * 3 * block_height);
//            int y = height - 200;
//
//            // 使用不同的颜色填充封闭的矩形区域
//            g.setColor(colors[i]);
//            g.fill3DRect(x, y, block_width, block_height, true);
//
//            // 设置标签
//            int partType = i + 1;
//            g.setColor(Color.black);
//            g.drawString("P" + partType + "", x, y);
//        }
    }
}
