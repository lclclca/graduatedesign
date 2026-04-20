# 自然语言提示词模板（Natural Language）— IMA2 系统

你是一名嵌入式航空软件工程师，熟悉 ARINC 653 标准和 ACoreOS653 实时操作系统。

请根据以下对 IMA 分区的自然语言描述，生成完整的 ARINC 653 C 代码。
该分区运行在 ACoreOS653 操作系统上，使用 APEX 服务接口。

## 分区描述

<!-- 根据目标分区选择下方对应的描述块，删除其余两个 -->

<!-- ========== PA ========== -->
本分区名称为 PA，属于 IMA 系统 MA 模块。包含 1 个周期性任务，任务名称为 taskA1，周期为 50 毫秒，优先级为 2，栈大小 8192 字节。分区有 1 个队列通信端口，名称为 ctrl_in，方向为接收（从外部接收消息），最大队列深度为 4 条消息，排队策略为先进先出。该分区没有采样端口、没有黑板资源、没有缓冲区资源、没有子程序。

<!-- ========== PB ========== -->
本分区名称为 PB，属于 IMA 系统 MA 模块。包含 2 个周期性任务：taskB1，周期 25 毫秒，优先级 2，栈大小 8192 字节；taskB2，周期 50 毫秒，优先级 3，栈大小 8192 字节。分区有以下通信资源：1 个采样端口 sensor_out，方向为发送（向外部写出数据），刷新周期 100 纳秒；1 个队列端口 cmd_in，方向为接收，最大队列深度 4，先进先出；1 个黑板资源，名称为 bb_data_link，用于分区内任务间数据共享（taskB1 写入，taskB2 读取）。该分区没有缓冲区资源、没有子程序。

<!-- ========== PC ========== -->
本分区名称为 PC，属于 IMA 系统 MA 模块。包含 3 个周期性任务：taskC1，周期 25 毫秒，优先级 2，栈大小 8192 字节；taskC2，周期 50 毫秒，优先级 3，栈大小 8192 字节；taskC3，周期 100 毫秒，优先级 4，栈大小 8192 字节。分区有以下通信资源：1 个采样端口 data_in，方向为接收（从外部读入数据），刷新周期 100 纳秒；1 个队列端口 cmd_out，方向为发送（向外部发出消息），最大队列深度 4，先进先出；1 个黑板资源，名称为 bb_data_link（taskC1 写入，taskC2 读取）；1 个缓冲区资源，名称为 buf_msg_queue，最大消息数 4（taskC1 发送，taskC2 接收）。该分区没有子程序。

## 要求

请生成以下 11 个文件的完整代码：

1. **deployment.h** — 使用 `#define` 定义部署常量（NB_THREADS、NB_SAMPLINGS、NB_QUEUEINGS、NB_BLACKBOARDS、NB_BUFFERS、STACKS_SIZE），常量名前缀为 `ACoreOS653_CONFIG_` 或 `ACoreOS653_NEEDS_`
2. **deployment.c** — 仅包含 `#include "deployment.h"`
3. **gtypes.h** — 定义 `typedef int integer;`
4. **gtypes.c** — 仅包含 `#include "gtypes.h"`
5. **globals.h** — 定义 `CHECK_CODE(msg, code)` 宏：NO_ERROR 时打印成功，否则打印真实错误码
6. **globals.c** — 仅包含 `#include "globals.h"`
7. **subprograms.h** — 空头文件（本分区无子程序）
8. **subprograms.c** — 仅包含 `#include "subprograms.h"`
9. **activity.h** — 声明所有任务函数，签名为 `void *taskXX_job(void *arg)`
10. **activity.c** — 实现所有任务函数，每个任务有 `while(1)` 循环，在循环末尾调用 `PERIODIC_WAIT`
11. **main.c** — 实现 `appMain(void)`：依次 CREATE 所有端口/黑板/缓冲区/进程，START 所有进程，最后调用 `SET_PARTITION_MODE(NORMAL)`；还需实现 `module_HM_callback` 和 `partition_HM_callback`

## 代码规范

- 任务函数命名：函数名为 `<任务名>_job`，例如任务名 taskA1 → 函数签名 `void *taskA1_job(void *arg)`
- 所有头文件必须有 `#ifndef`/`#define`/`#endif` 保护
- `activity.c` 和 `main.c` 必须包含 `#include <os/pos/apex/apexLib.h>`
- 任务周期在代码中的单位为**纳秒**（毫秒 × 1,000,000），例如 50ms 写作 50000000
- 端口/黑板/缓冲区的 ID 变量在 main.c 中声明为全局变量，在 activity.c 中用 `extern` 引用
- 采样端口 SOURCE 方向在任务中调用 `WRITE_SAMPLING_MESSAGE`，DESTINATION 方向调用 `READ_SAMPLING_MESSAGE`
- 队列端口 SOURCE 方向在任务中调用 `SEND_QUEUING_MESSAGE`，DESTINATION 方向调用 `RECEIVE_QUEUING_MESSAGE`
- 黑板写入调用 `DISPLAY_BLACKBOARD`，读取调用 `READ_BLACKBOARD`
- 缓冲区发送调用 `SEND_BUFFER`，接收调用 `RECEIVE_BUFFER`
- 每次 APEX API 调用后用 `CHECK_CODE("API名称", ret)` 检查返回值
