/* ==============================================================
 *  FILE:    subprograms.c
 *  ROLE:    用户自定义子程序实现（Subprogram Implementations）
 *
 *  IMA 架构层次：应用层（Application Layer）
 *  IMA 分区：ps5 (P5)     模块：M2 (arinckernel.module2)
 *  对应 AADL 文件：DIMA_subprograms.aadl
 *
 *  【注意】ps5 分区的任务不调用任何 AADL subprogram。
 *  task51/task52 只执行分区间消息收发，task53 无任何通信操作。
 *  若未来需要添加计算逻辑，在此文件中添加函数定义，
 *  并在 subprograms.h 中补充声明。
 * ==============================================================*/
#include "subprograms.h"
