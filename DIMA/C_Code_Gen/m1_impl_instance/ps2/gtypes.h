#ifndef __IMA2C_GENERATED_GTYPES_H__
#define __IMA2C_GENERATED_GTYPES_H__

/* ==============================================================
 *  FILE:    gtypes.h
 *  ROLE:    全局数据类型定义（Global Type Definitions）
 *
 *  IMA 架构层次：数据层（Data Layer）
 *  IMA 分区：ps2 (P2)     模块：M1
 *  对应 AADL 文件：
 *    - DIMA_datatype.aadl     ：定义 protected_data（并发保护数据类型）
 *                               Data_Representation => Integer，Data_Size => 4 bytes
 *    - DIMA_subprograms.aadl  ：子程序参数类型 Base_Types::Integer
 *    - DIMA_threads.aadl      ：线程端口数据类型 Base_Types::Integer
 *
 *  AADL 类型 → C 类型映射：
 *    Base_Types::Integer       → typedef int integer
 *    DIMA_datatype::protected_data → integer（并发安全由 BLACKBOARD 保证）
 *
 *  所有分区间/分区内消息均使用 integer 类型，
 *  消息大小固定为 sizeof(integer) = 4 字节，
 *  保证跨分区通信的消息大小一致性（ARINC 653 要求收发方 MAX_MESSAGE_SIZE 相同）。
 * ==============================================================*/

#include <os/pos/apex/apexLib.h>

/* 对应 AADL: Base_Types::Integer / DIMA_datatype::protected_data */
typedef int integer;

#endif
