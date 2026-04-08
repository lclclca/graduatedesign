# 组合提示词模板（Combined: Few-Shot + CoT）— IMA2 系统

你是一名嵌入式航空软件工程师，熟悉 ARINC 653 标准和 ACoreOS653 实时操作系统。

下面提供：① 一个完整的参考示例（ps3 规格 + 11 个 C 文件），② ARINC 653 API 参考，③ 目标分区规格。
请先按步骤分析，再生成目标分区的完整代码。

---

## 第一部分：参考示例（分区 ps3）

### 示例规格 JSON

```json
{
  "partition": "ps3",
  "module": "M2",
  "aadl_process": "DIMA_partitions::P3.impl",
  "required_files": [
    "activity.c", "activity.h", "deployment.c", "deployment.h",
    "globals.c", "globals.h", "gtypes.c", "gtypes.h",
    "main.c", "subprograms.c", "subprograms.h"
  ],
  "tasks": [
    {"name": "task31", "period_ms": 25, "priority": 2},
    {"name": "task32", "period_ms": 50, "priority": 3},
    {"name": "task33", "period_ms": 50, "priority": 4}
  ],
  "sampling_ports": [],
  "queuing_ports": [
    {"name": "order_in",   "direction": "DESTINATION", "max_nb": 10, "api_read": "RECEIVE_QUEUING_MESSAGE"},
    {"name": "queueingin", "direction": "DESTINATION", "max_nb": 4,  "api_read": "RECEIVE_QUEUING_MESSAGE"}
  ],
  "blackboards": [],
  "buffers": [],
  "subprograms": [],
  "deployment": {
    "nb_threads": 3, "nb_samplings": 0, "nb_queueings": 2,
    "nb_blackboards": 0, "nb_buffers": 0, "stacks_size": 24576
  }
}
```

### 示例输出：分区 ps3 的 11 个 C 文件

#### deployment.h

```c
#ifndef __PS3_GENERATED_DEPLOYMENT_H_
#define __PS3_GENERATED_DEPLOYMENT_H_

#define IMA2C_RUNTIME_ACoreOS653 1
#define ACoreOS653_GENERATED_CODE 1
#define ACoreOS653_CONFIG_NB_THREADS     3
#define ACoreOS653_CONFIG_NB_SAMPLINGS   0
#define ACoreOS653_CONFIG_NB_QUEUEINGS   2
#define ACoreOS653_CONFIG_NB_BLACKBOARDS 0
#define ACoreOS653_CONFIG_NB_BUFFERS     0
#define ACoreOS653_NEEDS_ARINC653_PARTITION  1
#define ACoreOS653_NEEDS_ARINC653_PROCESS    1
#define ACoreOS653_NEEDS_ARINC653_SAMPLING   0
#define ACoreOS653_NEEDS_ARINC653_QUEUEING   1
#define ACoreOS653_NEEDS_ARINC653_BLACKBOARD 0
#define ACoreOS653_NEEDS_ARINC653_BUFFER     0
#define ACoreOS653_NEEDS_ARINC653_SEMAPHORE  0
#define ACoreOS653_NEEDS_ARINC653_EVENT      0
#define ACoreOS653_NEEDS_MIDDLEWARE          1
#define ACoreOS653_NEEDS_ARINC653_TIME       1
#define ACoreOS653_CONFIG_STACKS_SIZE 24576

#endif
```

#### deployment.c

```c
#include "deployment.h"
```

#### globals.h

```c
#ifndef __PS3_GENERATED_GLOBALS_H_
#define __PS3_GENERATED_GLOBALS_H_

#include <stdio.h>
#include <os/pos/apex/apexLib.h>

#define CHECK_CODE(msg, code) \
    if ((code) == NO_ERROR) { \
        printf("%s : NO_ERROR\n", (msg)); \
    } else { \
        printf("%s : ERROR(%s)\n", (msg), codeToStr(code)); \
    }

#define CHECK_VALIDCODE(msg, validCode, ret) \
    if ((validCode) == VALID && (ret) == NO_ERROR) { \
        printf("%s : VALID/NO_ERROR\n", (msg)); \
    } else { \
        printf("%s : INVALID or ERROR(%s)\n", (msg), codeToStr(ret)); \
    }

static char *codeToStr(RETURN_CODE_TYPE retCode)
{
    switch (retCode) {
        case NO_ERROR:       return "NO_ERROR";
        case NO_ACTION:      return "NO_ACTION";
        case NOT_AVAILABLE:  return "NOT_AVAILABLE";
        case INVALID_PARAM:  return "INVALID_PARAM";
        case INVALID_CONFIG: return "INVALID_CONFIG";
        case INVALID_MODE:   return "INVALID_MODE";
        case TIMED_OUT:      return "TIMED_OUT";
        default:             break;
    }
    return "UNKNOWN";
}

#endif
```

