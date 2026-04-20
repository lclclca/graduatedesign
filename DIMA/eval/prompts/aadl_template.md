# AADL 提示词模板（AADL Input）— IMA2 系统

你是一名嵌入式航空软件工程师，熟悉 ARINC 653 标准、AADL 架构描述语言和 ACoreOS653 实时操作系统。

下面给出 IMA2 系统的 AADL 架构描述，包含线程定义和分区定义两个包。
请根据 AADL 模型，为指定的目标分区生成完整的 ARINC 653 C 代码。

## AADL 模型

### 线程定义包（DIMA2_threads）

```aadl
package DIMA2_threads
public

	with ARINC653;
	with Base_Types;

	thread TskA1
		features
			ctrl_in: in event data port Base_Types::Integer {Queue_Size => 4;
				ARINC653::Timeout => 5ms;
				ARINC653::Queueing_Discipline => FIFO;};
	end TskA1;

	thread implementation TskA1.impl
		properties
			Dispatch_Protocol     => Periodic;
			Period                => 50ms;
			Deadline              => 50ms;
			Priority              => 2;
			Stack_Size            => 8Kbyte;
			ARINC653::Time_Capacity  => 50ms;
			ARINC653::Deadline_Type  => soft;
	end TskA1.impl;

	thread TskB1
		features
			sensor_out: out data port Base_Types::Integer;
			bb_write:   out data port Base_Types::Integer;
	end TskB1;

	thread implementation TskB1.impl
		properties
			Dispatch_Protocol     => Periodic;
			Period                => 25ms;
			Deadline              => 25ms;
			Priority              => 2;
			Stack_Size            => 8Kbyte;
			ARINC653::Time_Capacity  => 25ms;
			ARINC653::Deadline_Type  => soft;
	end TskB1.impl;

	thread TskB2
		features
			cmd_in:  in event data port Base_Types::Integer {Queue_Size => 4;
				ARINC653::Timeout => 5ms;
				ARINC653::Queueing_Discipline => FIFO;};
			bb_read: in data port Base_Types::Integer;
	end TskB2;

	thread implementation TskB2.impl
		properties
			Dispatch_Protocol     => Periodic;
			Period                => 50ms;
			Deadline              => 50ms;
			Priority              => 3;
			Stack_Size            => 8Kbyte;
			ARINC653::Time_Capacity  => 50ms;
			ARINC653::Deadline_Type  => soft;
	end TskB2.impl;

	thread TskC1
		features
			data_in:  in data port Base_Types::Integer;
			bb_write: out data port Base_Types::Integer;
			buf_send: out event data port Base_Types::Integer {Queue_Size => 4;
				ARINC653::Timeout => 5ms;
				ARINC653::Queueing_Discipline => FIFO;};
	end TskC1;

	thread implementation TskC1.impl
		properties
			Dispatch_Protocol     => Periodic;
			Period                => 25ms;
			Deadline              => 25ms;
			Priority              => 2;
			Stack_Size            => 8Kbyte;
			ARINC653::Time_Capacity  => 25ms;
			ARINC653::Deadline_Type  => soft;
	end TskC1.impl;

	thread TskC2
		features
			bb_read:  in data port Base_Types::Integer;
			buf_recv: in event data port Base_Types::Integer {Queue_Size => 4;
				ARINC653::Timeout => 5ms;
				ARINC653::Queueing_Discipline => FIFO;};
	end TskC2;

	thread implementation TskC2.impl
		properties
			Dispatch_Protocol     => Periodic;
			Period                => 50ms;
			Deadline              => 50ms;
			Priority              => 3;
			Stack_Size            => 8Kbyte;
			ARINC653::Time_Capacity  => 50ms;
			ARINC653::Deadline_Type  => soft;
	end TskC2.impl;

	thread TskC3
		features
			cmd_out: out event data port Base_Types::Integer {Queue_Size => 4;
				ARINC653::Timeout => 5ms;
				ARINC653::Queueing_Discipline => FIFO;};
	end TskC3;

	thread implementation TskC3.impl
		properties
			Dispatch_Protocol     => Periodic;
			Period                => 100ms;
			Deadline              => 100ms;
			Priority              => 4;
			Stack_Size            => 8Kbyte;
			ARINC653::Time_Capacity  => 100ms;
			ARINC653::Deadline_Type  => soft;
	end TskC3.impl;

end DIMA2_threads;
```

### 分区定义包（DIMA2_partitions）

