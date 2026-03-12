# 少样本提示词模板（Few-Shot）

**使用方法**：
1. 将下方 `{{SPEC_JSON}}` 替换为目标分区的 `specs/psX.json` 内容（X = 2/3/4/5）
2. 将 `{{PARTITION_NAME}}` 替换为分区名（如 `ps2`）
3. 将整个提示词粘贴给 AI
4. 将结果保存到 `DIMA/eval/generated/fewshot/{{PARTITION_NAME}}/`

> **注意**：ps1 已作为固定示例嵌入本模板，请勿对 ps1 使用本模板。

---

## 提示词正文（从下方横线后复制）

---

你是一名嵌入式航空软件工程师，熟悉 ARINC 653 标准和 ACoreOS653 实时操作系统。

以下是一个完整的 ARINC 653 分区代码示例（分区 ps1），请仔细学习其代码结构和风格，
然后为另一个分区生成风格一致的代码。

---

## 示例：分区 ps1 的完整代码

### 示例文件 1：ps1/main.c

```c
#include <os/pos/apex/apexLib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

/* Array of ARINC 653 process (task) handles */
PROCESS_ID_TYPE arinc_threads[ACoreOS653_CONFIG_NB_THREADS];

/* ---- Inter-partition ports ---- */
SAMPLING_PORT_ID_TYPE ps1_pr1samplingout_id;
QUEUING_PORT_ID_TYPE  ps1_pr1queueingin_id;

/* ---- Intra-partition blackboards ---- */
BLACKBOARD_ID_TYPE ps1_bb_acc1_id;
BLACKBOARD_ID_TYPE ps1_bb_acc2_id;
BLACKBOARD_ID_TYPE ps1_bb_sem_id;
BLACKBOARD_ID_TYPE ps1_bb_t3t4_id;
BLACKBOARD_ID_TYPE ps1_bb_t4t3_id;

/* ---- Intra-partition buffers ---- */
BUFFER_ID_TYPE ps1_buf_sensor_id;
BUFFER_ID_TYPE ps1_buf_t3tot4_id;
BUFFER_ID_TYPE ps1_buf_t4tot3_id;

void appMain(void)
{
    PROCESS_ATTRIBUTE_TYPE tattr;
    RETURN_CODE_TYPE ret = NO_ERROR;

    CREATE_SAMPLING_PORT(
        "pr1samplingout",
        sizeof(integer),
        SOURCE,
        100ll,
        &(ps1_pr1samplingout_id),
        &(ret));
    CHECK_CODE("CREATE_SAMPLING_PORT(pr1samplingout)", ret);

    CREATE_BLACKBOARD("bb_acc1", sizeof(integer), &(ps1_bb_acc1_id), &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_acc1)", ret);

    CREATE_BLACKBOARD("bb_acc2", sizeof(integer), &(ps1_bb_acc2_id), &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_acc2)", ret);

    strcpy(tattr.NAME, "task11");
    tattr.ENTRY_POINT   = task11_job;
    tattr.BASE_PRIORITY = 2;
    tattr.PERIOD        = 25000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 25000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[0]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task11)", ret);
    START(arinc_threads[0], &(ret));
    CHECK_CODE("START(task11)", ret);

    CREATE_BUFFER("buf_sensor", sizeof(integer), 1, FIFO, &(ps1_buf_sensor_id), &(ret));
    CHECK_CODE("CREATE_BUFFER(buf_sensor)", ret);

    CREATE_BLACKBOARD("bb_sem", sizeof(integer), &(ps1_bb_sem_id), &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_sem)", ret);

    strcpy(tattr.NAME, "task12");
    tattr.ENTRY_POINT   = task12_job;
    tattr.BASE_PRIORITY = 3;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[1]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task12)", ret);
    START(arinc_threads[1], &(ret));
    CHECK_CODE("START(task12)", ret);

    CREATE_BUFFER("buf_t3tot4", sizeof(integer), 10, FIFO, &(ps1_buf_t3tot4_id), &(ret));
    CHECK_CODE("CREATE_BUFFER(buf_t3tot4)", ret);

    CREATE_BUFFER("buf_t4tot3", sizeof(integer), 10, FIFO, &(ps1_buf_t4tot3_id), &(ret));
    CHECK_CODE("CREATE_BUFFER(buf_t4tot3)", ret);

    CREATE_BLACKBOARD("bb_t3t4", sizeof(integer), &(ps1_bb_t3t4_id), &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_t3t4)", ret);

    CREATE_BLACKBOARD("bb_t4t3", sizeof(integer), &(ps1_bb_t4t3_id), &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_t4t3)", ret);

    strcpy(tattr.NAME, "task13");
    tattr.ENTRY_POINT   = task13_job;
    tattr.BASE_PRIORITY = 4;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[2]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task13)", ret);
    START(arinc_threads[2], &(ret));
    CHECK_CODE("START(task13)", ret);

    CREATE_QUEUING_PORT(
        "pr1queueingin",
        sizeof(integer),
        30,
        DESTINATION,
        FIFO,
        &(ps1_pr1queueingin_id),
        &(ret));
    CHECK_CODE("CREATE_QUEUING_PORT(pr1queueingin)", ret);

    strcpy(tattr.NAME, "task14");
    tattr.ENTRY_POINT   = task14_job;
    tattr.BASE_PRIORITY = 5;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[3]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task14)", ret);
    START(arinc_threads[3], &(ret));
    CHECK_CODE("START(task14)", ret);

    SET_PARTITION_MODE(NORMAL, &(ret));
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);
    return;
}

void module_HM_callback(ERROR_STATUS_TYPE *error_status)
{
    printf("[HM MODULE ps1] ErrorID=%d  FailedProcess=%d  State=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID,
           (int)error_status->SYSTEM_STATE);
}

void partition_HM_callback(ERROR_STATUS_TYPE *error_status)
{
    printf("[HM PARTITION ps1] ErrorID=%d  FailedProcess=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID);
}
```

