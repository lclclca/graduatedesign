# 思维链提示词模板（Chain-of-Thought）

**使用方法**：
1. 将下方 `{{SPEC_JSON}}` 替换为目标分区的 `specs/psX.json` 内容
2. 将 `{{PARTITION_NAME}}` 替换为分区名（如 `ps3`）
3. 将整个提示词粘贴给 AI
4. 将结果保存到 `DIMA/eval/generated/cot/{{PARTITION_NAME}}/`

---

## 提示词正文（从下方横线后复制）

---

你是一名嵌入式航空软件工程师，熟悉 ARINC 653 标准和 ACoreOS653 实时操作系统。

请根据以下分区规格，**先逐步分析，再生成**完整的 ARINC 653 C 代码。

## 分区规格

```json
{{SPEC_JSON}}
```

## ARINC 653 APEX API 参考

### 端口创建（在 appMain 中调用一次）

```c
/* 采样端口 */
CREATE_SAMPLING_PORT(
    "端口名",           /* PORT_NAME        */
    sizeof(integer),    /* MAX_MESSAGE_SIZE */
    SOURCE或DESTINATION,/* PORT_DIRECTION   */
    刷新周期_ns,        /* REFRESH_PERIOD（纳秒）*/
    &id变量,            /* PORT_ID          */
    &ret);

/* 队列端口 */
CREATE_QUEUING_PORT(
    "端口名",           /* PORT_NAME          */
    sizeof(integer),    /* MAX_MESSAGE_SIZE   */
    最大消息数,         /* MAX_NB_MESSAGE     */
    SOURCE或DESTINATION,/* PORT_DIRECTION     */
    FIFO,               /* QUEUING_DISCIPLINE */
    &id变量,            /* PORT_ID            */
    &ret);

/* 黑板（intra-partition，最新值覆盖） */
CREATE_BLACKBOARD("名称", sizeof(integer), &id变量, &ret);

/* 缓冲区（intra-partition，FIFO队列） */
CREATE_BUFFER("名称", sizeof(integer), 最大消息数, FIFO, &id变量, &ret);
```

### 任务创建（在 appMain 中调用）

```c
PROCESS_ATTRIBUTE_TYPE tattr;
/* 任务名：严格使用 tasks[].name 的值，如 "task31" */
strcpy(tattr.NAME, "task31");
tattr.ENTRY_POINT   = task31_job;   /* <name>_job 格式 */
tattr.BASE_PRIORITY = 2;            /* tasks[].priority 的值 */
tattr.PERIOD        = 25000000ll;   /* tasks[].period_ms × 1000000，直接写计算结果 */
tattr.STACK_SIZE    = 8192;
tattr.TIME_CAPACITY = 25000000ll;   /* 与 PERIOD 相同 */
tattr.DEADLINE      = SOFT;
CREATE_PROCESS(&tattr, &arinc_threads[N], &ret);
START(arinc_threads[N], &ret);
```

### 通信 API（在任务 while(1) 循环中调用）

```c
/* 采样端口读写 */
WRITE_SAMPLING_MESSAGE(id, (MESSAGE_ADDR_TYPE)&val, sizeof(integer), &ret); /* SOURCE */
READ_SAMPLING_MESSAGE(id, (MESSAGE_ADDR_TYPE)&val, &len, &validity, &ret); /* DEST */

/* 队列端口读写 */
SEND_QUEUING_MESSAGE(id, (MESSAGE_ADDR_TYPE)&val, sizeof(integer), timeout, &ret);    /* SOURCE */
RECEIVE_QUEUING_MESSAGE(id, timeout, (MESSAGE_ADDR_TYPE)&val, &len, &ret); /* DEST */

/* 黑板读写 */
DISPLAY_BLACKBOARD(id, (MESSAGE_ADDR_TYPE)&val, sizeof(integer), &ret);   /* 写 */
READ_BLACKBOARD(id, timeout, (MESSAGE_ADDR_TYPE)&val, &len, &ret);        /* 读 */

/* 缓冲区读写 */
WRITE_BUFFER(id, (MESSAGE_ADDR_TYPE)&val, sizeof(integer), timeout, &ret); /* 写 */
READ_BUFFER(id, timeout, (MESSAGE_ADDR_TYPE)&val, &len, &ret);             /* 读 */

/* 任务周期等待（每个任务 while(1) 末尾必须调用） */
PERIODIC_WAIT(&ret);
```

### 分区模式切换（appMain 最后一步）

```c
SET_PARTITION_MODE(NORMAL, &ret);
```

---

## 请按以下步骤推理，然后生成代码

**步骤 1：分析端口和通信资源**
- 列出所有需要创建的端口（类型、名称、方向）
- 列出所有黑板和缓冲区

**步骤 2：规划 appMain 的 CREATE 顺序**
- 通常顺序：先创建通信资源，在其依赖任务之前；最后 SET_PARTITION_MODE(NORMAL)
- 列出 CREATE 调用序列

**步骤 3：分析每个任务的通信行为**
- 对每个任务：它读哪些资源？写哪些资源？调用哪些子程序？
- 推断 activity.c 中的 READ/WRITE 操作序列
- **命名确认**：列出每个任务的函数名（必须为 `tasks[].name` + `_job`，如 `task31_job`）

**步骤 4：生成 11 个文件的完整代码**

请先完成步骤 1-3 的分析，再输出步骤 4 的代码。

## 代码规范

- **任务函数命名（严格）**：每个任务函数名必须为 `<name>_job`，其中 `<name>` 完全等于规格 JSON 中 `tasks[].name` 的值。例如 `"name": "task31"` → 函数签名为 `void *task31_job(void *arg)`。activity.h 声明、activity.c 实现、`tattr.ENTRY_POINT` 赋值、`strcpy(tattr.NAME, ...)` 字符串，均必须使用完全相同的名称。
- 全局变量前缀：`{{PARTITION_NAME}}_`
- 端口 ID 在 `main.c` 中声明为全局变量，在 `activity.c` 中用 `extern` 引用
- **不要**在 `while(1)` 循环内调用 `CREATE_*` 或 `GET_*_PORT_ID`
- 每次 APEX 调用后调用 `CHECK_CODE("调用描述", ret)`
- 周期单位为纳秒，**必须写计算结果**（如 25ms → `25000000ll`），不要写 `25 * 1000000ll`
- 所有头文件有 `#ifndef`/`#define`/`#endif` 保护
- `activity.c` 和 `main.c` 包含 `#include <os/pos/apex/apexLib.h>`
- `main.c` 末尾实现 `module_HM_callback` 和 `partition_HM_callback`

请逐一输出每个文件，格式为：
```
=== 文件名 ===
（完整代码）
```