```aadl
package DIMA2_partitions
public

	with DIMA2_threads;
	with ARINC653;
	with Base_Types;

	process PA
		features
			ctrl_in: in event data port Base_Types::Integer {Queue_Size => 4;
				ARINC653::Timeout => 5ms;
				ARINC653::Queueing_Discipline => FIFO;};
	end PA;

	process implementation PA.impl
		subcomponents
			taskA1: thread DIMA2_threads::TskA1.impl;
		connections
			c1: port ctrl_in -> taskA1.ctrl_in;
		properties
			ARINC653::DAL => LEVEL_A;
	end PA.impl;

	process PB
		features
			sensor_out: out data port Base_Types::Integer {ARINC653::Sampling_Refresh_Period => 100ns;};
			cmd_in:     in event data port Base_Types::Integer {Queue_Size => 4;
				ARINC653::Timeout => 5ms;
				ARINC653::Queueing_Discipline => FIFO;};
	end PB;

	process implementation PB.impl
		subcomponents
			taskB1: thread DIMA2_threads::TskB1.impl;
			taskB2: thread DIMA2_threads::TskB2.impl;
		connections
			sampling_out: port taskB1.sensor_out -> sensor_out;
			queuing_in:   port cmd_in -> taskB2.cmd_in;
			data_link:    port taskB1.bb_write -> taskB2.bb_read;
		properties
			ARINC653::DAL => LEVEL_A;
	end PB.impl;

	process PC
		features
			data_in: in data port Base_Types::Integer {ARINC653::Sampling_Refresh_Period => 100ns;};
			cmd_out: out event data port Base_Types::Integer {Queue_Size => 4;
				ARINC653::Timeout => 5ms;
				ARINC653::Queueing_Discipline => FIFO;};
	end PC;

	process implementation PC.impl
		subcomponents
			taskC1: thread DIMA2_threads::TskC1.impl;
			taskC2: thread DIMA2_threads::TskC2.impl;
			taskC3: thread DIMA2_threads::TskC3.impl;
		connections
			sampling_in: port data_in -> taskC1.data_in;
			queuing_out: port taskC3.cmd_out -> cmd_out;
			data_link:   port taskC1.bb_write -> taskC2.bb_read;
			msg_queue:   port taskC1.buf_send -> taskC2.buf_recv;
		properties
			ARINC653::DAL => LEVEL_A;
	end PC.impl;

end DIMA2_partitions;
```

## 目标分区

<!-- 根据实验目标填写：PA 或 PB 或 PC -->
请为分区 **{{PARTITION_NAME}}** 生成代码。

## AADL 解读说明

在将 AADL 映射到 APEX C 代码时，遵循以下对应关系：

| AADL 元素 | APEX 映射 |
|-----------|-----------|
| `thread` 的 `Period` | `CREATE_PROCESS` 中 `tattr.PERIOD`（单位纳秒，ms × 1,000,000） |
| `thread` 的 `Priority` | `tattr.BASE_PRIORITY` |
| `thread` 的 `Stack_Size` | `tattr.STACK_SIZE`（8Kbyte = 8192） |
| `out data port`（分区级） | 采样端口 SOURCE，用 `CREATE_SAMPLING_PORT` + `WRITE_SAMPLING_MESSAGE` |
| `in data port`（分区级） | 采样端口 DESTINATION，用 `CREATE_SAMPLING_PORT` + `READ_SAMPLING_MESSAGE` |
| `out event data port`（分区级） | 队列端口 SOURCE，用 `CREATE_QUEUING_PORT` + `SEND_QUEUING_MESSAGE` |
| `in event data port`（分区级） | 队列端口 DESTINATION，用 `CREATE_QUEUING_PORT` + `RECEIVE_QUEUING_MESSAGE` |
| 线程间 `data port` 连接（分区内） | 黑板，用 `CREATE_BLACKBOARD` + `DISPLAY_BLACKBOARD` / `READ_BLACKBOARD` |
| 线程间 `event data port` 连接（分区内） | 缓冲区，用 `CREATE_BUFFER` + `SEND_BUFFER` / `RECEIVE_BUFFER` |
| `connections` 中的连接名 | 作为黑板/缓冲区的资源名称（加前缀 `bb_` 或 `buf_`） |
| `Queue_Size` 属性 | `CREATE_QUEUING_PORT` / `CREATE_BUFFER` 的 `MAX_NB_MESSAGE` 参数 |

## 要求

请生成以下 11 个文件的完整代码：

1. **deployment.h** — 使用 `#define` 定义部署常量（NB_THREADS、NB_SAMPLINGS、NB_QUEUEINGS、NB_BLACKBOARDS、NB_BUFFERS、STACKS_SIZE），常量名前缀为 `ACoreOS653_CONFIG_` 或 `ACoreOS653_NEEDS_`
2. **deployment.c** — 仅包含 `#include "deployment.h"`
3. **gtypes.h** — 定义 `typedef int integer;`
4. **gtypes.c** — 仅包含 `#include "gtypes.h"`
5. **globals.h** — 定义 `CHECK_CODE(msg, code)` 宏：NO_ERROR 时打印成功，否则打印真实错误码
6. **globals.c** — 仅包含 `#include "globals.h"`
7. **subprograms.h** — 空头文件
8. **subprograms.c** — 仅包含 `#include "subprograms.h"`
9. **activity.h** — 声明所有任务函数，签名为 `void *taskXX_job(void *arg)`
10. **activity.c** — 实现所有任务函数，每个任务有 `while(1)` 循环，末尾调用 `PERIODIC_WAIT`
11. **main.c** — 实现 `appMain(void)`：依次 CREATE 所有端口/黑板/缓冲区/进程，START 所有进程，最后调用 `SET_PARTITION_MODE(NORMAL)`；还需实现 `module_HM_callback` 和 `partition_HM_callback`

## 代码规范

- 任务函数命名：`subcomponents` 中的实例名即为任务名，函数签名为 `void *<任务名>_job(void *arg)`，例如 `taskA1` → `void *taskA1_job(void *arg)`
- 所有头文件必须有 `#ifndef`/`#define`/`#endif` 保护
- `activity.c` 和 `main.c` 必须包含 `#include <os/pos/apex/apexLib.h>`
- 任务周期单位为**纳秒**（ms × 1,000,000）
- 端口/黑板/缓冲区的 ID 变量在 main.c 中声明为全局变量，在 activity.c 中用 `extern` 引用
- 每次 APEX API 调用后用 `CHECK_CODE("API名称", ret)` 检查返回值