### 示例文件 2：ps1/activity.c

```c
#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

#define SZ 1024

extern SAMPLING_PORT_ID_TYPE ps1_pr1samplingout_id;
extern BLACKBOARD_ID_TYPE    ps1_bb_acc1_id;
extern BLACKBOARD_ID_TYPE    ps1_bb_acc2_id;
extern BUFFER_ID_TYPE        ps1_buf_sensor_id;

void* task11_job(void)
{
    RETURN_CODE_TYPE  ret = NO_ERROR;
    MESSAGE_SIZE_TYPE buf_sensor_len;
    integer ps1_pr1samplingout_val = 0;
    integer ps1_bb_acc1_val        = 0;
    integer ps1_bb_acc2_val        = 0;
    integer ps1_buf_sensor_val     = 0;

    while (1)
    {
        READ_BUFFER(ps1_buf_sensor_id, 0ll,
                    (MESSAGE_ADDR_TYPE)&ps1_buf_sensor_val,
                    &buf_sensor_len, &ret);
        CHECK_CODE("READ_BUFFER(buf_sensor)", ret);

        commandboard_receiveinput_spg(&ps1_buf_sensor_val, &ps1_pr1samplingout_val);
        commandboard_printinfos_spg(&ps1_pr1samplingout_val);

        DISPLAY_BLACKBOARD(ps1_bb_acc1_id,
                           (MESSAGE_ADDR_TYPE)&ps1_bb_acc1_val,
                           sizeof(integer), &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD(bb_acc1)", ret);

        DISPLAY_BLACKBOARD(ps1_bb_acc2_id,
                           (MESSAGE_ADDR_TYPE)&ps1_bb_acc2_val,
                           sizeof(integer), &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD(bb_acc2)", ret);

        WRITE_SAMPLING_MESSAGE(ps1_pr1samplingout_id,
                               (MESSAGE_ADDR_TYPE)&ps1_pr1samplingout_val,
                               sizeof(integer), &ret);
        CHECK_CODE("WRITE_SAMPLING_MESSAGE(pr1samplingout)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task11)", ret);
    }
}

extern BLACKBOARD_ID_TYPE ps1_bb_sem_id;

void* task12_job(void)
{
    RETURN_CODE_TYPE  ret = NO_ERROR;
    MESSAGE_SIZE_TYPE bb_acc1_len, bb_acc2_len;
    integer ps1_bb_acc1_val    = 0;
    integer ps1_bb_acc2_val    = 0;
    integer ps1_buf_sensor_val = 0;
    integer ps1_bb_sem_val     = 0;

    while (1)
    {
        READ_BLACKBOARD(ps1_bb_acc1_id, 0ll,
                        (MESSAGE_ADDR_TYPE)&ps1_bb_acc1_val,
                        &bb_acc1_len, &ret);
        CHECK_CODE("READ_BLACKBOARD(bb_acc1)", ret);

        READ_BLACKBOARD(ps1_bb_acc2_id, 0ll,
                        (MESSAGE_ADDR_TYPE)&ps1_bb_acc2_val,
                        &bb_acc2_len, &ret);
        CHECK_CODE("READ_BLACKBOARD(bb_acc2)", ret);

        ps1_buf_sensor_val = ps1_bb_acc1_val + ps1_bb_acc2_val;
        ps1_bb_sem_val     = ps1_bb_acc1_val;

        WRITE_BUFFER(ps1_buf_sensor_id,
                     (MESSAGE_ADDR_TYPE)&ps1_buf_sensor_val,
                     sizeof(integer), 0ll, &ret);
        CHECK_CODE("WRITE_BUFFER(buf_sensor)", ret);

        DISPLAY_BLACKBOARD(ps1_bb_sem_id,
                           (MESSAGE_ADDR_TYPE)&ps1_bb_sem_val,
                           sizeof(integer), &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD(bb_sem)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task12)", ret);
    }
}

extern BLACKBOARD_ID_TYPE ps1_bb_t3t4_id;
extern BLACKBOARD_ID_TYPE ps1_bb_t4t3_id;
extern BUFFER_ID_TYPE     ps1_buf_t3tot4_id;
extern BUFFER_ID_TYPE     ps1_buf_t4tot3_id;

void* task13_job(void)
{
    RETURN_CODE_TYPE  ret = NO_ERROR;
    MESSAGE_SIZE_TYPE bb_sem_len, bb_t4t3_len, buf_t4tot3_len;
    integer ps1_bb_sem_val     = 0;
    integer ps1_buf_t4tot3_val = 0;
    integer ps1_bb_t4t3_val    = 0;
    integer ps1_buf_t3tot4_val = 0;
    integer ps1_bb_t3t4_val    = 0;

    while (1)
    {
        READ_BLACKBOARD(ps1_bb_sem_id, 0ll,
                        (MESSAGE_ADDR_TYPE)&ps1_bb_sem_val,
                        &bb_sem_len, &ret);
        CHECK_CODE("READ_BLACKBOARD(bb_sem)", ret);

        commandboard_receiveinput_spg(&ps1_bb_sem_val, &ps1_buf_t3tot4_val);

        READ_BUFFER(ps1_buf_t4tot3_id, 5000000ll,
                    (MESSAGE_ADDR_TYPE)&ps1_buf_t4tot3_val,
                    &buf_t4tot3_len, &ret);
        CHECK_CODE("READ_BUFFER(buf_t4tot3)", ret);

        READ_BLACKBOARD(ps1_bb_t4t3_id, 0ll,
                        (MESSAGE_ADDR_TYPE)&ps1_bb_t4t3_val,
                        &bb_t4t3_len, &ret);
        CHECK_CODE("READ_BLACKBOARD(bb_t4t3)", ret);

        ps1_bb_t3t4_val = ps1_buf_t4tot3_val + ps1_bb_t4t3_val;

        WRITE_BUFFER(ps1_buf_t3tot4_id,
                     (MESSAGE_ADDR_TYPE)&ps1_buf_t3tot4_val,
                     sizeof(integer), 5000000ll, &ret);
        CHECK_CODE("WRITE_BUFFER(buf_t3tot4)", ret);

        DISPLAY_BLACKBOARD(ps1_bb_t3t4_id,
                           (MESSAGE_ADDR_TYPE)&ps1_bb_t3t4_val,
                           sizeof(integer), &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD(bb_t3t4)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task13)", ret);
    }
}

extern QUEUING_PORT_ID_TYPE ps1_pr1queueingin_id;

void* task14_job(void)
{
    RETURN_CODE_TYPE  ret = NO_ERROR;
    MESSAGE_SIZE_TYPE qin_len, buf_t3tot4_len, bb_t3t4_len;
    integer ps1_pr1queueingin_val = 0;
    integer ps1_buf_t3tot4_val    = 0;
    integer ps1_bb_t3t4_val       = 0;
    integer ps1_buf_t4tot3_val    = 0;
    integer ps1_bb_t4t3_val       = 0;

    while (1)
    {
        RECEIVE_QUEUING_MESSAGE(ps1_pr1queueingin_id, 5000000ll,
                                (MESSAGE_ADDR_TYPE)&ps1_pr1queueingin_val,
                                &qin_len, &ret);
        CHECK_CODE("RECEIVE_QUEUING_MESSAGE(pr1queueingin)", ret);

        READ_BUFFER(ps1_buf_t3tot4_id, 5000000ll,
                    (MESSAGE_ADDR_TYPE)&ps1_buf_t3tot4_val,
                    &buf_t3tot4_len, &ret);
        CHECK_CODE("READ_BUFFER(buf_t3tot4)", ret);

        READ_BLACKBOARD(ps1_bb_t3t4_id, 0ll,
                        (MESSAGE_ADDR_TYPE)&ps1_bb_t3t4_val,
                        &bb_t3t4_len, &ret);
        CHECK_CODE("READ_BLACKBOARD(bb_t3t4)", ret);

        calculate_spg(&ps1_bb_t3t4_val, &ps1_bb_t4t3_val);

        ps1_buf_t4tot3_val = ps1_buf_t3tot4_val + ps1_pr1queueingin_val;
        WRITE_BUFFER(ps1_buf_t4tot3_id,
                     (MESSAGE_ADDR_TYPE)&ps1_buf_t4tot3_val,
                     sizeof(integer), 5000000ll, &ret);
        CHECK_CODE("WRITE_BUFFER(buf_t4tot3)", ret);

        DISPLAY_BLACKBOARD(ps1_bb_t4t3_id,
                           (MESSAGE_ADDR_TYPE)&ps1_bb_t4t3_val,
                           sizeof(integer), &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD(bb_t4t3)", ret);

        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task14)", ret);
    }
}
```

---

## 目标分区规格

现在请为以下分区生成完整的 11 个 C 文件，风格与上面 ps1 示例完全一致：

```json
{{SPEC_JSON}}
```

## 要求

- 11 个文件：deployment.h/.c、gtypes.h/.c、globals.h/.c、subprograms.h/.c、activity.h/.c、main.c
- 命名规范：全局变量前缀使用 `{{PARTITION_NAME}}_`（如 `ps2_bb_acc3_id`）
- 所有头文件有 `#ifndef`/`#define`/`#endif` 保护
- 任务周期单位为纳秒（ms × 1,000,000）
- 端口 ID 在 `appMain` 中创建，在 `activity.c` 中用 `extern` 引用
- 每次 APEX 调用后用 `CHECK_CODE` 检查返回值

请逐一输出每个文件的完整代码，格式为：
```
=== 文件名 ===
（完整代码）
```
