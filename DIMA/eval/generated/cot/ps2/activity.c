#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include "activity.h"
#include "globals.h"
#include "deployment.h"
#include "subprograms.h"
#include "gtypes.h"

/*===========================================================================
 * task21_job
 * 周期：50ms，优先级：2
 * 通信行为：
 *   读  - pr2samplingin（采样端口），bb_t3t2，buf_t3tot2
 *   写  - bb_t2t3，buf_t2tot3
 *   子程序：commandboard_receiveinput_spg
 *===========================================================================*/
void *task21_job(void *arg)
{
    RETURN_CODE_TYPE      ret;
    integer               val;
    MESSAGE_SIZE_TYPE     len;
    VALIDITY_TYPE         validity;

    (void)arg;

    while (1) {
        /* 1. 从采样端口读取输入 */
        val = 0;
        READ_SAMPLING_MESSAGE(
            ps2_pr2samplingin_id,
            (MESSAGE_ADDR_TYPE)&val,
            &len,
            &validity,
            &ret);
        CHECK_CODE("READ_SAMPLING_MESSAGE pr2samplingin", ret);

        /* 2. 调用子程序处理输入 */
        commandboard_receiveinput_spg(&val);

        /* 3. 读黑板 bb_t3t2（接收 task22/task23 写入的数据） */
        {
            integer bb_val = 0;
            READ_BLACKBOARD(
                ps2_bb_t3t2_id,
                BB_TIMEOUT,
                (MESSAGE_ADDR_TYPE)&bb_val,
                &len,
                &ret);
            CHECK_CODE("READ_BLACKBOARD bb_t3t2", ret);
            val += bb_val;
        }

        /* 4. 读缓冲区 buf_t3tot2 */
        {
            integer buf_val = 0;
            READ_BUFFER(
                ps2_buf_t3tot2_id,
                BUF_TIMEOUT,
                (MESSAGE_ADDR_TYPE)&buf_val,
                &len,
                &ret);
            /* buf 可能为空，不强制检查 */
            (void)ret;
            val += buf_val;
        }

        /* 5. 写黑板 bb_t2t3（传递数据给 task22） */
        DISPLAY_BLACKBOARD(
            ps2_bb_t2t3_id,
            (MESSAGE_ADDR_TYPE)&val,
            sizeof(integer),
            &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD bb_t2t3", ret);

        /* 6. 写缓冲区 buf_t2tot3 */
        WRITE_BUFFER(
            ps2_buf_t2tot3_id,
            (MESSAGE_ADDR_TYPE)&val,
            sizeof(integer),
            BUF_TIMEOUT,
            &ret);
        CHECK_CODE("WRITE_BUFFER buf_t2tot3", ret);

        /* 7. 周期等待 */
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task21", ret);
    }

    return NULL;
}

/*===========================================================================
 * task22_job
 * 周期：50ms，优先级：3
 * 通信行为：
 *   读  - bb_t2t3，buf_t2tot3
 *   写  - pr2queueingout（队列端口），bb_t3t2，buf_t3tot2
 *   子程序：commandboard_printinfos_spg
 *===========================================================================*/
void *task22_job(void *arg)
{
    RETURN_CODE_TYPE  ret;
    integer           val;
    MESSAGE_SIZE_TYPE len;

    (void)arg;

    while (1) {
        /* 1. 读黑板 bb_t2t3（接收 task21 写入的数据） */
        val = 0;
        READ_BLACKBOARD(
            ps2_bb_t2t3_id,
            BB_TIMEOUT,
            (MESSAGE_ADDR_TYPE)&val,
            &len,
            &ret);
        CHECK_CODE("READ_BLACKBOARD bb_t2t3", ret);

        /* 2. 读缓冲区 buf_t2tot3 */
        {
            integer buf_val = 0;
            READ_BUFFER(
                ps2_buf_t2tot3_id,
                BUF_TIMEOUT,
                (MESSAGE_ADDR_TYPE)&buf_val,
                &len,
                &ret);
            (void)ret;
            val += buf_val;
        }

        /* 3. 调用子程序打印信息 */
        commandboard_printinfos_spg(&val);

        /* 4. 写队列端口 pr2queueingout（向外部发送） */
        SEND_QUEUING_MESSAGE(
            ps2_pr2queueingout_id,
            (MESSAGE_ADDR_TYPE)&val,
            sizeof(integer),
            QUEUING_TIMEOUT,
            &ret);
        CHECK_CODE("SEND_QUEUING_MESSAGE pr2queueingout", ret);

        /* 5. 写黑板 bb_t3t2（传递数据给 task21） */
        DISPLAY_BLACKBOARD(
            ps2_bb_t3t2_id,
            (MESSAGE_ADDR_TYPE)&val,
            sizeof(integer),
            &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD bb_t3t2", ret);

        /* 6. 写缓冲区 buf_t3tot2 */
        WRITE_BUFFER(
            ps2_buf_t3tot2_id,
            (MESSAGE_ADDR_TYPE)&val,
            sizeof(integer),
            BUF_TIMEOUT,
            &ret);
        CHECK_CODE("WRITE_BUFFER buf_t3tot2", ret);

        /* 7. 周期等待 */
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task22", ret);
    }

    return NULL;
}

/*===========================================================================
 * task23_job
 * 周期：100ms，优先级：4
 * 通信行为：
 *   读  - bb_acc3，buf_order
 *   写  - bb_acc3，buf_order
 *===========================================================================*/
void *task23_job(void *arg)
{
    RETURN_CODE_TYPE  ret;
    integer           val;
    MESSAGE_SIZE_TYPE len;

    (void)arg;

    while (1) {
        /* 1. 读黑板 bb_acc3 */
        val = 0;
        READ_BLACKBOARD(
            ps2_bb_acc3_id,
            BB_TIMEOUT,
            (MESSAGE_ADDR_TYPE)&val,
            &len,
            &ret);
        CHECK_CODE("READ_BLACKBOARD bb_acc3", ret);

        /* 2. 读缓冲区 buf_order */
        {
            integer buf_val = 0;
            READ_BUFFER(
                ps2_buf_order_id,
                BUF_TIMEOUT,
                (MESSAGE_ADDR_TYPE)&buf_val,
                &len,
                &ret);
            (void)ret;
            val += buf_val;
        }

        /* 3. 本地处理（累加器逻辑示意） */
        val += 1;

        /* 4. 写黑板 bb_acc3（更新累加器状态） */
        DISPLAY_BLACKBOARD(
            ps2_bb_acc3_id,
            (MESSAGE_ADDR_TYPE)&val,
            sizeof(integer),
            &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD bb_acc3", ret);

        /* 5. 写缓冲区 buf_order */
        WRITE_BUFFER(
            ps2_buf_order_id,
            (MESSAGE_ADDR_TYPE)&val,
            sizeof(integer),
            BUF_TIMEOUT,
            &ret);
        CHECK_CODE("WRITE_BUFFER buf_order", ret);

        /* 6. 周期等待 */
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT task23", ret);
    }

    return NULL;
}