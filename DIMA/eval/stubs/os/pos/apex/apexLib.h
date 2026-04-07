/*
 * APEX API Stub Header for Static Analysis / Compile Check
 *
 * 用途：在无真实 ACoreOS653 环境的 Windows/Linux 上对生成代码进行编译检查。
 * 包含 ARINC 653 Part 1 的最小类型定义和宏，足以通过 gcc/cppcheck 语法检查。
 *
 * 用法（Windows MSYS2/MinGW 或 WSL）：
 *   gcc -fsyntax-only -DIMA2C_RUNTIME_ACoreOS653=1 \
 *       -I DIMA/eval/stubs \
 *       DIMA/eval/generated/fewshot/pb/*.c
 *
 * 用法（cppcheck，无需编译器）：
 *   cppcheck --enable=all --suppress=missingInclude \
 *       DIMA/eval/generated/fewshot/pb/
 */

#ifndef __STUB_APEX_LIB_H__
#define __STUB_APEX_LIB_H__

#include <stddef.h>
#include <string.h>
#include <stdio.h>

/* ── 基本类型 ───────────────────────────────────────────────────────────── */

typedef int                 RETURN_CODE_TYPE;
typedef unsigned int        MESSAGE_SIZE_TYPE;
typedef void *              MESSAGE_ADDR_TYPE;
typedef long long           SYSTEM_TIME_TYPE;

typedef int                 PROCESS_ID_TYPE;
typedef int                 SAMPLING_PORT_ID_TYPE;
typedef int                 QUEUING_PORT_ID_TYPE;
typedef int                 BLACKBOARD_ID_TYPE;
typedef int                 BUFFER_ID_TYPE;
typedef int                 SEMAPHORE_ID_TYPE;
typedef int                 EVENT_ID_TYPE;
typedef int                 VALIDITY_TYPE;

typedef unsigned int        STACK_SIZE_TYPE;
typedef unsigned int        PRIORITY_TYPE;
typedef unsigned int        NUM_CORES_TYPE;

/* ── 枚举 / 常量 ────────────────────────────────────────────────────────── */

/* RETURN_CODE_TYPE */
#define NO_ERROR        0
#define NO_ACTION       1
#define NOT_AVAILABLE   2
#define INVALID_PARAM   3
#define INVALID_CONFIG  4
#define INVALID_MODE    5
#define TIMED_OUT       6

/* PORT_DIRECTION_TYPE */
#define SOURCE          0
#define DESTINATION     1
typedef int PORT_DIRECTION_TYPE;

/* QUEUING_DISCIPLINE_TYPE */
#define FIFO            0
#define PRIORITY        1
typedef int QUEUING_DISCIPLINE_TYPE;

/* DEADLINE_TYPE */
#define SOFT            0
#define HARD            1
typedef int DEADLINE_TYPE;

/* OPERATING_MODE_TYPE */
#define IDLE            0
#define COLD_START      1
#define WARM_START      2
#define NORMAL          3
typedef int OPERATING_MODE_TYPE;

/* VALIDITY */
#define INVALID         0
#define VALID           1

/* WAITING_RANGE_TYPE / INFINITE_TIME */
#define INFINITE_TIME   (-1LL)

/* ── 结构体 ─────────────────────────────────────────────────────────────── */

#define MAX_NAME_LENGTH 32

typedef char NAME_TYPE[MAX_NAME_LENGTH];

typedef struct {
    NAME_TYPE           NAME;
    void *              (*ENTRY_POINT)(void);
    STACK_SIZE_TYPE     STACK_SIZE;
    PRIORITY_TYPE       BASE_PRIORITY;
    SYSTEM_TIME_TYPE    PERIOD;
    SYSTEM_TIME_TYPE    TIME_CAPACITY;
    DEADLINE_TYPE       DEADLINE;
} PROCESS_ATTRIBUTE_TYPE;

typedef struct {
    int     ERROR_IDENTIFIER;
    int     FAILED_PROCESS_ID;
    int     SYSTEM_STATE;
    int     LENGTH;
    char    DESCRIPTION[64];
} ERROR_STATUS_TYPE;

