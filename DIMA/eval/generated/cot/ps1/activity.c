#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include <string.h>

#include "activity.h"
#include "globals.h"
#include "gtypes.h"
#include "subprograms.h"

/* ═══════════════════════════════════════════════════════════════
 * task11_job
 * 周期：25 ms | 优先级：2
 * 行为：
 *   - 调用 commandboard_receiveinput_spg 从队列端口接收数据
 *   - 将接收到的传感器原始值写入 buf_sensor（供 task13 消费）
 *   - 同时更新 bb_acc2（累计计数/辅助状态）
 * ═══════════════════════════════════════════════════════════════ */
void *task11_job(void *arg)
{
    RETURN_CODE_TYPE  ret;
    integer           received_val = 0;
    integer           acc2_val     = 0;
    MESSAGE_SIZE_TYPE len;

    (void)arg;

    while (1) {
        /* 1. 从队列端口读取输入，写入 bb_acc1（子程序内部完成） */
        commandboard_receiveinput_spg(&received_val);

        /* 2. 将原始值写入 buf_sensor，供 task13 读取 */
        WRITE_BUFFER(
            ps1_buf_sensor_id,
            (MESSAGE_ADDR_TYPE)&received_val,
            sizeof(integer),
            0,          /* timeout = 0：非阻塞 */
            &ret);
        /* TIMED_OUT / NOT_AVAILABLE 时缓冲区满，丢弃最旧策略由应用层决定 */
        if (ret != NO_ERROR && ret != TIMED_OUT) {
            CHECK_CODE("WRITE_BUFFER buf_sensor in task11", ret);
        }

        /* 3. 读取并递增 bb_acc2（简单心跳计数） */
        READ_BLACKBOARD(
            ps1_bb_acc2_id,
            0,
            (MESSAGE_ADDR_TYPE)&acc2_val,
            &len,
            &ret);
        if (ret == NO_ERROR || ret == NOT_AVAILABLE) {
            acc2_val++;
            DISPLAY_BLACKBOARD(
                ps1_bb_acc2_id,
                (MESSAGE_ADDR_TYPE)&acc2_val,
                sizeof(integer),
                &ret);
            CHECK_CODE("DISPLAY_BLACKBOARD bb_acc2 in task11", ret);
        }

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task11", ret);
    }

    return NULL;
}

/* ═══════════════════════════════════════════════════════════════
 * task12_job
 * 周期：50 ms | 优先级：3
 * 行为：
 *   - 调用 commandboard_printinfos_spg 读取 bb_acc1 并发布到采样端口
 * ═══════════════════════════════════════════════════════════════ */
void *task12_job(void *arg)
{
    RETURN_CODE_TYPE ret;
    integer          out_val = 0;

    (void)arg;

    while (1) {
        /* 读 bb_acc1，打印诊断，写采样端口 pr1samplingout */
        commandboard_printinfos_spg(&out_val);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task12", ret);
    }

    return NULL;
}

/* ═══════════════════════════════════════════════════════════════
 * task13_job
 * 周期：50 ms | 优先级：4
 * 行为：
 *   - 从 buf_sensor 读取 task11 写入的传感器值
 *   - 调用 calculate_spg 执行核心计算（读 bb_t4t3，写 bb_t3t4）
 *   - 将计算结果写入 buf_t3tot4（供 task14 读取）
 *   - 使用 bb_sem 作为同步信号量，通知 task14 数据就绪
 * ═══════════════════════════════════════════════════════════════ */
