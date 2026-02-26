#include <os/pos/apex/apexLib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

/*****************************************************/
/*  Partition: ps2 (P2)  Module: M1 (module1)       */
/*  AADL model: DIMA_partitions::P2.impl            */
/*  Processor: arinckernel.module1  Part: part2      */
/*  DAL: LEVEL_A                                     */
/*****************************************************/

PROCESS_ID_TYPE arinc_threads[ACoreOS653_CONFIG_NB_THREADS];

/* ---- Inter-partition ports ---- */
/* Sampling DESTINATION port: receives data from ps1 via msg1_snd channel */
SAMPLING_PORT_ID_TYPE ps2_pr2samplingin_id;
/* Queuing SOURCE port: sends data to ps1 via msg2_snd channel */
QUEUING_PORT_ID_TYPE  ps2_pr2queueingout_id;

/* ---- Intra-partition blackboards ---- */
/* bb_acc3: task21 (DISPLAY) -> task22 (READ)  -- accelerometer channel 3 */
BLACKBOARD_ID_TYPE ps2_bb_acc3_id;
/* bb_t2t3: task22 (DISPLAY) -> task23 (READ) */
BLACKBOARD_ID_TYPE ps2_bb_t2t3_id;
/* bb_t3t2: task23 (DISPLAY) -> task22 (READ) */
BLACKBOARD_ID_TYPE ps2_bb_t3t2_id;

/* ---- Intra-partition buffers ---- */
/* buf_order: task22 (WRITE) -> task21 (READ) -- command/order feedback */
BUFFER_ID_TYPE ps2_buf_order_id;
/* buf_t2tot3: task22 (WRITE) -> task23 (READ) */
BUFFER_ID_TYPE ps2_buf_t2tot3_id;
/* buf_t3tot2: task23 (WRITE) -> task22 (READ) */
BUFFER_ID_TYPE ps2_buf_t3tot2_id;

