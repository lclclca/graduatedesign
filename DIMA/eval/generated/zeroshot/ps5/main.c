#include <stdio.h>
#include <string.h>
#include <os/pos/apex/apexLib.h>

#include "deployment.h"
#include "globals.h"
#include "gtypes.h"
#include "activity.h"

/* ===============================================================
 * 全局：队列端口 ID
 *   在 activity.c 中以 extern 引用
 * ============================================================= */
QUEUING_PORT_ID_TYPE order_in_id;
QUEUING_PORT_ID_TYPE queueingout_id;

/* ===============================================================
 * 健康监控回调
 * ============================================================= */

/* 模块级 HM 回调 */
void module_HM_callback(HM_ERROR_STATUS_TYPE *error_status)
{
    printf("[PS5][module_HM_callback] error_code=%d, failed_process=%d\n",
           (int)error_status->ERROR_CODE,
           (int)error_status->FAILED_PROCESS_ID);
    /* TODO: 根据项目安全策略实现模块级健康监控处理 */
}

/* 分区级 HM 回调 */
void partition_HM_callback(HM_ERROR_STATUS_TYPE *error_status)
{
    printf("[PS5][partition_HM_callback] error_code=%d, failed_process=%d\n",
           (int)error_status->ERROR_CODE,
           (int)error_status->FAILED_PROCESS_ID);
    /* TODO: 根据项目安全策略实现分区级健康监控处理 */
}

/* ===============================================================
 * appMain — 分区初始化入口
 *   1. 创建队列端口
 *   2. 创建所有任务进程
 *   3. 切换分区模式为 NORMAL
 * ============================================================= */
void appMain(void)
{
    RETURN_CODE_TYPE   ret;
    PROCESS_ID_TYPE    proc_id;
    PROCESS_ATTRIBUTE_TYPE tattr;

    printf("[PS5] appMain: initializing partition ps5\n");

    /* -----------------------------------------------------------
     * 1. 创建队列端口
     * --------------------------------------------------------- */

    /* order_in — DESTINATION，最大消息数 4 */
    CREATE_QUEUING_PORT(
        "order_in",             /* 端口名称（须与系统配置一致）*/
        MAX_MESSAGE_SIZE,       /* MAX_MESSAGE_SIZE 由 APEX 库定义 */
        4,                      /* MAX_NB_MESSAGE */
        DESTINATION,            /* PORT_DIRECTION */
        FIFO,                   /* QUEUING_DISCIPLINE */
        &order_in_id,
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(order_in)", ret);

    /* queueingout — SOURCE，最大消息数 4 */
    CREATE_QUEUING_PORT(
        "queueingout",
        MAX_MESSAGE_SIZE,
        4,
        SOURCE,
        FIFO,
        &queueingout_id,
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(queueingout)", ret);

    /* -----------------------------------------------------------
     * 2. 创建任务进程
     *    周期单位：纳秒（ms × 1,000,000）
     *    优先级：规格中定义的值（数值越小优先级越高，视 APEX 实现而定）
     * --------------------------------------------------------- */

    /* ---- task51：周期 50 ms，优先级 1 ---- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task51");
    tattr.ENTRY_POINT       = (SYSTEM_ADDRESS_TYPE)task51_job;
    tattr.STACK_SIZE        = ACoreOS653_NEEDS_STACKS_SIZE /
                              ACoreOS653_CONFIG_NB_THREADS;  /* 8192 bytes */
    tattr.BASE_PRIORITY     = 1;
    tattr.PERIOD            = 50 * 1000000LL;    /* 50 ms → ns */
    tattr.TIME_CAPACITY     = 50 * 1000000LL;    /* WCET ≤ 周期 */
    tattr.DEADLINE          = SOFT;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task51)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task51)", ret);

    /* ---- task52：周期 50 ms，优先级 2 ---- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task52");
    tattr.ENTRY_POINT       = (SYSTEM_ADDRESS_TYPE)task52_job;
    tattr.STACK_SIZE        = ACoreOS653_NEEDS_STACKS_SIZE /
                              ACoreOS653_CONFIG_NB_THREADS;
    tattr.BASE_PRIORITY     = 2;
    tattr.PERIOD            = 50 * 1000000LL;
    tattr.TIME_CAPACITY     = 50 * 1000000LL;
    tattr.DEADLINE          = SOFT;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task52)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task52)", ret);

    /* ---- task53：周期 200 ms，优先级 3 ---- */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "task53");
    tattr.ENTRY_POINT       = (SYSTEM_ADDRESS_TYPE)task53_job;
    tattr.STACK_SIZE        = ACoreOS653_NEEDS_STACKS_SIZE /
                              ACoreOS653_CONFIG_NB_THREADS;
    tattr.BASE_PRIORITY     = 3;
    tattr.PERIOD            = 200 * 1000000LL;   /* 200 ms → ns */
    tattr.TIME_CAPACITY     = 200 * 1000000LL;
    tattr.DEADLINE          = SOFT;

    CREATE_PROCESS(&tattr, &proc_id, &ret);
    CHECK_CODE("CREATE_PROCESS(task53)", ret);

    START(proc_id, &ret);
    CHECK_CODE("START(task53)", ret);

    /* -----------------------------------------------------------
     * 3. 切换分区模式为 NORMAL，触发调度器开始运行所有周期进程
     * --------------------------------------------------------- */
    printf("[PS5] appMain: setting partition mode to NORMAL\n");
    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);

    /* SET_PARTITION_MODE(NORMAL) 调用成功后不会返回（控制权交给调度器） */
}