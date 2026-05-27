#include <stdio.h>
#include <string.h>
#include <os/pos/apex/apexLib.h>

#include "activity.h"
#include "globals.h"
#include "gtypes.h"

/* ---------------------------------------------------------------
 * 外部引用：队列端口 ID，在 main.c 中声明并初始化
 * ------------------------------------------------------------- */
extern QUEUING_PORT_ID_TYPE order_in_id;
extern QUEUING_PORT_ID_TYPE queueingout_id;

/* ===============================================================
 * task51_job
 *   周期：50 ms（50,000,000 ns），优先级：1
 *   职责：从队列端口 order_in 接收消息并处理
 * ============================================================= */
void *task51_job(void *arg)
{
    RETURN_CODE_TYPE      ret;
    MESSAGE_SIZE_TYPE     received_len;
    MESSAGE_ADDR_TYPE     msg_buf[MAX_MESSAGE_SIZE];

    while (1)
    {
        /* 从 order_in 队列端口接收消息（阻塞超时 = 0，即不阻塞）*/
        RECEIVE_QUEUING_MESSAGE(order_in_id,
                                0,              /* TIME_OUT: 0 = 非阻塞 */
                                msg_buf,
                                &received_len,
                                &ret);
        if (ret == NO_ERROR)
        {
            printf("[PS5][task51] Received message, len=%d\n",
                   (int)received_len);
            /* TODO: 在此处添加消息处理逻辑 */
        }
        else if (ret != NOT_AVAILABLE)
        {
            /* NOT_AVAILABLE 表示队列为空，属于正常情况，其余视为错误 */
            CHECK_CODE("RECEIVE_QUEUING_MESSAGE(order_in)", ret);
        }

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task51)", ret);
    }

    return NULL;
}

/* ===============================================================
 * task52_job
 *   周期：50 ms（50,000,000 ns），优先级：2
 *   职责：向队列端口 queueingout 发送消息
 * ============================================================= */
void *task52_job(void *arg)
{
    RETURN_CODE_TYPE  ret;
    /* 示例消息内容，实际内容由业务逻辑填充 */
    const char        payload[] = "PS5_TASK52_DATA";
    MESSAGE_SIZE_TYPE payload_len = (MESSAGE_SIZE_TYPE)sizeof(payload);

    while (1)
    {
        /* TODO: 在此处填充真实的发送数据 */

        SEND_QUEUING_MESSAGE(queueingout_id,
                             (MESSAGE_ADDR_TYPE)payload,
                             payload_len,
                             0,     /* TIME_OUT: 0 = 非阻塞 */
                             &ret);
        CHECK_CODE("SEND_QUEUING_MESSAGE(queueingout)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task52)", ret);
    }

    return NULL;
}

/* ===============================================================
 * task53_job
 *   周期：200 ms（200,000,000 ns），优先级：3
 *   职责：周期性后台处理任务（业务逻辑待实现）
 * ============================================================= */
void *task53_job(void *arg)
{
    RETURN_CODE_TYPE ret;

    while (1)
    {
        /* TODO: 在此处实现 task53 的业务逻辑 */
        printf("[PS5][task53] periodic execution\n");

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task53)", ret);
    }

    return NULL;
}
