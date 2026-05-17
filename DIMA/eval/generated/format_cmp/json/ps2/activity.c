/*============================================================
 * activity.c
 * 任务逻辑实现 —— ps2 分区
 *
 * task21 (50 ms, P2): 读采样端口 → 更新 bb_acc3 → 写 buf_order
 * task22 (50 ms, P3): 读 bb_acc3 → 读 buf_t3tot2 → 发队列消息
 *                     → 写 bb_t2t3 → 写 buf_t2tot3
 * task23 (100 ms,P4): 读 bb_t2t3 → 读 buf_t2tot3 → 计算
 *                     → 写 bb_t3t2 → 写 buf_t3tot2
 *============================================================*/
#include <string.h>
#include "activity.h"
#include "globals.h"
#include "deployment.h"
#include "subprograms.h"
#include "gtypes.h"

/* ============================================================
 * task21_entry
 * 周期：50 ms  优先级：2
 * 职责：
 *   1. 调用子程序读取采样端口
 *   2. 累加传感器值并更新 bb_acc3（DISPLAY_BLACKBOARD）
 *   3. 将指令写入 buf_order（SEND_BUFFER）
 * ============================================================*/
void task21_entry(void)
{
    RETURN_CODE_TYPE     ret;
    SAMPLING_IN_MSG_TYPE in_msg;
    BB_ACC3_TYPE         acc3_data;
    BUF_ORDER_TYPE       order_data;
    MESSAGE_SIZE_TYPE    rx_len = 0;
    BOOL_TYPE            got_sample;

    /* 初始化累加器 */
    memset(&acc3_data, 0, sizeof(acc3_data));

    for (;;) {
        /*-- 1. 读采样端口 --*/
        got_sample = commandboard_receiveinput_spg(&in_msg);

        /*-- 2. 更新黑板 bb_acc3 --*/
        if (got_sample == G_TRUE) {
            acc3_data.acc_value   += in_msg.raw_value;
            acc3_data.update_count++;
        }

        DISPLAY_BLACKBOARD(
            g_bb_acc3_id,
            (MESSAGE_ADDR_TYPE)&acc3_data,
            (MESSAGE_SIZE_TYPE)sizeof(acc3_data),
            &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD bb_acc3", ret);

        /*-- 3. 将指令压入 buf_order --*/
        if (got_sample == G_TRUE) {
            order_data.order_id = (APEX_INTEGER)in_msg.sensor_id;
            order_data.priority = 1;
            order_data.flags    = (APEX_UNSIGNED)(acc3_data.update_count & 0xFFU);

            SEND_BUFFER(
                g_buf_order_id,
                (MESSAGE_ADDR_TYPE)&order_data,
                (MESSAGE_SIZE_TYPE)sizeof(order_data),
                (SYSTEM_TIME_TYPE)SEND_TIMEOUT_NS,
                &ret);
            /* FULL 时丢弃最旧条目属业务决策，此处仅检查严重错误 */
            if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
                CHECK_CODE("SEND_BUFFER buf_order", ret);
            }
        }

        /*-- 等待下一周期 --*/
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task21", ret);

        (void)rx_len; /* 消除未使用警告 */
    }
}

/* ============================================================
 * task22_entry
 * 周期：50 ms  优先级：3
 * 职责：
 *   1. 读黑板 bb_acc3（READ_BLACKBOARD）
 *   2. 读缓冲区 buf_t3tot2（RECEIVE_BUFFER）
 *   3. 构造指令并通过队列端口发送（SEND_QUEUING_MESSAGE）
 *   4. 写黑板 bb_t2t3（DISPLAY_BLACKBOARD）
 *   5. 写缓冲区 buf_t2tot3（SEND_BUFFER）
 * ============================================================*/
void task22_entry(void)
{
    RETURN_CODE_TYPE      ret;
    BB_ACC3_TYPE          acc3_data;
    BB_T2T3_TYPE          t2t3_data;
    BB_T3T2_TYPE          t3t2_data;
    BUF_T3TOT2_TYPE       rx_t3t2;
    BUF_T2TOT3_TYPE       tx_t2t3;
    QUEUING_OUT_MSG_TYPE  q_msg;
    MESSAGE_SIZE_TYPE     rx_len = 0;
    static APEX_UNSIGNED  seq    = 0U;

    memset(&acc3_data, 0, sizeof(acc3_data));
    memset(&t2t3_data, 0, sizeof(t2t3_data));
    memset(&t3t2_data, 0, sizeof(t3t2_data));
    memset(&rx_t3t2,   0, sizeof(rx_t3t2));
    memset(&tx_t2t3,   0, sizeof(tx_t2t3));

    for (;;) {
        /*-- 1. 读黑板 bb_acc3 --*/
        READ_BLACKBOARD(
            g_bb_acc3_id,
            (MESSAGE_ADDR_TYPE)&acc3_data,
            &rx_len,
            &ret);
        if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
            CHECK_CODE("READ_BLACKBOARD bb_acc3", ret);
        }

        /*-- 2. 读缓冲区 buf_t3tot2（非阻塞） --*/
        RECEIVE_BUFFER(
            g_buf_t3tot2_id,
            (SYSTEM_TIME_TYPE)RECV_TIMEOUT_NS,
            (MESSAGE_ADDR_TYPE)&rx_t3t2,
            &rx_len,
            &ret);
        if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
            CHECK_CODE("RECEIVE_BUFFER buf_t3tot2", ret);
        }

        /*-- 3. 发送队列消息 pr2queueingout --*/
        seq++;
        q_msg.command_id  = acc3_data.acc_value;
        q_msg.param1      = rx_t3t2.payload[0];
        q_msg.param2      = rx_t3t2.length;
        q_msg.sequence_no = seq;

        SEND_QUEUING_MESSAGE(
            g_pr2queueingout_id,
            (MESSAGE_ADDR_TYPE)&q_msg,
            (MESSAGE_SIZE_TYPE)sizeof(q_msg),
            (SYSTEM_TIME_TYPE)SEND_TIMEOUT_NS,
            &ret);
        if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
            CHECK_CODE("SEND_QUEUING_MESSAGE pr2queueingout", ret);
        }

        /*-- 4. 写黑板 bb_t2t3 --*/
        t2t3_data.data  = acc3_data.acc_value + rx_t3t2.payload[0];
        t2t3_data.valid = G_TRUE;

        DISPLAY_BLACKBOARD(
            g_bb_t2t3_id,
            (MESSAGE_ADDR_TYPE)&t2t3_data,
            (MESSAGE_SIZE_TYPE)sizeof(t2t3_data),
            &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD bb_t2t3", ret);

        /*-- 5. 写缓冲区 buf_t2tot3 --*/
        tx_t2t3.payload[0] = t2t3_data.data;
        tx_t2t3.length     = 1;

        SEND_BUFFER(
            g_buf_t2tot3_id,
            (MESSAGE_ADDR_TYPE)&tx_t2t3,
            (MESSAGE_SIZE_TYPE)sizeof(tx_t2t3),
            (SYSTEM_TIME_TYPE)SEND_TIMEOUT_NS,
            &ret);
        if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
            CHECK_CODE("SEND_BUFFER buf_t2tot3", ret);
        }

        /*-- 等待下一周期 --*/
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task22", ret);
    }
}

