# 自然语言 → AADL 转换提示词模板

你是一名航空系统架构师，精通 AADL（架构分析与设计语言）和 ARINC 653 标准。

请将下方的自然语言分区描述转换为格式正确的 AADL 模型。
输出必须包含两个 package：线程定义包（`_threads`）和分区定义包（`_partitions`）。

---

## 示例：PA 分区

**输入描述：**
分区名称为 PA，包含 1 个周期性任务 taskA1，周期 50ms，优先级 2，栈大小 8192 字节。
有 1 个队列端口 ctrl_in，方向为接收（DESTINATION），最大队列深度 4，FIFO 排队。
无采样端口、无黑板、无缓冲区。

**输出 AADL：**

```aadl
package PA_threads
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

end PA_threads;
```

```aadl
package PA_partitions
public

	with PA_threads;
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
			taskA1: thread PA_threads::TskA1.impl;
		connections
			c1: port ctrl_in -> taskA1.ctrl_in;
		properties
			ARINC653::DAL => LEVEL_A;
	end PA.impl;

end PA_partitions;
```

---

## 映射规则

在将自然语言描述转换为 AADL 时，遵循以下对应关系：

| 自然语言描述 | AADL 写法 |
|-------------|-----------|
| 任务名 taskXX，周期 Nms，优先级 P | `thread TskXX` + `thread implementation TskXX.impl`，`Period => Nms; Priority => P;` |
| 栈大小 S 字节 | `Stack_Size => SKbyte;`（8192 字节 = 8Kbyte） |
| 采样端口 NAME，方向发送（SOURCE） | process features: `NAME: out data port Base_Types::Integer {ARINC653::Sampling_Refresh_Period => 100ns;};` |
| 采样端口 NAME，方向接收（DEST） | process features: `NAME: in data port Base_Types::Integer {ARINC653::Sampling_Refresh_Period => 100ns;};` |
| 队列端口 NAME，方向发送（SOURCE），深度 N | process features: `NAME: out event data port Base_Types::Integer {Queue_Size => N; ARINC653::Timeout => 5ms; ARINC653::Queueing_Discipline => FIFO;};` |
| 队列端口 NAME，方向接收（DEST），深度 N | process features: `NAME: in event data port Base_Types::Integer {Queue_Size => N; ARINC653::Timeout => 5ms; ARINC653::Queueing_Discipline => FIFO;};` |
| 黑板 NAME，taskA 写入，taskB 读取 | 线程 taskA 有 `bb_write: out data port`，线程 taskB 有 `bb_read: in data port`；connections 中：`NAME: port taskA.bb_write -> taskB.bb_read;` |
| 缓冲区 NAME，taskA 发送，taskB 接收 | 线程 taskA 有 `buf_send: out event data port {Queue_Size => N; ...}`，线程 taskB 有 `buf_recv: in event data port {Queue_Size => N; ...}`；connections 中：`NAME: port taskA.buf_send -> taskB.buf_recv;` |

---

## 格式约束（必须严格遵守）

以下约束是为了确保生成的 AADL 能被自动解析工具正确处理：

1. **Package 命名**：线程包必须命名为 `<分区名>_threads`，分区包必须命名为 `<分区名>_partitions`（如分区名 PB → `PB_threads` 和 `PB_partitions`）
2. **线程命名**：thread 类型名以 `Tsk` 开头（如 `TskB1`），implementation 名加 `.impl` 后缀
3. **线程实例名**：process implementation 中 subcomponents 里的实例名即为任务名（如 `taskB1: thread PB_threads::TskB1.impl`）
4. **必需属性**：每个 thread implementation 必须包含 `Dispatch_Protocol`、`Period`、`Deadline`、`Priority`、`Stack_Size`、`ARINC653::Time_Capacity`、`ARINC653::Deadline_Type`
5. **Deadline = Period**：`Deadline` 值与 `Period` 相同；`ARINC653::Time_Capacity` 同样等于 `Period`
6. **分区内通信**：黑板和缓冲区不在 process features 中声明，只通过线程间端口连接（connections）表达
7. **分区间端口**：只有跨分区通信的端口才在 process features 中声明

---

## 目标分区描述

{{PARTITION_DESCRIPTION}}

## 输出要求

请输出两个完整的 AADL package，格式与上方示例完全一致：
1. `<分区名>_threads` package：包含所有线程的声明和 implementation
2. `<分区名>_partitions` package：包含 process 声明和 process implementation

只输出 AADL 代码，不要输出其他解释文字。
