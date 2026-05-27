#ifndef SUBPROGRAMS_H
#define SUBPROGRAMS_H

#include "gtypes.h"

/* ── 子程序声明 ── */

/**
 * commandboard_receiveinput_spg
 * 从队列端口读取一条输入消息，写入 bb_acc1 黑板。
 */
void commandboard_receiveinput_spg(integer *out_val);

/**
 * commandboard_printinfos_spg
 * 从 bb_acc1 黑板读取最新值并打印诊断信息，
 * 同时将结果写出到采样端口。
 */
void commandboard_printinfos_spg(integer *in_val);

/**
 * calculate_spg
 * 执行核心计算：读 buf_sensor，写 buf_t3tot4；
 * 读 bb_t4t3，写 bb_t3t4。
 */
void calculate_spg(integer *sensor_val, integer *result_val);

#endif /* SUBPROGRAMS_H */