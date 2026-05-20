你是一名嵌入式航空软件工程师，熟悉 ARINC 653 标准和 ACoreOS653 实时操作系统。

下面给出一个完整的示例：一个 ARINC 653 分区的规格 JSON，以及对应的 11 个 C 文件。
请仔细学习该示例的代码结构和风格，然后为新的目标分区生成风格一致的代码。

---

## 示例

### 示例规格 JSON（分区 ps3）

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
        RECEIVE_QUEUING_MESSAGE(ps3_order_in_id,
            5000000ll,
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
        RECEIVE_QUEUING_MESSAGE(ps3_order_in_id,
            5000000ll,
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
        RECEIVE_QUEUING_MESSAGE(ps3_queueingin_id,
            5000000ll,
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

    CREATE_QUEUING_PORT(
        "order_in",
        sizeof(integer),
        10,
        DESTINATION,
        FIFO,
        &(ps3_order_in_id), &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(order_in)", ret);

    CREATE_QUEUING_PORT(
        "queueingin",
        sizeof(integer),
        4,
        DESTINATION,
        FIFO,
        &(ps3_queueingin_id), &ret);
    CHECK_CODE("CREATE_QUEUING_PORT(queueingin)", ret);

    SET_PARTITION_MODE(NORMAL, &ret);
    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);
    return;
}

void module_HM_callback(ERROR_STATUS_TYPE *error_status)
{
    printf("[HM MODULE ps3] ErrorID=%d  FailedProcess=%d  State=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID,
           (int)error_status->SYSTEM_STATE);
}

void partition_HM_callback(ERROR_STATUS_TYPE *error_status)
{
    printf("[HM PARTITION ps3] ErrorID=%d  FailedProcess=%d\n",
           (int)error_status->ERROR_IDENTIFIER,
           (int)error_status->FAILED_PROCESS_ID);
}
```

---

## 目标分区规格

现在请为以下新分区生成完整的 11 个 C 文件，风格与上面 ps3 示例完全一致：

```json
{{SPEC_JSON}}
```

## 要求

- 生成 11 个文件：`deployment.h/.c`、`globals.h/.c`、`gtypes.h/.c`、`subprograms.h/.c`、`activity.h/.c`、`main.c`
- **任务函数命名（严格）**：函数名必须为 `<name>_job`，其中 `<name>` 完全等于规格 JSON 中 `tasks[].name` 的值。`activity.h` 声明、`activity.c` 实现、`tattr.ENTRY_POINT`、`strcpy(tattr.NAME, ...)` 字符串，均必须使用完全相同的名称
- **全局变量前缀**：使用 `{{PARTITION_NAME}}_`（如 `{{PARTITION_NAME}}_order_in_id`）
- **端口 ID 全局变量**：在 `main.c` 中声明，在 `activity.c` 中用 `extern` 引用
- **周期单位为纳秒**：`period_ms` × 1,000,000（如 50ms → `50000000ll`），直接写计算结果
- **所有头文件**有 `#ifndef`/`#define`/`#endif` 保护
- **deployment.h 的宏**：`NB_THREADS`、`NB_SAMPLINGS`、`NB_QUEUEINGS`、`NB_BLACKBOARDS`、`NB_BUFFERS` 及 `STACKS_SIZE` 均从规格 JSON 的 `deployment` 字段读取；`NEEDS_ARINC653_SAMPLING/QUEUEING/BLACKBOARD/BUFFER` 在数量 > 0 时为 1，否则为 0
- **每次 APEX 调用后**用 `CHECK_CODE("描述", ret)` 检查返回值
- **不要**在 `while(1)` 内调用 `CREATE_*`

请逐一输出每个文件的完整代码，格式为：
```
=== 文件名 ===
（完整代码）
```
