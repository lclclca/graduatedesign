#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "deployment.h"
#include "activity.h"
#include "gtypes.h"

/* ══════════════════════════════════════════════════════════════
 * appMain
 * 分区初始化入口：创建所有通信资源和任务，最后切换为 NORMAL 模式。
 * ══════════════════════════════════════════════════════════════ */
void appMain(void)
{
    RETURN_CODE_TYPE       ret;
    PROCESS_ATTRIBUTE_TYPE tattr;

    /* ── 1. 创建采样端口 ───────────────────────────────────────
     * pr1samplingout：SOURCE，刷新周期 50 ms = 50000000 ns
     * ────────────────────────────────────────────────────────── */
    CREATE_SAMPLING_PORT(
        "pr1samplingout",
        sizeof(integer),
        SOURCE,
        50000000ll,
        &ps1_pr1samplingout_id,
        &ret);
    CHECK_CODE("CREATE_SAMPLING_PORT pr1samplingout", ret);

    /* ── 2. 创建队列端口 ────────────────────────────────────────
     * pr1queueingin：DESTINATION，max_nb=30，FIFO
     * ────────────────────────────────────────────────────────── */
    CREATE_QUEUING_PORT(
        "pr1queueingin",
        sizeof(integer),
        30,
        DESTINATION,
        FIFO,
        &ps1_pr1queueingin_id,
        &ret);
    CHECK_CODE("CREATE_QUEUING_PORT pr1queueingin", ret);

    /* ── 3. 创建黑板 ────────────────────────────────────────────
     * 顺序：bb_acc1, bb_acc2, bb_sem, bb_t3t4, bb_t4t3
     * ────────────────────────────────────────────────────────── */
    CREATE_BLACKBOARD(
        "bb_acc1",
        sizeof(integer),
        &ps1_bb_acc1_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD bb_acc1", ret);

    CREATE_BLACKBOARD(
        "bb_acc2",
        sizeof(integer),
        &ps1_bb_acc2_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD bb_acc2", ret);

    CREATE_BLACKBOARD(
        "bb_sem",
        sizeof(integer),
        &ps1_bb_sem_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD bb_sem", ret);

    CREATE_BLACKBOARD(
        "bb_t3t4",
        sizeof(integer),
        &ps1_bb_t3t4_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD bb_t3t4", ret);

    CREATE_BLACKBOARD(
        "bb_t4t3",
        sizeof(integer),
        &ps1_bb_t4t3_id,
        &ret);
    CHECK_CODE("CREATE_BLACKBOARD bb_t4t3", ret);

    /* ── 4. 创建缓冲区 ──────────────────────────────────────────
     * 顺序：buf_sensor, buf_t3tot4, buf_t4tot3，每个容量 30 条消息
     * ────────────────────────────────────────────────────────── */
    CREATE_BUFFER(
        "buf_sensor",
        sizeof(integer),
        30,
        FIFO,
        &ps1_buf_sensor_id,
        &ret);
    CHECK_CODE("CREATE_BUFFER buf_sensor", ret);

    CREATE_BUFFER(
        "buf_t3tot4",
        sizeof(integer),
        30,
        FIFO,
        &ps1_buf_t3tot4_id,
        &ret);
    CHECK_CODE("CREATE_BUFFER buf_t3tot4", ret);

    CREATE_BUFFER(
        "buf_t4tot3",
        sizeof(integer),
        30,
        FIFO,
        &ps1_buf_t4tot3_id,
        &ret);
    CHECK_CODE("CREATE_BUFFER buf_t4tot3", ret);

    /* ── 5. 创建并启动任务 ──────────────────────────────────────
     * task11：周期 25 ms，优先级 2
     * ────────────────────────────────────────────────────────── */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME,     "task11");
    tattr.ENTRY_POINT   = task11_job;
    tattr.BASE_PRIORITY = 2;
    tattr.PERIOD        = 25000000ll;
    tattr.TIME_CAPACITY = 25000000ll;
    tattr.STACK_SIZE    = TASK_STACK_SIZE;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[0], &ret);
    CHECK_CODE("CREATE_PROCESS task11", ret);
    START(arinc_threads[0], &ret);
    CHECK_CODE("START task11", ret);

    /* task12：周期 50 ms，优先级 3 */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME,     "task12");
    tattr.ENTRY_POINT   = task12_job;
    tattr.BASE_PRIORITY = 3;
    tattr.PERIOD        = 50000000ll;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.STACK_SIZE    = TASK_STACK_SIZE;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[1], &ret);
    CHECK_CODE("CREATE_PROCESS task12", ret);
    START(arinc_threads[1], &ret);
    CHECK_CODE("START task12", ret);

    /* task13：周期 50 ms，优先级 4 */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME,     "task13");
    tattr.ENTRY_POINT   = task13_job;
    tattr.BASE_PRIORITY = 4;
    tattr.PERIOD        = 50000000ll;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.STACK_SIZE    = TASK_STACK_SIZE;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[2], &ret);
    CHECK_CODE("CREATE_PROCESS task13", ret);
    START(arinc_threads[2], &ret);
    CHECK_CODE("START task13", ret);

    /* task14：周期 50 ms，优先级 5 */
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME,     "task14");
    tattr.ENTRY_POINT   = task14_job;
    tattr.BASE_PRIORITY = 5;
    tattr.PERIOD        = 50000000ll;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.STACK_SIZE    = TASK_STACK_SIZE;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&tattr, &arinc_threads[3], &ret);
    CHECK_CODE("CREATE_PROCESS task14", ret);
    START(arinc_threads[3], &ret);
    CHECK_CODE("START task14", ret);

    /* ── 6. 切换分区为 NORMAL 模式 ─────────────────────────────
     * 此调用之后 appMain 不再执行，调度器接管
     * ────────────────────────────────────────────────────────── */
    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE NORMAL", ret);
}

/* ══════════════════════════════════════════════════════════════
 * module_HM_callback
 * 模块级健康监控回调：处理模块范围的故障。
 * 实际项目中应根据 HM 表配置执行相应恢复动作。
 * ══════════════════════════════════════════════════════════════ */
void module_HM_callback(
    ERROR_STATUS_TYPE *error_status)
{
    printf("[ps1][MODULE_HM] error_code=%d, failed_process=%lu\n",
           (int)error_status->ERROR_CODE,
           (unsigned long)error_status->FAILED_PROCESS_ID);

    /* 默认策略：停止模块 */
    STOP_SELF();
}

/* ══════════════════════════════════════════════════════════════
 * partition_HM_callback
 * 分区级健康监控回调：处理分区内部故障。
 * ══════════════════════════════════════════════════════════════ */
void partition_HM_callback(
    ERROR_STATUS_TYPE *error_status)
{
    printf("[ps1][PARTITION_HM] error_code=%d, failed_process=%lu\n",
           (int)error_status->ERROR_CODE,
           (unsigned long)error_status->FAILED_PROCESS_ID);

    /* 默认策略：重启分区 */
    RETURN_CODE_TYPE ret;
    SET_PARTITION_MODE(WARM_START, &ret);
}