/* ── APEX 进程管理 ──────────────────────────────────────────────────────── */

#define CREATE_PROCESS(attr, pid, ret)          (*(ret) = NO_ERROR)
#define START(pid, ret)                         (*(ret) = NO_ERROR)
#define STOP(pid, ret)                          (*(ret) = NO_ERROR)
#define SUSPEND(timeout, ret)                   (*(ret) = NO_ERROR)
#define RESUME(pid, ret)                        (*(ret) = NO_ERROR)
#define PERIODIC_WAIT(ret)                      (*(ret) = NO_ERROR)
#define TIMED_WAIT(delay, ret)                  (*(ret) = NO_ERROR)

/* ── 分区模式 ───────────────────────────────────────────────────────────── */

#define SET_PARTITION_MODE(mode, ret)           (*(ret) = NO_ERROR)
#define GET_PARTITION_STATUS(status, ret)       (*(ret) = NO_ERROR)

/* ── 采样端口 ───────────────────────────────────────────────────────────── */

#define CREATE_SAMPLING_PORT(name, size, dir, period, pid, ret) \
    (*(ret) = NO_ERROR)

#define WRITE_SAMPLING_MESSAGE(pid, addr, size, ret) \
    (*(ret) = NO_ERROR)

#define READ_SAMPLING_MESSAGE(pid, addr, size, valid, ret) \
    (*(ret) = NO_ERROR, *(valid) = VALID)

/* ── 队列端口 ───────────────────────────────────────────────────────────── */

#define CREATE_QUEUING_PORT(name, msg_size, max_nb, dir, disc, pid, ret) \
    (*(ret) = NO_ERROR)

#define SEND_QUEUING_MESSAGE(pid, addr, size, timeout, ret) \
    (*(ret) = NO_ERROR)

#define RECEIVE_QUEUING_MESSAGE(pid, timeout, addr, size, ret) \
    (*(ret) = NO_ERROR)

/* ── 黑板 ───────────────────────────────────────────────────────────────── */

#define CREATE_BLACKBOARD(name, size, bid, ret) \
    (*(ret) = NO_ERROR)

#define DISPLAY_BLACKBOARD(bid, addr, size, ret) \
    (*(ret) = NO_ERROR)

#define READ_BLACKBOARD(bid, timeout, addr, size, ret) \
    (*(ret) = NO_ERROR)

/* ── 缓冲区 ─────────────────────────────────────────────────────────────── */

#define CREATE_BUFFER(name, size, max_nb, disc, bid, ret) \
    (*(ret) = NO_ERROR)

#define SEND_BUFFER(bid, addr, size, timeout, ret) \
    (*(ret) = NO_ERROR)

#define RECEIVE_BUFFER(bid, timeout, addr, size, ret) \
    (*(ret) = NO_ERROR)

/* ── 信号量 ─────────────────────────────────────────────────────────────── */

#define CREATE_SEMAPHORE(name, cur, max, disc, sid, ret) \
    (*(ret) = NO_ERROR)

#define WAIT_SEMAPHORE(sid, timeout, ret)       (*(ret) = NO_ERROR)
#define SIGNAL_SEMAPHORE(sid, ret)              (*(ret) = NO_ERROR)

/* ── 事件 ───────────────────────────────────────────────────────────────── */

#define CREATE_EVENT(name, eid, ret)            (*(ret) = NO_ERROR)
#define SET_EVENT(eid, ret)                     (*(ret) = NO_ERROR)
#define RESET_EVENT(eid, ret)                   (*(ret) = NO_ERROR)
#define WAIT_EVENT(eid, timeout, ret)           (*(ret) = NO_ERROR)

/* ── HM 回调（声明，由生成代码定义） ───────────────────────────────────── */

extern void module_HM_callback(ERROR_STATUS_TYPE *error_status);
extern void partition_HM_callback(ERROR_STATUS_TYPE *error_status);

/* ── codeToStr 辅助（globals.h 中已定义为 static，此处不重复） ─────────── */

#endif /* __STUB_APEX_LIB_H__ */