/* ============================================================
 * task23_entry
 * 周期：100 ms  优先级：4
 * 职责：
 *   1. 读黑板 bb_t2t3（READ_BLACKBOARD）
 *   2. 读缓冲区 buf_t2tot3（RECEIVE_BUFFER）
 *   3. 综合计算
 *   4. 写黑板 bb_t3t2（DISPLAY_BLACKBOARD）
 *   5. 写缓冲区 buf_t3tot2（SEND_BUFFER）
 *   6. 调用打印子程序（调试）
 * ============================================================*/
void task23_entry(void)
{
    RETURN_CODE_TYPE  ret;
    BB_T2T3_TYPE      t2t3_data;
    BB_T3T2_TYPE      t3t2_data;
    BB_ACC3_TYPE      acc3_snap;
    BUF_T2TOT3_TYPE   rx_t2t3;
    BUF_T3TOT2_TYPE   tx_t3t2;
    MESSAGE_SIZE_TYPE rx_len = 0;

    memset(&t2t3_data, 0, sizeof(t2t3_data));
    memset(&t3t2_data, 0, sizeof(t3t2_data));
    memset(&acc3_snap, 0, sizeof(acc3_snap));
    memset(&rx_t2t3,   0, sizeof(rx_t2t3));
    memset(&tx_t3t2,   0, sizeof(tx_t3t2));

    for (;;) {
        /*-- 1. 读黑板 bb_t2t3 --*/
        READ_BLACKBOARD(
            g_bb_t2t3_id,
            (MESSAGE_ADDR_TYPE)&t2t3_data,
            &rx_len,
            &ret);
        if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
            CHECK_CODE("READ_BLACKBOARD bb_t2t3", ret);
        }

        /*-- 2. 读缓冲区 buf_t2tot3（非阻塞） --*/
        RECEIVE_BUFFER(
            g_buf_t2tot3_id,
            (SYSTEM_TIME_TYPE)RECV_TIMEOUT_NS,
            (MESSAGE_ADDR_TYPE)&rx_t2t3,
            &rx_len,
            &ret);
        if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
            CHECK_CODE("RECEIVE_BUFFER buf_t2tot3", ret);
        }

        /*-- 3. 计算结果并构造返回数据 --*/
        t3t2_data.data  = t2t3_data.data + rx_t2t3.payload[0];
        t3t2_data.valid = G_TRUE;

        /*-- 4. 写黑板 bb_t3t2 --*/
        DISPLAY_BLACKBOARD(
            g_bb_t3t2_id,
            (MESSAGE_ADDR_TYPE)&t3t2_data,
            (MESSAGE_SIZE_TYPE)sizeof(t3t2_data),
            &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD bb_t3t2", ret);

        /*-- 5. 写缓冲区 buf_t3tot2 --*/
        tx_t3t2.payload[0] = t3t2_data.data;
        tx_t3t2.length     = 1;

        SEND_BUFFER(
            g_buf_t3tot2_id,
            (MESSAGE_ADDR_TYPE)&tx_t3t2,
            (MESSAGE_SIZE_TYPE)sizeof(tx_t3t2),
            (SYSTEM_TIME_TYPE)SEND_TIMEOUT_NS,
            &ret);
        if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
            CHECK_CODE("SEND_BUFFER buf_t3tot2", ret);
        }

        /*-- 6. 快照 bb_acc3 用于调试打印 --*/
        READ_BLACKBOARD(
            g_bb_acc3_id,
            (MESSAGE_ADDR_TYPE)&acc3_snap,
            &rx_len,
            &ret);
        if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
            CHECK_CODE("READ_BLACKBOARD bb_acc3 (snap)", ret);
        }

        /* 打印调试信息（生产环境可条件编译关闭） */
        {
            SAMPLING_IN_MSG_TYPE dummy_in;
            memset(&dummy_in, 0, sizeof(dummy_in));
            commandboard_printinfos_spg(&dummy_in, &acc3_snap,
                                        &t2t3_data, &t3t2_data);
        }

        /*-- 等待下一周期 --*/
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task23", ret);
    }
}