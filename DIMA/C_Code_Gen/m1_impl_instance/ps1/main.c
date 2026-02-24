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
/*  Partition: ps1 (P1)  Module: M1 (module1)       */
/*  AADL model: DIMA_partitions::P1.impl            */
/*  Processor: arinckernel.module1  Part: part1      */
/*  DAL: LEVEL_A                                     */
/*****************************************************/

/* Array of ARINC 653 process (task) handles */
PROCESS_ID_TYPE arinc_threads[ACoreOS653_CONFIG_NB_THREADS];

/* ---- Inter-partition ports ---- */
/* Sampling SOURCE port: sends data to ps2 via msg1_snd channel */
SAMPLING_PORT_ID_TYPE ps1_pr1samplingout_id;
/* Queuing DESTINATION port: receives data from ps2 via msg2_snd channel */
QUEUING_PORT_ID_TYPE  ps1_pr1queueingin_id;

/* ---- Intra-partition blackboards ---- */
/* bb_acc1: task11 (DISPLAY) -> task12 (READ)  -- accelerometer channel 1 */
BLACKBOARD_ID_TYPE ps1_bb_acc1_id;
/* bb_acc2: task11 (DISPLAY) -> task12 (READ)  -- accelerometer channel 2 */
BLACKBOARD_ID_TYPE ps1_bb_acc2_id;
/* bb_sem: task12 (DISPLAY/write_only) -> task13 (READ/read_only)
 *   Maps to AADL: data access sem <-> task12.task2_printer
 *               + data access sem <-> task13.task3_receiver
 *   sem is protected_data (Concurrency_Control_Protocol => Protected_Access)
 *   Implemented as ARINC 653 BLACKBOARD to ensure read-after-write consistency */
BLACKBOARD_ID_TYPE ps1_bb_sem_id;
/* bb_t3t4: task13 (DISPLAY) -> task14 (READ) */
BLACKBOARD_ID_TYPE ps1_bb_t3t4_id;
/* bb_t4t3: task14 (DISPLAY) -> task13 (READ) */
BLACKBOARD_ID_TYPE ps1_bb_t4t3_id;

/* ---- Intra-partition buffers ---- */
/* buf_sensor: task12 (WRITE) -> task11 (READ) -- sensor feedback */
BUFFER_ID_TYPE ps1_buf_sensor_id;
/* buf_t3tot4: task13 (WRITE) -> task14 (READ) */
BUFFER_ID_TYPE ps1_buf_t3tot4_id;
/* buf_t4tot3: task14 (WRITE) -> task13 (READ) */
BUFFER_ID_TYPE ps1_buf_t4tot3_id;