void *task13_job(void *arg)
{
    RETURN_CODE_TYPE  ret;
    integer           sensor_val = 0;
    integer           result_val = 0;
    integer           sem_val    = 1;
    MESSAGE_SIZE_TYPE len;

    (void)arg;

    while (1) {
        /* 1. 从 buf_sensor 读取传感器原始数据 */
        READ_BUFFER(
            ps1_buf_sensor_id,
            0,
            (MESSAGE_ADDR_TYPE)&sensor_val,
            &len,
            &ret);
        if (ret != NO_ERROR && ret != TIMED_OUT && ret != NOT_AVAILABLE) {
            CHECK_CODE("READ_BUFFER buf_sensor in task13", ret);
        }

        /* 2. 核心计算（内部读 bb_t4t3，写 bb_t3t4） */
        calculate_spg(&sensor_val, &result_val);

        /* 3. 将结果写入 buf_t3tot4，供 task14 消费 */
        WRITE_BUFFER(
            ps1_buf_t3tot4_id,
            (MESSAGE_ADDR_TYPE)&result_val,
            sizeof(integer),
            0,
            &ret);
        if (ret != NO_ERROR && ret != TIMED_OUT) {
            CHECK_CODE("WRITE_BUFFER buf_t3tot4 in task13", ret);
        }

        /* 4. 置位 bb_sem，通知 task14 数据就绪 */
        DISPLAY_BLACKBOARD(
            ps1_bb_sem_id,
            (MESSAGE_ADDR_TYPE)&sem_val,
            sizeof(integer),
            &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD bb_sem in task13", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task13", ret);
    }

    return NULL;
}

/* ═══════════════════════════════════════════════════════════════
 * task14_job
 * 周期：50 ms | 优先级：5
 * 行为：
 *   - 检查 bb_sem，若 task13 已置位则读取 buf_t3tot4
 *   - 读 bb_t3t4（task13 的计算结果黑板）
 *   - 执行本地处理后写 bb_t4t3（反馈给 task13）
 *   - 将最终结果写入 buf_t4tot3
 * ═══════════════════════════════════════════════════════════════ */
void *task14_job(void *arg)
{
    RETURN_CODE_TYPE  ret;
    integer           sem_val    = 0;
    integer           t3_data    = 0;
    integer           t3t4_bb    = 0;
    integer           feedback   = 0;
    integer           sem_clear  = 0;
    MESSAGE_SIZE_TYPE len;

    (void)arg;

    while (1) {
        /* 1. 读取信号量黑板，检查 task13 是否已就绪 */
        READ_BLACKBOARD(
            ps1_bb_sem_id,
            0,
            (MESSAGE_ADDR_TYPE)&sem_val,
            &len,
            &ret);

        if (ret == NO_ERROR && sem_val != 0) {
            /* 2. 清除信号量 */
            DISPLAY_BLACKBOARD(
                ps1_bb_sem_id,
                (MESSAGE_ADDR_TYPE)&sem_clear,
                sizeof(integer),
                &ret);
            CHECK_CODE("DISPLAY_BLACKBOARD bb_sem clear in task14", ret);

            /* 3. 从 buf_t3tot4 读取 task13 的计算结果 */
            READ_BUFFER(
                ps1_buf_t3tot4_id,
                0,
                (MESSAGE_ADDR_TYPE)&t3_data,
                &len,
                &ret);
            if (ret != NO_ERROR && ret != TIMED_OUT && ret != NOT_AVAILABLE) {
                CHECK_CODE("READ_BUFFER buf_t3tot4 in task14", ret);
            }

            /* 4. 读取 bb_t3t4（task13 写的黑板） */
            READ_BLACKBOARD(
                ps1_bb_t3t4_id,
                0,
                (MESSAGE_ADDR_TYPE)&t3t4_bb,
                &len,
                &ret);
            if (ret != NO_ERROR && ret != NOT_AVAILABLE) {
                CHECK_CODE("READ_BLACKBOARD bb_t3t4 in task14", ret);
            }

            /* 5. 本地处理：反馈值 = buf 数据 + 黑板数据之差（示例逻辑） */
            feedback = t3_data - t3t4_bb;

            /* 6. 将反馈写入 bb_t4t3，供 task13 下次迭代读取 */
            DISPLAY_BLACKBOARD(
                ps1_bb_t4t3_id,
                (MESSAGE_ADDR_TYPE)&feedback,
                sizeof(integer),
                &ret);
            CHECK_CODE("DISPLAY_BLACKBOARD bb_t4t3 in task14", ret);

            /* 7. 将最终结果写入 buf_t4tot3 */
            WRITE_BUFFER(
                ps1_buf_t4tot3_id,
                (MESSAGE_ADDR_TYPE)&feedback,
                sizeof(integer),
                0,
                &ret);
            if (ret != NO_ERROR && ret != TIMED_OUT) {
                CHECK_CODE("WRITE_BUFFER buf_t4tot3 in task14", ret);
            }
        }

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task14", ret);
    }

    return NULL;
}