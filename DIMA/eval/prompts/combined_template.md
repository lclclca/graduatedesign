# 组合提示词模板（Combined: Few-Shot + CoT）

**使用方法**：
1. 将下方 `{{SPEC_JSON}}` 替换为目标分区的 `specs/psX.json` 内容
2. 将 `{{PARTITION_NAME}}` 替换为分区名（如 `ps4`）
3. 将整个提示词粘贴给 AI
4. 将结果保存到 `DIMA/eval/generated/combined/{{PARTITION_NAME}}/`

> **注意**：ps1 已作为固定示例嵌入本模板，请勿对 ps1 使用本模板。

---

## 提示词正文（从下方横线后复制）

---

你是一名嵌入式航空软件工程师，熟悉 ARINC 653 标准和 ACoreOS653 实时操作系统。

以下提供了：① 一个完整的参考示例（ps1），② ARINC 653 API 参考，③ 目标分区规格。
请先逐步分析，再生成完整的 11 个 C 文件。

---

## 第一部分：参考示例（ps1）

### ps1/main.c

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

PROCESS_ID_TYPE arinc_threads[ACoreOS653_CONFIG_NB_THREADS];

SAMPLING_PORT_ID_TYPE ps1_pr1samplingout_id;
QUEUING_PORT_ID_TYPE  ps1_pr1queueingin_id;
BLACKBOARD_ID_TYPE ps1_bb_acc1_id;
BLACKBOARD_ID_TYPE ps1_bb_acc2_id;
BLACKBOARD_ID_TYPE ps1_bb_sem_id;
BLACKBOARD_ID_TYPE ps1_bb_t3t4_id;
BLACKBOARD_ID_TYPE ps1_bb_t4t3_id;
BUFFER_ID_TYPE ps1_buf_sensor_id;
BUFFER_ID_TYPE ps1_buf_t3tot4_id;
BUFFER_ID_TYPE ps1_buf_t4tot3_id;

void appMain(void)
{
    PROCESS_ATTRIBUTE_TYPE tattr;
    RETURN_CODE_TYPE ret = NO_ERROR;

    CREATE_SAMPLING_PORT("pr1samplingout", sizeof(integer), SOURCE, 100ll,
                         &(ps1_pr1samplingout_id), &(ret));
    CHECK_CODE("CREATE_SAMPLING_PORT(pr1samplingout)", ret);

    CREATE_BLACKBOARD("bb_acc1", sizeof(integer), &(ps1_bb_acc1_id), &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_acc1)", ret);
    CREATE_BLACKBOARD("bb_acc2", sizeof(integer), &(ps1_bb_acc2_id), &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_acc2)", ret);

    strcpy(tattr.NAME, "task11"); tattr.ENTRY_POINT = task11_job;
    tattr.BASE_PRIORITY = 2; tattr.PERIOD = 25000000ll;
    tattr.STACK_SIZE = 8192; tattr.TIME_CAPACITY = 25000000ll; tattr.DEADLINE = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[0]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task11)", ret);
    START(arinc_threads[0], &(ret));
    CHECK_CODE("START(task11)", ret);

    CREATE_BUFFER("buf_sensor", sizeof(integer), 1, FIFO, &(ps1_buf_sensor_id), &(ret));
    CHECK_CODE("CREATE_BUFFER(buf_sensor)", ret);
    CREATE_BLACKBOARD("bb_sem", sizeof(integer), &(ps1_bb_sem_id), &(ret));
    CHECK_CODE("CREATE_BLACKBOARD(bb_sem)", ret);

    strcpy(tattr.NAME, "task12"); tattr.ENTRY_POINT = task12_job;
    tattr.BASE_PRIORITY = 3; tattr.PERIOD = 50000000ll;
    tattr.STACK_SIZE = 8192; tattr.TIME_CAPACITY = 50000000ll; tattr.DEADLINE = SOFT;
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

    strcpy(tattr.NAME, "task13"); tattr.ENTRY_POINT = task13_job;
    tattr.BASE_PRIORITY = 4; tattr.PERIOD = 50000000ll;
    tattr.STACK_SIZE = 8192; tattr.TIME_CAPACITY = 50000000ll; tattr.DEADLINE = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[2]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task13)", ret);
    START(arinc_threads[2], &(ret));
    CHECK_CODE("START(task13)", ret);

    CREATE_QUEUING_PORT("pr1queueingin", sizeof(integer), 30, DESTINATION, FIFO,
                        &(ps1_pr1queueingin_id), &(ret));
    CHECK_CODE("CREATE_QUEUING_PORT(pr1queueingin)", ret);

    strcpy(tattr.NAME, "task14"); tattr.ENTRY_POINT = task14_job;
    tattr.BASE_PRIORITY = 5; tattr.PERIOD = 50000000ll;
    tattr.STACK_SIZE = 8192; tattr.TIME_CAPACITY = 50000000ll; tattr.DEADLINE = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[3]), &(ret));
    CHECK_CODE("CREATE_PROCESS(task14)", ret);
    START(arinc_threads[3], &(ret));
    CHECK_CODE("START(task14)", ret);

    SET_PARTITION_MODE(NORMAL, &(ret));
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);
    return;
}