#### globals.c

```c
#include "globals.h"
```

#### gtypes.h

```c
#ifndef __PS3_GENERATED_GTYPES_H__
#define __PS3_GENERATED_GTYPES_H__

#include <os/pos/apex/apexLib.h>

typedef int integer;

#endif
```

#### gtypes.c

```c
#include "gtypes.h"
```

#### subprograms.h

```c
#ifndef __PS3_GENERATED_SUBPROGRAMS_H_
#define __PS3_GENERATED_SUBPROGRAMS_H_

#include "gtypes.h"

#endif
```

#### subprograms.c

```c
#include "subprograms.h"
```

#### activity.h

```c
#ifndef __PS3_GENERATED_ACTIVITY_H_
#define __PS3_GENERATED_ACTIVITY_H_

void* task31_job(void);
void* task32_job(void);
void* task33_job(void);

#endif
```

#### activity.c

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

extern QUEUING_PORT_ID_TYPE ps3_order_in_id;

void* task31_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    integer val = 0;
    while (1)
    {
        RECEIVE_QUEUING_MESSAGE(ps3_order_in_id, 5000000ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("RECEIVE_QUEUING_MESSAGE(order_in)", ret);
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task31)", ret);
    }
}

extern QUEUING_PORT_ID_TYPE ps3_order_in_id;

void* task32_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    integer val = 0;
    while (1)
    {
        RECEIVE_QUEUING_MESSAGE(ps3_order_in_id, 5000000ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("RECEIVE_QUEUING_MESSAGE(order_in)", ret);
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task32)", ret);
    }
}

extern QUEUING_PORT_ID_TYPE ps3_queueingin_id;

void* task33_job(void)
{
    RETURN_CODE_TYPE ret = NO_ERROR;
    MESSAGE_SIZE_TYPE msg_len;
    integer val = 0;
    while (1)
    {
        RECEIVE_QUEUING_MESSAGE(ps3_queueingin_id, 5000000ll,
            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);
        CHECK_CODE("RECEIVE_QUEUING_MESSAGE(queueingin)", ret);
        PERIODIC_WAIT(&ret);
        CHECK_CODE("PERIODIC_WAIT(task33)", ret);
    }
}
```

#### main.c

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

/* ---- Inter-partition ports ---- */
QUEUING_PORT_ID_TYPE  ps3_order_in_id;
QUEUING_PORT_ID_TYPE  ps3_queueingin_id;

void appMain(void)
{
    PROCESS_ATTRIBUTE_TYPE tattr;
    RETURN_CODE_TYPE ret = NO_ERROR;

    strcpy(tattr.NAME, "task31");
    tattr.ENTRY_POINT   = task31_job;
    tattr.BASE_PRIORITY = 2;
    tattr.PERIOD        = 25000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[0]), &ret);
    CHECK_CODE("CREATE_PROCESS(task31)", ret);
    START(arinc_threads[0], &ret);
    CHECK_CODE("START(task31)", ret);

    strcpy(tattr.NAME, "task32");
    tattr.ENTRY_POINT   = task32_job;
    tattr.BASE_PRIORITY = 3;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[1]), &ret);
    CHECK_CODE("CREATE_PROCESS(task32)", ret);
    START(arinc_threads[1], &ret);
    CHECK_CODE("START(task32)", ret);

    strcpy(tattr.NAME, "task33");
    tattr.ENTRY_POINT   = task33_job;
    tattr.BASE_PRIORITY = 4;
    tattr.PERIOD        = 50000000ll;
    tattr.STACK_SIZE    = 8192;
    tattr.TIME_CAPACITY = 50000000ll;
    tattr.DEADLINE      = SOFT;
    CREATE_PROCESS(&(tattr), &(arinc_threads[2]), &ret);
    CHECK_CODE("CREATE_PROCESS(task33)", ret);
    START(arinc_threads[2], &ret);
    CHECK_CODE("START(task33)", ret);

    CREATE_QUEUING_PORT("order_in", sizeof(integer), 10, DESTINATION, FIFO,
                        &(ps3_order_in_id), &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(order_in)", ret);

    CREATE_QUEUING_PORT("queueingin", sizeof(integer), 4, DESTINATION, FIFO,
                        &(ps3_queueingin_id), &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(queueingin)", ret);

    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);
    return;
}

void module_HM_callback(ERROR_STATUS_TYPE *error_status) {
    printf("[HM MODULE ps3] ErrorID=%d  FailedProcess=%d  State=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID,
           (int)error_status->SYSTEM_STATE);
}

void partition_HM_callback(ERROR_STATUS_TYPE *error_status) {
    printf("[HM PARTITION ps3] ErrorID=%d  FailedProcess=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID);
}
```

---

## 第二部分：ARINC 653 API 补充参考

