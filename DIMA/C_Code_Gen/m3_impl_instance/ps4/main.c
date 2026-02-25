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
/*  Partition: ps4 (P4)  Module: M3                 */
/*  AADL model: DIMA_partitions::P4.impl            */
/*  Processor: arinckernel.module3  Part: part4      */
/*  DAL: LEVEL_A                                     */
/*****************************************************/

/* Array of ARINC 653 process (task) handles */
PROCESS_ID_TYPE arinc_threads[ACoreOS653_CONFIG_NB_THREADS];

/* ---- Inter-partition queuing ports ---- */
/* order_in:    DESTINATION port, receives orders from ps1/ps2 */
QUEUING_PORT_ID_TYPE ps4_order_in_id;
/* queueingout: SOURCE port, forwards processed data */
QUEUING_PORT_ID_TYPE ps4_queueingout_id;
/* queueingin:  DESTINATION port, receives feedback from ps5 */
QUEUING_PORT_ID_TYPE ps4_queueingin_id;

void appMain(void)
{
    PROCESS_ATTRIBUTE_TYPE tattr;
    RETURN_CODE_TYPE ret = NO_ERROR;

    /* ------------------------------------------------------------------ */
    /*  1. Create inter-partition QUEUING port: order_in                   */
    /*     AADL: order_in: in event data port Base_Types::Integer          */
    /*           Queue_Size => 1  Queueing => FIFO                         */
    /* ------------------------------------------------------------------ */
    CREATE_QUEUING_PORT(
        "order_in",
        sizeof(integer),       /* MAX_MESSAGE_SIZE */
        1,                     /* MAX_NB_MESSAGE   */
        DESTINATION,           /* PORT_DIRECTION   */
        FIFO,                  /* QUEUING_DISCIPLINE */
        &(ps4_order_in_id),
        &(ret));
    CHECK_CODE("CREATE_QUEUING_PORT(order_in)", ret);

    /* ------------------------------------------------------------------ */
    /*  2. Create inter-partition QUEUING port: queueingout                */
    /*     AADL: queueingout: out event data port Base_Types::Integer      */
    /*           Queue_Size => 4  Queueing => FIFO                         */
    /* ------------------------------------------------------------------ */
    CREATE_QUEUING_PORT(
        "queueingout",
        sizeof(integer),       /* MAX_MESSAGE_SIZE */
        4,                     /* MAX_NB_MESSAGE   */
        SOURCE,                /* PORT_DIRECTION   */
        FIFO,                  /* QUEUING_DISCIPLINE */
        &(ps4_queueingout_id),
        &(ret));
    CHECK_CODE("CREATE_QUEUING_PORT(queueingout)", ret);

    /* ------------------------------------------------------------------ */
    /*  3. Create inter-partition QUEUING port: queueingin                 */
    /*     AADL: queueingin: in event data port Base_Types::Integer        */
    /*           Queue_Size => 4  Queueing => FIFO                         */
    /* ------------------------------------------------------------------ */
    CREATE_QUEUING_PORT(
        "queueingin",
        sizeof(integer),       /* MAX_MESSAGE_SIZE */
        4,                     /* MAX_NB_MESSAGE   */
        DESTINATION,           /* PORT_DIRECTION   */
        FIFO,                  /* QUEUING_DISCIPLINE */
        &(ps4_queueingin_id),
        &(ret));
    CHECK_CODE("CREATE_QUEUING_PORT(queueingin)", ret);

    /* ------------------------------------------------------------------ */
    /*  4. Create task41 (AADL: Tsk41)                                     */
    /*     Period=25ms  Priority=2  Stack=8KB  Deadline=SOFT               */
    /* ------------------------------------------------------------------ */
    strcpy(tattr.NAME, "task41");
    tattr.ENTRY_POINT    = task41_job;
    tattr.BASE_PRIORITY  = 2;
    tattr.PERIOD         = 25000000ll;   /* 25 ms in nanoseconds */
    tattr.STACK_SIZE     = 8192;
    tattr.TIME_CAPACITY  = 50000000ll;
    tattr.DEADLINE       = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[0]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task41)", ret);
    START(arinc_threads[0], &(ret));
    CHECK_CODE("START(task41)", ret);

    /* ------------------------------------------------------------------ */
    /*  5. Create task42 (AADL: Tsk42)                                     */
    /*     Period=50ms  Priority=3  Stack=8KB  Deadline=SOFT               */
    /* ------------------------------------------------------------------ */
    strcpy(tattr.NAME, "task42");
    tattr.ENTRY_POINT    = task42_job;
    tattr.BASE_PRIORITY  = 3;
    tattr.PERIOD         = 50000000ll;   /* 50 ms */
    tattr.STACK_SIZE     = 8192;
    tattr.TIME_CAPACITY  = 50000000ll;
    tattr.DEADLINE       = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[1]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task42)", ret);
    START(arinc_threads[1], &(ret));
    CHECK_CODE("START(task42)", ret);

    /* ------------------------------------------------------------------ */
    /*  6. Create task43 (AADL: Tsk43)                                     */
    /*     Period=50ms  Priority=4  Stack=8KB  Deadline=SOFT               */
    /* ------------------------------------------------------------------ */
    strcpy(tattr.NAME, "task43");
    tattr.ENTRY_POINT    = task43_job;
    tattr.BASE_PRIORITY  = 4;
    tattr.PERIOD         = 50000000ll;
    tattr.STACK_SIZE     = 8192;
    tattr.TIME_CAPACITY  = 50000000ll;
    tattr.DEADLINE       = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[2]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task43)", ret);
    START(arinc_threads[2], &(ret));
    CHECK_CODE("START(task43)", ret);

    /* ------------------------------------------------------------------ */
    /*  7. Create task44 (AADL: Tsk44)                                     */
    /*     Period=100ms  Priority=5  Stack=8KB  Deadline=SOFT              */
    /* ------------------------------------------------------------------ */
    strcpy(tattr.NAME, "task44");
    tattr.ENTRY_POINT    = task44_job;
    tattr.BASE_PRIORITY  = 5;
    tattr.PERIOD         = 100000000ll;  /* 100 ms */
    tattr.STACK_SIZE     = 8192;
    tattr.TIME_CAPACITY  = 50000000ll;
    tattr.DEADLINE       = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[3]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task44)", ret);
    START(arinc_threads[3], &(ret));
    CHECK_CODE("START(task44)", ret);

    /* ------------------------------------------------------------------ */
    /*  8. Create task45 (AADL: Tsk45)                                     */
    /*     Period=200ms  Priority=6  Stack=8KB  Deadline=SOFT              */
    /* ------------------------------------------------------------------ */
    strcpy(tattr.NAME, "task45");
    tattr.ENTRY_POINT    = task45_job;
    tattr.BASE_PRIORITY  = 6;
    tattr.PERIOD         = 200000000ll;  /* 200 ms */
    tattr.STACK_SIZE     = 8192;
    tattr.TIME_CAPACITY  = 50000000ll;
    tattr.DEADLINE       = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[4]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task45)", ret);
    START(arinc_threads[4], &(ret));
    CHECK_CODE("START(task45)", ret);

    /* ------------------------------------------------------------------ */
    /*  9. Switch partition to NORMAL operating mode                        */
    /* ------------------------------------------------------------------ */
    SET_PARTITION_MODE(NORMAL, &(ret));
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);
    return;
}
