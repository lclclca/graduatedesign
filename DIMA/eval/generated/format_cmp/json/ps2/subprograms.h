/*============================================================
 * subprograms.h
 * 子程序声明 —— ps2 分区
 *============================================================*/
#ifndef SUBPROGRAMS_H
#define SUBPROGRAMS_H

#include "gtypes.h"

/*------------------------------------------------------------
 * commandboard_receiveinput_spg
 *   读取采样端口消息，返回是否成功取得有效数据
 *------------------------------------------------------------*/
BOOL_TYPE commandboard_receiveinput_spg(SAMPLING_IN_MSG_TYPE *out_msg);

/*------------------------------------------------------------
 * commandboard_printinfos_spg
 *   格式化打印当前分区状态（调试/维护用）
 *------------------------------------------------------------*/
void commandboard_printinfos_spg(const SAMPLING_IN_MSG_TYPE  *in_msg,
                                 const BB_ACC3_TYPE          *acc3,
                                 const BB_T2T3_TYPE          *t2t3,
                                 const BB_T3T2_TYPE          *t3t2);

#endif /* SUBPROGRAMS_H */