ps3 示例只涉及队列端口。若目标分区含有采样端口、黑板或缓冲区，参考以下模式：

```c
/* ── 采样端口（inter-partition）── */
/* appMain 中创建 */
SAMPLING_PORT_ID_TYPE pX_port_id;
CREATE_SAMPLING_PORT("port_name", sizeof(integer), SOURCE,      100ll, &pX_port_id, &ret);  /* SOURCE：写 */
CREATE_SAMPLING_PORT("port_name", sizeof(integer), DESTINATION, 100ll, &pX_port_id, &ret);  /* DEST：读  */
/* 任务 while(1) 中 */
WRITE_SAMPLING_MESSAGE(pX_port_id, (MESSAGE_ADDR_TYPE)&val, sizeof(integer), &ret);         /* SOURCE 写 */
VALIDITY_TYPE validity;
READ_SAMPLING_MESSAGE(pX_port_id, (MESSAGE_ADDR_TYPE)&val, &msg_len, &validity, &ret);      /* DEST 读   */
CHECK_VALIDCODE("READ_SAMPLING_MESSAGE(port_name)", validity, ret);

/* ── 黑板（intra-partition，最新值语义）── */
/* appMain 中创建 */
BLACKBOARD_ID_TYPE pX_bb_name_id;
CREATE_BLACKBOARD("bb_name", sizeof(integer), &pX_bb_name_id, &ret);
/* 任务 while(1) 中 */
DISPLAY_BLACKBOARD(pX_bb_name_id, (MESSAGE_ADDR_TYPE)&val, sizeof(integer), &ret);         /* 写 */
READ_BLACKBOARD(pX_bb_name_id, 0ll, (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);              /* 读 */

/* ── 缓冲区（intra-partition，FIFO 队列语义）── */
/* appMain 中创建 */
BUFFER_ID_TYPE pX_buf_name_id;
CREATE_BUFFER("buf_name", sizeof(integer), max_nb, FIFO, &pX_buf_name_id, &ret);
/* 任务 while(1) 中 */
WRITE_BUFFER(pX_buf_name_id,  (MESSAGE_ADDR_TYPE)&val, sizeof(integer), 5000000ll, &ret); /* 写 */
READ_BUFFER(pX_buf_name_id,   5000000ll, (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);        /* 读 */
```

**ID 声明规则**：
- 采样/队列端口 ID 在 `main.c` 全局声明，`activity.c` 中 `extern` 引用
- 黑板/缓冲区 ID 同上（亦可在 `main.c` 全局声明后 `extern` 引用）

---

## 第三部分：目标分区规格

```json
{{SPEC_JSON}}
```

---

## 请按以下步骤推理，然后生成代码

**步骤 1：对照示例，列出差异**
- 目标分区与 ps3 相比，任务数量、端口类型、内部资源有哪些不同？

**步骤 2：规划 deployment.h 的宏值**
- 从规格 JSON 的 `deployment` 字段直接读取 `NB_THREADS`、`NB_SAMPLINGS`、`NB_QUEUEINGS`、`NB_BLACKBOARDS`、`NB_BUFFERS`、`STACKS_SIZE`
- 对应的 `NEEDS_*` 宏：数量 > 0 时置 1，否则置 0

**步骤 3：规划 main.c 的 appMain() 创建顺序**
- 每个资源（端口/黑板/缓冲区）在依赖它的 CREATE_PROCESS 之前或之后创建均可，但必须在 SET_PARTITION_MODE(NORMAL) 之前全部完成
- 列出 appMain 中所有 CREATE_xxx 调用及顺序

**步骤 4：分析每个任务的 while(1) 通信逻辑**
- 根据规格 JSON 中的端口方向和黑板/缓冲区归属，确定每个任务 READ / WRITE 哪些资源

**步骤 5：生成 11 个文件的完整代码**（风格与 ps3 示例完全一致）

## 代码规范

- **任务函数命名（严格）**：函数名必须为 `<name>_job`，其中 `<name>` 完全等于规格 JSON 中 `tasks[].name` 的值。`activity.h` 声明、`activity.c` 实现、`tattr.ENTRY_POINT`、`strcpy(tattr.NAME, ...)` 字符串，均必须使用完全相同的名称
- **全局变量前缀**：使用 `{{PARTITION_NAME}}_`
- **周期单位为纳秒**：直接写计算结果（如 50ms → `50000000ll`），不要写 `50 * 1000000ll`
- **所有头文件**有 `#ifndef`/`#define`/`#endif` 保护
- **每次 APEX 调用后**用 `CHECK_CODE("描述", ret)` 或 `CHECK_VALIDCODE(...)` 检查返回值
- **不要**在 `while(1)` 内调用 `CREATE_*`

请逐一输出每个文件的完整代码，格式为：
```
=== 文件名 ===
（完整代码）
```
