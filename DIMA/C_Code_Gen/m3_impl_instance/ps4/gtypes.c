/* ==============================================================
 *  FILE:    gtypes.c
 *  ROLE:    全局数据类型定义编译单元
 *
 *  IMA 架构层次：分区级（Partition Level）
 *  IMA 分区：ps4 (P4)          模块：M3
 *  对应 AADL 文件：DIMA_datatype.aadl（protected_data 类型），DIMA_subprograms.aadl（参数类型）
 *
 *  【为何此文件只有一行 #include？】
 *  gtypes.h 中只有 typedef 声明。typedef 是类型别名，
 *  不产生任何可链接符号（无函数体、无全局变量）。
 *  typedef 必须放在头文件中，所有需要该类型的 .c 都包含此头文件。
 *  本文件存在的唯一作用：满足构建系统的编译单元要求。
 * ==============================================================*/
#include "gtypes.h"
