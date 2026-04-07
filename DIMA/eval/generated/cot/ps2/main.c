#include <os/pos/apex/apexLib.h>
#include <string.h>
#include <stdio.h>
#include "globals.h"
#include "deployment.h"
#include "activity.h"
#include "gtypes.h"

/*===========================================================================
 * appMain
 * 分区：ps2，模块：M1，AADL进程：DIMA_partitions::P2.impl
 * 创建顺序：端口 → 黑板 → 缓冲区 → 任务 → SET_PARTITION_MODE
 *===========================================================================*/
void appMain(void)
{
    RETURN_CODE_TYPE       ret;
    PROCESS_ATTRIBUTE_TYPE tattr;

    /*-----------------------------------------------------------------------
     * 1. 创建采样端口
     *    pr2samplingin：DESTINATION，刷新周期 50ms
     *-----------------------------------------------------------------------*/
    CREATE_SAMPLING_PORT(
        "pr2samplingin",
        (MESSAGE_SIZE_TYPE)sizeof(integer),
        DESTINATION,
        PR2SAMPLINGIN_REFRESH_PERIOD,
        &ps2_pr2samplingin_id,
        &ret);
    CHECK_CODE("CREATE_SAMPLING_PORT pr2samplingin", ret);

    /*-----------------------------------------------------------------------
     * 2. 创建队列端口
     *    pr2queueingout：SOURCE，最大30条消息，FIFO
     *-----------------------------------------------------------------------*/
    CREATE_QUEUING_PORT(
        "pr2queueingout",
        (MESSAGE_SIZE_TYPE)sizeof(integer),
        30,
        SOURCE,
        FIFO,
        &ps2_pr2queueingout_id,
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT pr2queueingout", ret);

    /*-----------------------------------------------------------------------
     * 3. 创建黑板
     *-----------------------------------------------------------------------*/
    CREATE_BLACKBOARD(
        "bb_acc3",
        (MESSAGE_SIZE_TYPE)sizeof(integer),
        &ps2_bb_acc3_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD bb_acc3", ret);

    CREATE_BLACKBOARD(
        "bb_t2t3",
        (MESSAGE_SIZE_TYPE)sizeof(integer),
        &ps2_bb_t2t3_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD bb_t2t3", ret);

    CREATE_BLACKBOARD(
        "bb_t3t2",
        (MESSAGE_SIZE_TYPE)sizeof(integer),
        &ps2_bb_t3t2_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD bb_t3t2", ret);

    /*-----------------------------------------------------------------------
     * 4. 创建缓冲区（FIFO，最大消息数与队列端口保持一致取10）
     *-----------------------------------------------------------------------*/
    CREATE_BUFFER(
        "buf_order",
        (MESSAGE_SIZE_TYPE)sizeof(integer),
        10,
        FIFO,
        &ps2_buf_order_id,
        &ret);
    CHECK_CODE("CREATE_BUFFER buf_order", ret);

    CREATE_BUFFER(
        "buf_t2tot3",
        (MESSAGE_SIZE_TYPE)sizeof(integer),
        10,
        FIFO,
        &ps2_buf_t2tot3_id,
        &ret);
    CHECK_CODE("CREATE_BUFFER buf_t2tot3", ret);

    CREATE_BUFFER(
        "buf_t3tot2",
        (MESSAGE_SIZE_TYPE)sizeof(integer),
        10,
        FIFO,
        &ps2_buf_t3tot2_id,
        &ret);
    CHECK_CODE("CREATE_BUFFER buf_t3tot2", ret);

    /*-----------------------------------------------------------------------
     * 5a. 创建并启动 task21（50ms，优先级 2）
     *-----------------------------------------------------------------------*/
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task21");
    tattr.ENTRY_POINT   = task21_job;
    tattr.BASE_PRIORITY = 2;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = TASK_STACK_SIZE;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[0], &ret);
    CHECK_CODE("CREATE_PROCESS task21", ret);
    START(arinc_threads[0], &ret);
    CHECK_CODE("START task21", ret);

    /*-----------------------------------------------------------------------
     * 5b. 创建并启动 task22（50ms，优先级 3）
     *-----------------------------------------------------------------------*/
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task22");
    tattr.ENTRY_POINT   = task22_job;
    tattr.BASE_PRIORITY = 3;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = TASK_STACK_SIZE;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[1], &ret);
    CHECK_CODE("CREATE_PROCESS task22", ret);
    START(arinc_threads[1], &ret);
    CHECK_CODE("START task22", ret);

    /*-----------------------------------------------------------------------
     * 5c. 创建并启动 task23（100ms，优先级 4）
     *-----------------------------------------------------------------------*/
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task23");
    tattr.ENTRY_POINT   = task23_job;
    tattr.BASE_PRIORITY = 4;
    tattr.PERIOD        = 100000000ll;
    tattr.STACK_SIZE    = TASK_STACK_SIZE;
    tattr.TIME_CAPACITY = 100000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[2], &ret);
    CHECK_CODE("CREATE_PROCESS task23", ret);
    START(arinc_threads[2], &ret);
    CHECK_CODE("START task23", ret);

    /*-----------------------------------------------------------------------
     * 6. 切换分区模式为 NORMAL
     *-----------------------------------------------------------------------*/
    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE NORMAL", ret);
}

/*===========================================================================
 * module_HM_callback
 * 模块级健康监控回调
 *===========================================================================*/
void module_HM_callback(
    ERROR_STATUS_TYPE *error_status)
{
    (void)error_status;
    /* 模块级错误处理：记录并停止 */
    STOP_SELF();
}

/*===========================================================================
 * partition_HM_callback
 * 分区级健康监控回调
 *===========================================================================*/
void partition_HM_callback(
    ERROR_STATUS_TYPE *error_status)
{
    (void)error_status;
    /* 分区级错误处理：记录并停止当前进程 */
    STOP_SELF();
}