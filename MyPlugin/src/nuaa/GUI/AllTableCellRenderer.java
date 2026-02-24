package nuaa.GUI;

import javax.swing.*;
import javax.swing.table.DefaultTableCellRenderer;
import java.awt.*;


public class AllTableCellRenderer extends DefaultTableCellRenderer {
    @Override
    public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {

        if(column==0){//隔行变色
            setBackground(Color.LIGHT_GRAY);
        }else if(column==1 || column==2){
            setBackground(Color.white);
        }else setBackground(Color.WHITE);
        if (column == 0) {//对齐
            setHorizontalAlignment(SwingConstants.CENTER);
        } else if (column == (table.getColumnCount() - 1)) {
            setHorizontalAlignment(SwingConstants.CENTER);
        } else {
            setHorizontalAlignment(SwingConstants.CENTER);
        }
        //自动换行
//        int maxPreferredHeight = 0;
//        for (int i = 0; i < table.getColumnCount(); i++) {
//            setText("" + table.getValueAt(row, i));
//            setSize(table.getColumnModel().getColumn(column).getWidth(), 0);
//            maxPreferredHeight = Math.max(maxPreferredHeight, getPreferredSize().height);
//        }
//
//        if (table.getRowHeight(row) != maxPreferredHeight)
//            table.setRowHeight(row, maxPreferredHeight);
//
//        setText(value == null ? "" : value.toString());

        return super.getTableCellRendererComponent(table, value, isSelected, hasFocus, row, column);
    }
}
