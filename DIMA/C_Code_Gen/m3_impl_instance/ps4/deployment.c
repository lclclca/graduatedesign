/* ==============================================================
 *  FILE:    deployment.c
 *  ROLE:    ARINC 653 模块部署配置编译单元
 *
 *  IMA 架构层次：分区级（Partition Level）
 *  IMA 分区：ps4 (P4)          模块：M3 (arinckernel.module3)
 *  对应 AADL 文件：DIMA_Processor.aadl（处理器/分区绑定），DIMA_models.aadl（模块调度）
 *
 *  【为何此文件只有一行 #include？】
 *  deployment.h 中的内容全部是 #define 编译期常量（宏）。
 *  宏不产生任何可链接符号，因此不需要 .c 提供函数体或变量定义。
 *  本文件存在的唯一作用：满足构建系统（Makefile）要求每个模块
 *  都有可编译的 .c 文件。编译产生的 deployment.o 是空的目标文件。
 * ==============================================================*/
#include "deployment.h"
