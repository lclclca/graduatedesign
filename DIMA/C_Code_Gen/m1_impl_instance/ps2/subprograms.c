/* ==============================================================
 *  FILE:    subprograms.c
 *  ROLE:    用户自定义子程序实现（Subprogram Implementations）
 *
 *  IMA 架构层次：应用层（Application Layer）
 *  IMA 分区：ps2 (P2)     模块：M1 (arinckernel.module1)
 *  对应 AADL 文件：DIMA_subprograms.aadl
 *
 *  这是框架代码中用户需要填写业务逻辑的文件之一。
 *  main.c 和 activity.c 负责所有 ARINC 653 APEX API 调用（资源创建、
 *  消息收发）；本文件中的子程序负责对已收到的数据进行计算/处理。
 *
 *  用户应将 TODO 占位代码替换为实际业务逻辑：
 *    - commandboard_receiveinput_spg：处理从采样端口读到的传感器数据
 *    - commandboard_printinfos_spg  ：格式化打印或记录数据
 * ==============================================================*/
#include "subprograms.h"

/* 对应 AADL: subprograms::commandboard_receiveinput_spg
 * 调用者：task21 — 将 pr2samplingin 读到的值处理后写入 bb_acc3
 * data_in  ← 从采样端口 pr2samplingin 读取的值
 * data_out → 写入黑板 bb_acc3 的值
 */
void commandboard_receiveinput_spg(integer *data_in, integer *data_out)
{
    /* TODO: 用户填写接收处理逻辑 */
    if (data_out != 0)
        *data_out = *data_in;
}

/* 对应 AADL: subprograms::commandboard_printinfos_spg
 * 调用者：task21 — 打印/记录 bb_acc3 的当前值
 * data_in ← 要打印的数据
 */
void commandboard_printinfos_spg(integer *data_in)
{
    /* TODO: 用户填写打印/记录逻辑 */
    (void)data_in;
}
