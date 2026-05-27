/* ==============================================================
 *  FILE:    globals.c
 *  ROLE:    全局工具宏编译单元
 *
 *  IMA 架构层次：分区级（Partition Level）
 *  IMA 分区：ps4 (P4)          模块：M3
 *  对应 AADL 文件：无直接对应（运行时基础设施，为 APEX API 调用提供错误检查宏）
 *
 *  【为何此文件只有一行 #include？】
 *  globals.h 中的内容全部是 #define 宏和 static 函数（codeToStr）。
 *  - #define 宏：不产生任何链接符号
 *  - static 函数：internal linkage，每个包含 globals.h 的 .c 都获得
 *    自己的私有副本，不需要在此 .c 中重复定义。
 *  本文件存在的唯一作用：满足构建系统的编译单元要求。
 * ==============================================================*/
#include "globals.h"
