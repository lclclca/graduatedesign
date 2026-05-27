#ifndef __IMA2C_GENERATED_SUBPROGRAMS_H_
#define __IMA2C_GENERATED_SUBPROGRAMS_H_

/* ==============================================================
 *  FILE:    subprograms.h
 *  ROLE:    用户自定义子程序接口声明（Subprogram Interface Declarations）
 *
 *  IMA 架构层次：应用层（Application Layer）
 *  IMA 分区：ps1 (P1)     模块：M1 (arinckernel.module1)
 *  对应 AADL 文件：DIMA_subprograms.aadl
 *    AADL subprogram 构造定义了无副作用的纯计算单元，
 *    由 thread（任务）在自身执行循环中调用。
 *
 *  AADL subprogram → C 函数映射：
 *    subprograms::commandboard_receiveinput_spg
 *      调用者：task11 (sensor → sampling_out), task13 (bb_sem → buf_t3tot4)
 *    subprograms::commandboard_printinfos_spg
 *      调用者：task11 (打印 pr1samplingout 的值)
 *    subprograms::calculate_spg
 *      调用者：task14 (bb_t3t4 → bb_t4t3)，对应 AADL 中的子程序调用连接
 *        C1: task4_blackboard_read → calculate_spg.data_in
 *        C2: calculate_spg.data_out → task4_blackboard_write
 * ==============================================================*/

#include "gtypes.h"

/* 对应 AADL: subprograms::commandboard_receiveinput_spg */
void commandboard_receiveinput_spg(integer *data_in, integer *data_out);

/* 对应 AADL: subprograms::commandboard_printinfos_spg */
void commandboard_printinfos_spg(integer *data_in);

/* 对应 AADL: subprograms::calculate_spg */
void calculate_spg(integer *data_in, integer *data_out);

#endif