void module_HM_callback(ERROR_STATUS_TYPE *error_status) {
    printf("[HM MODULE ps1] ErrorID=%d  FailedProcess=%d  State=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID,
           (int)error_status->SYSTEM_STATE);
}
void partition_HM_callback(ERROR_STATUS_TYPE *error_status) {
    printf("[HM PARTITION ps1] ErrorID=%d  FailedProcess=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID);
}
```

### ps1/activity.c（节选关键任务）

```c
#include <os/pos/apex/apexLib.h>
#include <stdio.h>
#include "activity.h"
#include "gtypes.h"
#include "deployment.h"
#include "globals.h"
#include "subprograms.h"

extern SAMPLING_PORT_ID_TYPE ps1_pr1samplingout_id;
extern BLACKBOARD_ID_TYPE    ps1_bb_acc1_id;
extern BUFFER_ID_TYPE        ps1_buf_sensor_id;

void* task11_job(void) {
    RETURN_CODE_TYPE  ret = NO_ERROR;
    MESSAGE_SIZE_TYPE buf_sensor_len;
    integer ps1_buf_sensor_val = 0, ps1_bb_acc1_val = 0;
    integer ps1_pr1samplingout_val = 0;
    while (1) {
        READ_BUFFER(ps1_buf_sensor_id, 0ll,
                    (MESSAGE_ADDR_TYPE)&ps1_buf_sensor_val, &buf_sensor_len, &ret);
        CHECK_CODE("READ_BUFFER(buf_sensor)", ret);
        DISPLAY_BLACKBOARD(ps1_bb_acc1_id,
                           (MESSAGE_ADDR_TYPE)&ps1_bb_acc1_val, sizeof(integer), &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD(bb_acc1)", ret);
        WRITE_SAMPLING_MESSAGE(ps1_pr1samplingout_id,
                               (MESSAGE_ADDR_TYPE)&ps1_pr1samplingout_val, sizeof(integer), &ret);
        CHECK_CODE("WRITE_SAMPLING_MESSAGE(pr1samplingout)", ret);
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task11)", ret);
    }
}

extern QUEUING_PORT_ID_TYPE ps1_pr1queueingin_id;
extern BLACKBOARD_ID_TYPE   ps1_bb_t3t4_id;
extern BUFFER_ID_TYPE       ps1_buf_t3tot4_id;

void* task14_job(void) {
    RETURN_CODE_TYPE  ret = NO_ERROR;
    MESSAGE_SIZE_TYPE qin_len, buf_len, bb_len;
    integer ps1_pr1queueingin_val = 0, ps1_buf_t3tot4_val = 0;
    integer ps1_bb_t3t4_val = 0, ps1_bb_t4t3_val = 0, ps1_buf_t4tot3_val = 0;
    while (1) {
        RECEIVE_QUEUING_MESSAGE(ps1_pr1queueingin_id, 5000000ll,
                                (MESSAGE_ADDR_TYPE)&ps1_pr1queueingin_val, &qin_len, &ret);
        CHECK_CODE("RECEIVE_QUEUING_MESSAGE(pr1queueingin)", ret);
        READ_BUFFER(ps1_buf_t3tot4_id, 5000000ll,
                    (MESSAGE_ADDR_TYPE)&ps1_buf_t3tot4_val, &buf_len, &ret);
        CHECK_CODE("READ_BUFFER(buf_t3tot4)", ret);
        calculate_spg(&ps1_bb_t3t4_val, &ps1_bb_t4t3_val);
        DISPLAY_BLACKBOARD(ps1_bb_t4t3_id,
                           (MESSAGE_ADDR_TYPE)&ps1_bb_t4t3_val, sizeof(integer), &ret);
        CHECK_CODE("DISPLAY_BLACKBOARD(bb_t4t3)", ret);
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task14)", ret);
    }
}
```

---

## 第二部分：ARINC 653 API 参考

```c
/* 端口创建（appMain 中） */
CREATE_SAMPLING_PORT("名称", sizeof(integer), SOURCE/DESTINATION, 刷新周期ns, &id, &ret);
CREATE_QUEUING_PORT("名称", sizeof(integer), 最大消息数, SOURCE/DESTINATION, FIFO, &id, &ret);
CREATE_BLACKBOARD("名称", sizeof(integer), &id, &ret);
CREATE_BUFFER("名称", sizeof(integer), 最大消息数, FIFO, &id, &ret);

