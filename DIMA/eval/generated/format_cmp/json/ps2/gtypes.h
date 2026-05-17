/*============================================================
 * gtypes.h
 * 全局类型定义 —— ps2 分区
 * 模块: M1   分区: ps2
 * 遵循 ARINC 653 Part 1
 *============================================================*/
#ifndef GTYPES_H
#define GTYPES_H

#include <apex/apex.h>
#include <apex/apexLib.h>

/*------------------------------------------------------------
 * 基础布尔类型
 *------------------------------------------------------------*/
typedef enum {
    G_FALSE = 0,
    G_TRUE  = 1
} BOOL_TYPE;

/*------------------------------------------------------------
 * 应用层消息结构
 *------------------------------------------------------------*/
/* 采样端口消息：来自上游的输入数据 */
typedef struct {
    APEX_INTEGER  sensor_id;
    APEX_INTEGER  raw_value;
    APEX_UNSIGNED timestamp_ms;
} SAMPLING_IN_MSG_TYPE;

/* 队列端口消息：发往下游的指令数据 */
typedef struct {
    APEX_INTEGER  command_id;
    APEX_INTEGER  param1;
    APEX_INTEGER  param2;
    APEX_UNSIGNED sequence_no;
} QUEUING_OUT_MSG_TYPE;

/*------------------------------------------------------------
 * 黑板共享数据结构
 *------------------------------------------------------------*/
/* bb_acc3: 累加器数据（task21 写，task22/task23 读） */
typedef struct {
    APEX_INTEGER  acc_value;
    APEX_UNSIGNED update_count;
} BB_ACC3_TYPE;

/* bb_t2t3: task22 → task23 单向通道 */
typedef struct {
    APEX_INTEGER  data;
    BOOL_TYPE     valid;
} BB_T2T3_TYPE;

/* bb_t3t2: task23 → task22 单向通道 */
typedef struct {
    APEX_INTEGER  data;
    BOOL_TYPE     valid;
} BB_T3T2_TYPE;

/*------------------------------------------------------------
 * 缓冲区共享数据结构
 *------------------------------------------------------------*/
/* buf_order: 指令缓冲 */
typedef struct {
    APEX_INTEGER  order_id;
    APEX_INTEGER  priority;
    APEX_UNSIGNED flags;
} BUF_ORDER_TYPE;

/* buf_t2tot3: task22 → task23 数据流缓冲 */
typedef struct {
    APEX_INTEGER  payload[8];
    APEX_INTEGER  length;
} BUF_T2TOT3_TYPE;

/* buf_t3tot2: task23 → task22 数据流缓冲 */
typedef struct {
    APEX_INTEGER  payload[8];
    APEX_INTEGER  length;
} BUF_T3TOT2_TYPE;

/*------------------------------------------------------------
 * 返回码检查宏
 *------------------------------------------------------------*/
#define CHECK_CODE(operation, ret_code)                                 \
    do {                                                                \
        if ((ret_code) != NO_ERROR) {                                   \
            /* 在实际产品中可调用 HM 上报；此处挂起分区 */             \
            RAISE_APPLICATION_ERROR(APPLICATION_ERROR,                  \
                (MESSAGE_ADDR_TYPE)(operation),                         \
                (ERROR_CODE_TYPE)(ret_code));                           \
        }                                                               \
    } while (0)

#endif /* GTYPES_H */