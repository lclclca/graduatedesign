/* ==============================================================
 *  FILE:    subprograms.c
 *  ROLE:    用户自定义子程序实现（Subprogram Implementations）
 *
 *  IMA 架构层次：应用层（Application Layer）
 *  IMA 分区：ps4 (P4)     模块：M3 (arinckernel.module3)
 *  对应 AADL 文件：DIMA_subprograms.aadl
 *
 *  【注意】ps4 分区的任务不调用任何 AADL subprogram。
 *  task42 执行 order_in 消息读取 → queueingout 转发；
 *  task43 接收 queueingin 队列消息；
 *  task41/task44/task45 为无通信的计算占位任务。
 *  若未来需要添加计算逻辑，在此文件中添加函数定义，
 *  并在 subprograms.h 中补充声明。
 * ==============================================================*/
#include "subprograms.h"