void appMain(void)
{
    PROCESS_ATTRIBUTE_TYPE tattr;
    RETURN_CODE_TYPE ret = NO_ERROR;

    /* ------------------------------------------------------------------ */
    /*  1. Create inter-partition SAMPLING port                            */
    /*     AADL: pr2samplingin: in data port Base_Types::Integer           */
    /*           ARINC653::Sampling_Refresh_Period => 100ns                */
    /* ------------------------------------------------------------------ */
    CREATE_SAMPLING_PORT(
        "pr2samplingin",
        sizeof(integer),
        DESTINATION,           /* PORT_DIRECTION */
        100ll,                 /* REFRESH_PERIOD: 100 ns */
        &(ps2_pr2samplingin_id),
        &(ret));
    CHECK_CODE("CREATE_SAMPLING_PORT(pr2samplingin)", ret);

    /* ------------------------------------------------------------------ */
    /*  2. Create intra-partition BLACKBOARD: bb_acc3                      */
    /*     Connects: task21.task1_acc3_out -> task22.task2_acc3_in         */
    /* ------------------------------------------------------------------ */
    CREATE_BLACKBOARD(
        "bb_acc3",
        sizeof(integer),
        &(ps2_bb_acc3_id),
        &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_acc3)", ret);

    /* ------------------------------------------------------------------ */
    /*  3. Create intra-partition BUFFER: buf_order                        */
    /*     Connects: task22.task2_order_out -> task21.task1_order_in       */
    /* ------------------------------------------------------------------ */
    CREATE_BUFFER(
        "buf_order",
        sizeof(integer),
        10,                    /* MAX_NB_MESSAGE */
        FIFO,
        &(ps2_buf_order_id),
        &(ret));
    CHECK_CODE("CREATE_BUFFER(buf_order)", ret);

    /* ------------------------------------------------------------------ */
    /*  4. Create task21 (AADL: Tsk21)                                     */
    /*     Period=50ms  Priority=2  Stack=8KB  Deadline=SOFT               */
    /* ------------------------------------------------------------------ */
    strcpy(tattr.NAME, "task21");
    tattr.ENTRY_POINT    = task21_job;
    tattr.BASE_PRIORITY  = 2;
    tattr.PERIOD         = 50000000ll;   /* 50 ms */
    tattr.STACK_SIZE     = 8192;
    tattr.TIME_CAPACITY  = 50000000ll;
    tattr.DEADLINE       = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[0]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task21)", ret);
    START(arinc_threads[0], &(ret));
    CHECK_CODE("START(task21)", ret);

    /* ------------------------------------------------------------------ */
    /*  5. Create intra-partition BUFFER: buf_t2tot3                       */
    /*     Connects: task22.task2_buffer_send -> task23.task3_buffer_recv  */
    /* ------------------------------------------------------------------ */
    CREATE_BUFFER(
        "buf_t2tot3",
        sizeof(integer),
        10,
        FIFO,
        &(ps2_buf_t2tot3_id),
        &(ret));
    CHECK_CODE("CREATE_BUFFER(buf_t2tot3)", ret);

    /* ------------------------------------------------------------------ */
    /*  6. Create intra-partition BUFFER: buf_t3tot2                       */
    /*     Connects: task23.task3_buffer_send -> task22.task2_buffer_recv  */
    /* ------------------------------------------------------------------ */
    CREATE_BUFFER(
        "buf_t3tot2",
        sizeof(integer),
        10,
        FIFO,
        &(ps2_buf_t3tot2_id),
        &(ret));
    CHECK_CODE("CREATE_BUFFER(buf_t3tot2)", ret);

    /* ------------------------------------------------------------------ */
    /*  7. Create intra-partition BLACKBOARD: bb_t2t3                      */
    /*     Connects: task22.task2_blackboard_write -> task23.task3_bb_read */
    /* ------------------------------------------------------------------ */
    CREATE_BLACKBOARD(
        "bb_t2t3",
        sizeof(integer),
        &(ps2_bb_t2t3_id),
        &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_t2t3)", ret);

    /* ------------------------------------------------------------------ */
    /*  8. Create intra-partition BLACKBOARD: bb_t3t2                      */
    /*     Connects: task23.task3_blackboard_write -> task22.task2_bb_read */
    /* ------------------------------------------------------------------ */
    CREATE_BLACKBOARD(
        "bb_t3t2",
        sizeof(integer),
        &(ps2_bb_t3t2_id),
        &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_t3t2)", ret);

    /* ------------------------------------------------------------------ */
    /*  9. Create task22 (AADL: Tsk22)                                     */
    /*     Period=50ms  Priority=3  Stack=8KB  Deadline=SOFT               */
    /* ------------------------------------------------------------------ */
    strcpy(tattr.NAME, "task22");
    tattr.ENTRY_POINT    = task22_job;
    tattr.BASE_PRIORITY  = 3;
    tattr.PERIOD         = 50000000ll;
    tattr.STACK_SIZE     = 8192;
    tattr.TIME_CAPACITY  = 50000000ll;
    tattr.DEADLINE       = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[1]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task22)", ret);
    START(arinc_threads[1], &(ret));
    CHECK_CODE("START(task22)", ret);

    /* ------------------------------------------------------------------ */
    /* 10. Create inter-partition QUEUING port                             */
    /*     AADL: pr2queueingout: out event data port Base_Types::Integer   */
    /*           Queue_Size => 30  Timeout => 5ns  Queueing => FIFO        */
    /* ------------------------------------------------------------------ */
    CREATE_QUEUING_PORT(
        "pr2queueingout",
        sizeof(integer),
        30,                    /* MAX_NB_MESSAGE */
        SOURCE,                /* PORT_DIRECTION */
        FIFO,
        &(ps2_pr2queueingout_id),
        &(ret));
    CHECK_CODE("CREATE_QUEUING_PORT(pr2queueingout)", ret);

    /* ------------------------------------------------------------------ */
    /* 11. Create task23 (AADL: Tsk23)                                     */
    /*     Period=100ms  Priority=4  Stack=8KB  Deadline=SOFT              */
    /* ------------------------------------------------------------------ */
    strcpy(tattr.NAME, "task23");
    tattr.ENTRY_POINT    = task23_job;
    tattr.BASE_PRIORITY  = 4;
    tattr.PERIOD         = 100000000ll;  /* 100 ms */
    tattr.STACK_SIZE     = 8192;
    tattr.TIME_CAPACITY  = 50000000ll;
    tattr.DEADLINE       = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[2]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task23)", ret);
    START(arinc_threads[2], &(ret));
    CHECK_CODE("START(task23)", ret);

    /* ------------------------------------------------------------------ */
    /* 12. Switch partition to NORMAL operating mode                        */
    /* ------------------------------------------------------------------ */
    SET_PARTITION_MODE(NORMAL, &(ret));
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);
    return;
}

/* ==============================================================
 *  健康监控回调函数（Health Monitoring Callbacks）
 *
 *  ARINC 653 HM 机制：当操作系统检测到错误（内存越界、时间超限等），
 *  会调用对应的回调函数，由应用决定如何处置（记录日志、重启分区等）。
 *
 *  module.xml 中的绑定：
 *    Module_HM_Table    ModuleCallback="module_HM_callback"
 *    Partition_HM_Table PartitionCallback="partition_HM_callback"
 * ==============================================================*/

/* 模块级 HM 回调：模块初始化阶段或系统功能执行阶段发生错误时调用 */
void module_HM_callback(ERROR_STATUS_TYPE *error_status)
{
    printf("[HM MODULE ps2 (P2)] ErrorID=%d  FailedProcess=%d  State=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID,
           (int)error_status->SYSTEM_STATE);
    /* TODO: 根据 ErrorID 决定具体处置策略（记录日志、触发 SHUTDOWN 等）*/
}

/* 分区级 HM 回调：分区初始化或运行阶段发生错误时调用 */
void partition_HM_callback(ERROR_STATUS_TYPE *error_status)
{
    printf("[HM PARTITION ps2 (P2)] ErrorID=%d  FailedProcess=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID);
    /* TODO: 根据 ErrorID 决定处置策略（IDLE / COLD_START / WARM_START 等）*/
}
