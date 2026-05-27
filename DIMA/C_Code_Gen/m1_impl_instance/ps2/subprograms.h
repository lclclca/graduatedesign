#ifndef __IMA2C_GENERATED_SUBPROGRAMS_H_
#define __IMA2C_GENERATED_SUBPROGRAMS_H_

/* ==============================================================
 *  FILE:    subprograms.h
 *  ROLE:    用户自定义子程序接口声明（Subprogram Interface Declarations）
 *
 *  IMA 架构层次：应用层（Application Layer）
 *  IMA 分区：ps2 (P2)     模块：M1 (arinckernel.module1)
 *  对应 AADL 文件：DIMA_subprograms.aadl
 *    AADL subprogram 构造定义了无副作用的纯计算单元，
 *    由 thread（任务）在自身执行循环中调用。
 *
 *  本文件声明 subprograms.c 中定义的所有子程序接口，
 *  activity.c 包含此头文件以调用这些子程序。
 *
 *  AADL subprogram → C 函数映射：
 *    subprograms::commandboard_receiveinput_spg → void commandboard_receiveinput_spg(...)
 *      参数: data_in (in)  — 来自端口/黑板的读取值
 *            data_out (out) — 处理结果，写入端口/黑板
 *    subprograms::commandboard_printinfos_spg   → void commandboard_printinfos_spg(...)
 *      参数: data_in (in)  — 要打印/记录的数据
 * ==============================================================*/

#include "gtypes.h"

/* 对应 AADL: subprograms::commandboard_receiveinput_spg
 * 在 task21 中调用：接收采样端口数据 → 输出给黑板 bb_acc3
 */
void commandboard_receiveinput_spg(integer *data_in, integer *data_out);

/* 对应 AADL: subprograms::commandboard_printinfos_spg
 * 在 task21 中调用：打印/记录 bb_acc3 的值
 */
void commandboard_printinfos_spg(integer *data_in);

#endif