/* 任务创建（appMain 中） */
/* tattr.PERIOD = period_ms * 1000000ll  （纳秒） */
CREATE_PROCESS(&tattr, &arinc_threads[N], &ret);
START(arinc_threads[N], &ret);

/* 通信 API（任务 while(1) 中） */
WRITE_SAMPLING_MESSAGE(id, (MESSAGE_ADDR_TYPE)&val, sizeof(integer), &ret);      /* 采样SOURCE写 */
READ_SAMPLING_MESSAGE(id, (MESSAGE_ADDR_TYPE)&val, &len, &validity, &ret);       /* 采样DEST读  */
SEND_QUEUING_MESSAGE(id, (MESSAGE_ADDR_TYPE)&val, sizeof(integer), timeout, &ret); /* 队列SOURCE写 */
RECEIVE_QUEUING_MESSAGE(id, timeout, (MESSAGE_ADDR_TYPE)&val, &len, &ret);       /* 队列DEST读  */
DISPLAY_BLACKBOARD(id, (MESSAGE_ADDR_TYPE)&val, sizeof(integer), &ret);          /* 黑板写 */
READ_BLACKBOARD(id, timeout, (MESSAGE_ADDR_TYPE)&val, &len, &ret);               /* 黑板读 */
WRITE_BUFFER(id, (MESSAGE_ADDR_TYPE)&val, sizeof(integer), timeout, &ret);       /* 缓冲区写 */
READ_BUFFER(id, timeout, (MESSAGE_ADDR_TYPE)&val, &len, &ret);                   /* 缓冲区读 */
PERIODIC_WAIT(&ret);   /* 每个任务 while(1) 末尾必须调用 */

/* 分区模式（appMain 最后） */
SET_PARTITION_MODE(NORMAL, &ret);
```

---

## 第三部分：目标分区规格

```json
{{SPEC_JSON}}
```

---

## 请按以下步骤推理，然后生成代码

**步骤 1：分析通信资源**
- 列出所有需要 CREATE 的资源（端口类型/方向、黑板、缓冲区）

**步骤 2：规划 appMain 创建顺序**
- 通信资源在依赖它的任务之前创建；最后调用 SET_PARTITION_MODE(NORMAL)

**步骤 3：分析每个任务的 READ/WRITE 操作**
- 每个任务：读哪些资源？写哪些资源？调用哪些子程序？

**步骤 4：生成 11 个文件的完整代码**（参照 ps1 的风格）

## 代码规范

- **任务函数命名（严格）**：每个任务函数名必须为 `<name>_job`，其中 `<name>` 完全等于规格 JSON 中 `tasks[].name` 的值。如上例 ps1 中 `"name": "task11"` → `task11_job`。activity.h 声明、activity.c 实现、`tattr.ENTRY_POINT`、`strcpy(tattr.NAME, ...)` 字符串，均必须使用完全相同的名称。
- 全局变量前缀：`{{PARTITION_NAME}}_`
- 端口 ID 在 `main.c` 全局声明，`activity.c` 中用 `extern` 引用
- **不要**在 `while(1)` 内调用 `CREATE_*` 或 `GET_*_PORT_ID`
- 每次 APEX 调用后调用 `CHECK_CODE("描述", ret)`
- 所有头文件有 `#ifndef`/`#define`/`#endif` 保护

请逐一输出每个文件的完整代码，格式为：
```
=== 文件名 ===
（完整代码）
```
