你是一名嵌入式航空软件工程师，熟悉 ARINC 653 标准和 ACoreOS653 实时操作系统。

请根据以下 IMA（综合模块化航空电子）分区规格，生成完整的 ARINC 653 C 代码。
该分区运行在 ACoreOS653 操作系统上，使用 APEX 服务接口。

## 分区规格

```json
{
  "partition": "ps2",
  "module": "M1",
  "tasks": [
    {"name": "task21", "period_ms": 50,  "priority": 2},
    {"name": "task22", "period_ms": 50,  "priority": 3},
    {"name": "task23", "period_ms": 100, "priority": 4}
  ],
  "sampling_ports": [
    {"name": "pr2samplingin", "direction": "DESTINATION", "api_read": "READ_SAMPLING_MESSAGE"}
  ],
  "queuing_ports": [
    {"name": "pr2queueingout", "direction": "SOURCE", "max_nb": 30, "api_write": "SEND_QUEUING_MESSAGE"}
  ],
  "blackboards": ["bb_acc3", "bb_t2t3", "bb_t3t2"],
  "buffers":     ["buf_order", "buf_t2tot3", "buf_t3tot2"],
  "subprograms": ["commandboard_receiveinput_spg", "commandboard_printinfos_spg"],
  "deployment": {
    "nb_threads": 3, "nb_samplings": 1, "nb_queueings": 1,
    "nb_blackboards": 3, "nb_buffers": 3, "stacks_size": 24576
  }
}
## 要求

请生成以下 11 个文件的完整代码：

1. **deployment.h** — 使用 `#define` 定义部署常量（NB_THREADS、NB_SAMPLINGS、NB_QUEUEINGS、NB_BLACKBOARDS、NB_BUFFERS、STACKS_SIZE），常量名前缀为 `ACoreOS653_CONFIG_` 或 `ACoreOS653_NEEDS_`
2. **deployment.c** — 仅包含 `#include "deployment.h"`
3. **gtypes.h** — 定义 `typedef int integer;`（映射 AADL Base_Types::Integer）
4. **gtypes.c** — 仅包含 `#include "gtypes.h"`
5. **globals.h** — 定义 `CHECK_CODE(msg, code)` 宏：NO_ERROR 时打印成功，否则打印真实错误码
6. **globals.c** — 仅包含 `#include "globals.h"`
7. **subprograms.h** — 声明规格中列出的子程序函数（若无则为空头文件）
8. **subprograms.c** — 子程序桩实现（函数体为 TODO 注释）
9. **activity.h** — 声明所有 `taskXX_job(void)` 函数
10. **activity.c** — 实现所有任务函数，每个任务有 `while(1)` 循环，末尾调用 `PERIODIC_WAIT`
11. **main.c** — 实现 `appMain(void)`：依次 CREATE 所有端口/黑板/缓冲区/进程，最后调用 `SET_PARTITION_MODE(NORMAL)`；还需实现 `module_HM_callback` 和 `partition_HM_callback`

## 代码规范

- **任务函数命名（严格）**：每个任务函数名必须为 `<name>_job`，其中 `<name>` 完全等于规格 JSON 中 `tasks[].name` 的值。例如 `"name": "task31"` → 函数签名为 `void *task31_job(void *arg)`。activity.h 中的声明、activity.c 中的实现、CREATE_PROCESS 的 `tattr.ENTRY_POINT` 赋值，以及 `strcpy(tattr.NAME, ...)` 的字符串，均必须使用完全相同的名称。
- 所有头文件必须有 `#ifndef`/`#define`/`#endif` 头文件保护
- `activity.c` 和 `main.c` 必须包含 `#include <os/pos/apex/apexLib.h>`
- 任务周期单位为**纳秒**（ms × 1,000,000）
- 端口/黑板/缓冲区的 ID 变量声明为全局变量（在 main.c 中），在 activity.c 中用 `extern` 引用
- **不要**在任务的 `while(1)` 循环内调用 `GET_QUEUING_PORT_ID`，端口 ID 应在 `appMain` 中创建时获得
- 采样端口（SOURCE）用 `WRITE_SAMPLING_MESSAGE`，采样端口（DESTINATION）用 `READ_SAMPLING_MESSAGE`
- 队列端口（SOURCE）用 `SEND_QUEUING_MESSAGE`，队列端口（DESTINATION）用 `RECEIVE_QUEUING_MESSAGE`
- 每次 APEX API 调用后都要调用 `CHECK_CODE("API名称", ret)` 检查返回值

请逐一输出每个文件的完整代码，格式为：
```
=== 文件名 ===
（完整代码）
```
