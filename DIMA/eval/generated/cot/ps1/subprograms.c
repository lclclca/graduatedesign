#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include <string.h>

#include "subprograms.h"
#include "globals.h"
#include "gtypes.h"

/* ─────────────────────────────────────────────────────────────
 * commandboard_receiveinput_spg
 * 从队列端口 pr1queueingin 接收一条消息，
 * 将结果写入黑板 bb_acc1，并通过 out_val 返回给调用方。
 * ───────────────────────────────────────────────────────────── */
void commandboard_receiveinput_spg(integer *out_val)
{
    RETURN_CODE_TYPE    ret;
    MESSAGE_SIZE_TYPE   len;
    integer             val = 0;

    RECEIVE_QUEUING_MESSAGE(
        ps1_pr1queueingin_id,
        0,                              /* timeout = 0：非阻塞轮询 */
        (MESSAGE_ADDR_TYPE)&val,
        &len,
        &ret);
    /* NO_ACTION 表示队列为空，属于正常情况，不视为错误 */
    if (ret == NO_ERROR) {
        DISPLAY_BLACKBOARD(
            ps1_bb_acc1_id,
            (MESSAGE_ADDR_TYPE)&val,
            sizeof(integer),
            &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD bb_acc1 in receiveinput_spg", ret);
        *out_val = val;
    } else if (ret == NO_ACTION) {
        /* 队列空：保持 out_val 不变 */
    } else {
        CHECK_CODE("RECEIVE_QUEUING_MESSAGE pr1queueingin", ret);
    }
}

/* ─────────────────────────────────────────────────────────────
 * commandboard_printinfos_spg
 * 从黑板 bb_acc1 读取最新值，打印诊断信息，
 * 并将该值写出到采样端口 pr1samplingout。
 * ───────────────────────────────────────────────────────────── */
void commandboard_printinfos_spg(integer *in_val)
{
    RETURN_CODE_TYPE  ret;
    MESSAGE_SIZE_TYPE len;
    integer           bb_val = 0;

    READ_BLACKBOARD(
        ps1_bb_acc1_id,
        0,                              /* timeout = 0 */
        (MESSAGE_ADDR_TYPE)&bb_val,
        &len,
        &ret);
    CHECK_CODE("READ_BLACKBOARD bb_acc1 in printinfos_spg", ret);

    printf("[ps1][task12] bb_acc1 = %d\n", (int)bb_val);

    WRITE_SAMPLING_MESSAGE(
        ps1_pr1samplingout_id,
        (MESSAGE_ADDR_TYPE)&bb_val,
        sizeof(integer),
        &ret);
    CHECK_CODE("WRITE_SAMPLING_MESSAGE pr1samplingout", ret);

    if (in_val != NULL) {
        *in_val = bb_val;
    }
}

/* ─────────────────────────────────────────────────────────────
 * calculate_spg
 * 核心计算子程序：
 *   1. 从 bb_t4t3 读取 task14 发来的结果；
 *   2. 对 sensor_val 执行简单累加计算；
 *   3. 将结果写入 bb_t3t4（供 task14 读取）；
 *   4. 通过 result_val 返回计算结果。
 * ───────────────────────────────────────────────────────────── */
void calculate_spg(integer *sensor_val, integer *result_val)
{
    RETURN_CODE_TYPE  ret;
    MESSAGE_SIZE_TYPE len;
    integer           t4_val   = 0;
    integer           computed = 0;

    /* 读取 task14 反馈值 */
    READ_BLACKBOARD(
        ps1_bb_t4t3_id,
        0,
        (MESSAGE_ADDR_TYPE)&t4_val,
        &len,
        &ret);
    /* 首次运行时黑板可能为空，忽略 NOT_AVAILABLE */
    if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
        CHECK_CODE("READ_BLACKBOARD bb_t4t3 in calculate_spg", ret);
    }

    /* 核心计算：累加传感器值与 task14 反馈 */
    computed = (sensor_val != NULL ? *sensor_val : 0) + t4_val;

    /* 将计算结果写入 bb_t3t4 供 task14 读取 */
    DISPLAY_BLACKBOARD(
        ps1_bb_t3t4_id,
        (MESSAGE_ADDR_TYPE)&computed,
        sizeof(integer),
        &ret);
    CHECK_CODE("DISPLAY_BLACKBOARD bb_t3t4 in calculate_spg", ret);

    if (result_val != NULL) {
        *result_val = computed;
    }
}