void appMain(void)
{
    PROCESS_ATTRIBUTE_TYPE tattr;
    RETURN_CODE_TYPE ret = NO_ERROR;

    /* ------------------------------------------------------------------ */
    /*  1. Create inter-partition SAMPLING port                            */
    /*     AADL: pr1samplingout: out data port Base_Types::Integer         */
    /*           ARINC653::Sampling_Refresh_Period => 100ns                */
    /* ------------------------------------------------------------------ */
    CREATE_SAMPLING_PORT(
        "pr1samplingout",
        sizeof(integer),       /* MAX_MESSAGE_SIZE */
        SOURCE,                /* PORT_DIRECTION   */
        100ll,                 /* REFRESH_PERIOD   : 100 ns */
        &(ps1_pr1samplingout_id),
        &(ret));
    CHECK_CODE("CREATE_SAMPLING_PORT(pr1samplingout)", ret);

    /* ------------------------------------------------------------------ */
    /*  2. Create intra-partition BLACKBOARD: bb_acc1                      */
    /*     Connects: task11.task1_acc1_out -> task12.task2_acc1_in         */
    /* ------------------------------------------------------------------ */
    CREATE_BLACKBOARD(
        "bb_acc1",
        sizeof(integer),
        &(ps1_bb_acc1_id),
        &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_acc1)", ret);

    /* ------------------------------------------------------------------ */
    /*  3. Create intra-partition BLACKBOARD: bb_acc2                      */
    /*     Connects: task11.task1_acc2_out -> task12.task2_acc2_in         */
    /* ------------------------------------------------------------------ */
    CREATE_BLACKBOARD(
        "bb_acc2",
        sizeof(integer),
        &(ps1_bb_acc2_id),
        &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_acc2)", ret);

    /* ------------------------------------------------------------------ */
    /*  4. Create task11 (AADL: Tsk11)                                     */
    /*     Period=25ms  Priority=2  Stack=8KB  Deadline=SOFT               */
    /* ------------------------------------------------------------------ */
    strcpy(tattr.NAME, "task11");
    tattr.ENTRY_POINT    = task11_job;
    tattr.BASE_PRIORITY  = 2;
    tattr.PERIOD         = 25000000ll;   /* 25 ms in nanoseconds */
    tattr.STACK_SIZE     = 8192;
    tattr.TIME_CAPACITY  = 25000000ll;
    tattr.DEADLINE       = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[0]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task11)", ret);
    START(arinc_threads[0], &(ret));
    CHECK_CODE("START(task11)", ret);

    /* ------------------------------------------------------------------ */
    /*  5. Create intra-partition BUFFER: buf_sensor                       */
    /*     Connects: task12.task2_sensor_out -> task11.task1_sensor_in     */
    /* ------------------------------------------------------------------ */
    CREATE_BUFFER(
        "buf_sensor",
        sizeof(integer),
        1,                     /* MAX_NB_MESSAGE: sensor value (latest only) */
        FIFO,
        &(ps1_buf_sensor_id),
        &(ret));
    CHECK_CODE("CREATE_BUFFER(buf_sensor)", ret);

    /* ------------------------------------------------------------------ */
    /*  6. Create intra-partition BLACKBOARD: bb_sem                       */
    /*     Connects: sem <-> task12.task2_printer  (write_only)            */
    /*              sem <-> task13.task3_receiver  (read_only)             */
    /*     sem is AADL protected_data; ARINC 653 BLACKBOARD used for       */
    /*     mutual-exclusion-safe shared data access                        */
    /* ------------------------------------------------------------------ */
    CREATE_BLACKBOARD(
        "bb_sem",
        sizeof(integer),
        &(ps1_bb_sem_id),
        &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_sem)", ret);

    /* ------------------------------------------------------------------ */
    /*  7. Create task12 (AADL: Tsk12)                                     */
    /*     Period=50ms  Priority=3  Stack=8KB  Deadline=SOFT               */
    /* ------------------------------------------------------------------ */
    strcpy(tattr.NAME, "task12");
    tattr.ENTRY_POINT    = task12_job;
    tattr.BASE_PRIORITY  = 3;
    tattr.PERIOD         = 50000000ll;   /* 50 ms */
    tattr.STACK_SIZE     = 8192;
    tattr.TIME_CAPACITY  = 50000000ll;
    tattr.DEADLINE       = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[1]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task12)", ret);
    START(arinc_threads[1], &(ret));
    CHECK_CODE("START(task12)", ret);

    /* ------------------------------------------------------------------ */
    /*  8. Create intra-partition BUFFER: buf_t3tot4                       */
    /*     Connects: task13.task3_buffer_send -> task14.task4_buffer_recv  */
    /* ------------------------------------------------------------------ */
    CREATE_BUFFER(
        "buf_t3tot4",
        sizeof(integer),
        10,                    /* MAX_NB_MESSAGE */
        FIFO,
        &(ps1_buf_t3tot4_id),
        &(ret));
    CHECK_CODE("CREATE_BUFFER(buf_t3tot4)", ret);

    /* ------------------------------------------------------------------ */
    /*  9. Create intra-partition BUFFER: buf_t4tot3                       */
    /*     Connects: task14.task4_buffer_send -> task13.task3_buffer_recv  */
    /* ------------------------------------------------------------------ */
    CREATE_BUFFER(
        "buf_t4tot3",
        sizeof(integer),
        10,
        FIFO,
        &(ps1_buf_t4tot3_id),
        &(ret));
    CHECK_CODE("CREATE_BUFFER(buf_t4tot3)", ret);

    /* ------------------------------------------------------------------ */
    /* 10. Create intra-partition BLACKBOARD: bb_t3t4                      */
    /*     Connects: task13.task3_blackboard_write -> task14.task4_bb_read */
    /* ------------------------------------------------------------------ */
    CREATE_BLACKBOARD(
        "bb_t3t4",
        sizeof(integer),
        &(ps1_bb_t3t4_id),
        &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_t3t4)", ret);

    /* ------------------------------------------------------------------ */
    /* 11. Create intra-partition BLACKBOARD: bb_t4t3                      */
    /*     Connects: task14.task4_blackboard_write -> task13.task3_bb_read */
    /* ------------------------------------------------------------------ */
    CREATE_BLACKBOARD(
        "bb_t4t3",
        sizeof(integer),
        &(ps1_bb_t4t3_id),
        &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_t4t3)", ret);

    /* ------------------------------------------------------------------ */
    /* 12. Create task13 (AADL: Tsk13)                                     */
    /*     Period=50ms  Priority=4  Stack=8KB  Deadline=SOFT               */
    /* ------------------------------------------------------------------ */
    strcpy(tattr.NAME, "task13");
    tattr.ENTRY_POINT    = task13_job;
    tattr.BASE_PRIORITY  = 4;
    tattr.PERIOD         = 50000000ll;
    tattr.STACK_SIZE     = 8192;
    tattr.TIME_CAPACITY  = 50000000ll;
    tattr.DEADLINE       = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[2]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task13)", ret);
    START(arinc_threads[2], &(ret));
    CHECK_CODE("START(task13)", ret);

    /* ------------------------------------------------------------------ */
    /* 13. Create inter-partition QUEUING port                             */
    /*     AADL: pr1queueingin: in event data port Base_Types::Integer     */
    /*           Queue_Size => 30  Timeout => 5ns  Queueing => FIFO        */
    /* ------------------------------------------------------------------ */
    CREATE_QUEUING_PORT(
        "pr1queueingin",
        sizeof(integer),       /* MAX_MESSAGE_SIZE */
        30,                    /* MAX_NB_MESSAGE   */
        DESTINATION,           /* PORT_DIRECTION   */
        FIFO,                  /* QUEUING_DISCIPLINE */
        &(ps1_pr1queueingin_id),
        &(ret));
    CHECK_CODE("CREATE_QUEUING_PORT(pr1queueingin)", ret);

    /* ------------------------------------------------------------------ */
    /* 14. Create task14 (AADL: Tsk14)                                     */
    /*     Period=50ms  Priority=5  Stack=8KB  Deadline=SOFT               */
    /* ------------------------------------------------------------------ */
    strcpy(tattr.NAME, "task14");
    tattr.ENTRY_POINT    = task14_job;
    tattr.BASE_PRIORITY  = 5;
    tattr.PERIOD         = 50000000ll;
    tattr.STACK_SIZE     = 8192;
    tattr.TIME_CAPACITY  = 50000000ll;
    tattr.DEADLINE       = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[3]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task14)", ret);
    START(arinc_threads[3], &(ret));
    CHECK_CODE("START(task14)", ret);

    /* ------------------------------------------------------------------ */
    /* 15. Switch partition to NORMAL operating mode                        */
    /* ------------------------------------------------------------------ */
    SET_PARTITION_MODE(NORMAL, &(ret));
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);
    /*  appMain returns; partition now runs in NORMAL mode under the RTOS. */
    return;
}
