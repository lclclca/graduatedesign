/* ==============================================================
 *  FILE:    subprograms.c
 *  ROLE:    用户自定义子程序实现（Subprogram Implementations）
 *
 *  IMA 架构层次：应用层（Application Layer）
 *  IMA 分区：ps3 (P3)     模块：M2 (arinckernel.module2)
 *  对应 AADL 文件：DIMA_subprograms.aadl
 *
 *  【注意】ps3 分区的任务不调用任何 AADL subprogram。
 *  task31/task32/task33 只执行 RECEIVE_QUEUING_MESSAGE（读队列端口），
 *  收到消息后直接 PERIODIC_WAIT，不进行用户定义的计算。
 *  若未来需要添加计算逻辑，在此文件中添加函数定义，
 *  并在 subprograms.h 中补充声明。
 * ==============================================================*/
#include "subprograms.h"
