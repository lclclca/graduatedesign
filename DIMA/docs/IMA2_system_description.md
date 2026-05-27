# IMA2 系统说明

## 1. 系统概述

IMA2 是一个为**提示词工程 Few-Shot 实验**专门设计的新 IMA 系统，
包含 1 个模块（MA）、3 个分区（PA / PB / PC）和 6 个周期性任务。

**设计目的**：在 Few-Shot 实验中，以 DIMA 系统的现有分区（如 ps3）作为**示例**，
以 IMA2 的分区作为**测试目标**，评估大语言模型在见过一个示例后能否正确生成
新系统各分区的 ARINC 653 C 代码。

**设计原则**：三个分区覆盖由简到繁的三种复杂度，
使 Few-Shot 实验能够观察示例对不同复杂度任务的迁移能力。

---

## 2. 模块设计（MA）

IMA2 采用**单模块架构**，所有分区运行在同一个 ARINC 653 模块 MA 中。
与 DIMA 的三模块分布式架构不同，IMA2 不涉及跨模块光纤通道通信，
分区间通信（采样端口/队列端口）通过模块内部连接实现。

```
┌──────────────────────────────────────────────────┐
│                  模块 MA                          │
│                                                  │
│  ┌──────────┐   ┌──────────┐   ┌──────────────┐  │
│  │   PA     │   │   PB     │   │     PC       │  │
│  │ 1 task   │   │ 2 tasks  │   │   3 tasks    │  │
│  │ 1 队列入 │   │1采+1队列 │   │ 1采+1队列+   │  │
│  │          │   │+ 1黑板   │   │ 1黑板+1缓冲  │  │
│  └──────────┘   └──────────┘   └──────────────┘  │
└──────────────────────────────────────────────────┘
```

---

## 3. 分区设计

### 3.1 PA——极简分区（1 任务 + 1 队列入端口）

**功能**：系统中最简单的分区。仅有 1 个周期任务，通过 1 个队列接收端口接收外部控制指令，
无分区内共享资源，无采样端口。是验证模型是否掌握 ARINC 653 最基本代码框架的基准用例。

**任务列表**

| 任务名 | 周期 | 优先级 | 主要职责 |
|-------|------|--------|---------|
| taskA1 | 50ms | 2 | 接收队列消息（ctrl_in），执行控制逻辑，等待下一周期 |

**端口与内部资源**

| 类型 | 名称 | 方向 | 参数 | 说明 |
|------|------|------|------|------|
| 队列端口 | ctrl_in | DESTINATION（入） | max_nb=4, FIFO | 接收外部控制指令 |
| 黑板/缓冲区 | — | — | — | 无分区内共享资源 |

**复杂度定位**：⭐（最简）——仅需掌握 `CREATE_QUEUING_PORT` + `CREATE_PROCESS` + `RECEIVE_QUEUING_MESSAGE`

---

### 3.2 PB——中等分区（2 任务 + 1 采样出 + 1 队列入 + 1 黑板）

**功能**：中等复杂度分区，引入两种不同类型的分区间通信（采样输出 + 队列输入）
和一种分区内通信（黑板）。task B1 负责周期性数据采集并向外广播采样消息，
同时将中间结果写入黑板；taskB2 接收外部队列指令并读取黑板数据进行综合处理。

**任务列表**

| 任务名 | 周期 | 优先级 | 主要职责 |
|-------|------|--------|---------|
| taskB1 | 25ms | 2 | 写采样端口（sensor_out），写黑板（bb_data_link）；高频率数据采集 |
| taskB2 | 50ms | 3 | 接收队列消息（cmd_in），读黑板（bb_data_link）；综合处理 |

**端口与内部资源**

| 类型 | 名称 | 方向 | 参数 | 说明 |
|------|------|------|------|------|
| 采样端口 | sensor_out | SOURCE（出） | refresh=100ns | 向外广播传感器数据 |
| 队列端口 | cmd_in | DESTINATION（入） | max_nb=4, FIFO | 接收外部命令 |
| 黑板 | bb_data_link | taskB1→taskB2 | — | 分区内最新值共享（taskB1 写，taskB2 读） |
| 缓冲区 | — | — | — | 无缓冲区 |

**复杂度定位**：⭐⭐（中等）——需同时掌握 `CREATE_SAMPLING_PORT`、`CREATE_QUEUING_PORT`、`CREATE_BLACKBOARD` 及对应读写 API

---

### 3.3 PC——复杂分区（3 任务 + 1 采样入 + 1 队列出 + 1 黑板 + 1 缓冲区）

**功能**：最复杂的分区，覆盖 ARINC 653 全部四类通信资源。taskC1 以最高频率读取采样数据，
经过处理后通过黑板和缓冲区向 taskC2 传递结果；taskC2 消费黑板（最新值）和缓冲区（FIFO队列）；
taskC3 负责低频率汇总并向外部发送队列消息。

**任务列表**

| 任务名 | 周期 | 优先级 | 主要职责 |
|-------|------|--------|---------|
| taskC1 | 25ms | 2 | 读采样端口（data_in），写黑板（bb_data_link），写缓冲区（buf_msg_queue）；数据接入 |
| taskC2 | 50ms | 3 | 读黑板（bb_data_link），读缓冲区（buf_msg_queue）；数据消费与处理 |
| taskC3 | 100ms | 4 | 发送队列消息（cmd_out）；结果输出 |

**端口与内部资源**

| 类型 | 名称 | 方向 | 参数 | 说明 |
|------|------|------|------|------|
| 采样端口 | data_in | DESTINATION（入） | refresh=100ns | 接收外部传感器数据 |
| 队列端口 | cmd_out | SOURCE（出） | max_nb=4, FIFO | 向外部发送处理结果 |
| 黑板 | bb_data_link | taskC1→taskC2 | — | 分区内最新值共享（taskC1 写，taskC2 读） |
| 缓冲区 | buf_msg_queue | taskC1→taskC2 | max_nb=4 | 分区内 FIFO 队列（taskC1 写，taskC2 读） |

**复杂度定位**：⭐⭐⭐（最复杂）——需掌握全部 8 种 ARINC 653 资源初始化及读写 API：
`CREATE_SAMPLING_PORT` / `READ_SAMPLING_MESSAGE`、
`CREATE_QUEUING_PORT` / `SEND_QUEUING_MESSAGE`、
`CREATE_BLACKBOARD` / `DISPLAY_BLACKBOARD` / `READ_BLACKBOARD`、
`CREATE_BUFFER` / `SEND_BUFFER` / `RECEIVE_BUFFER`

---

## 4. C 代码文件结构

每个分区生成 11 个 C/H 文件，存放在 `DIMA/C_Code_Gen/IMA2/<分区名>/` 下：

```
DIMA/C_Code_Gen/IMA2/
├── pa/                ← 分区 PA 的参考代码
│   ├── deployment.h
│   ├── deployment.c
│   ├── globals.h / globals.c
│   ├── gtypes.h / gtypes.c
│   ├── subprograms.h / subprograms.c
│   ├── activity.h / activity.c
│   ├── main.c
│   └── pa.json        ← 自动生成的规格 JSON
├── pb/                ← 分区 PB 的参考代码
│   └── ...（同结构）
└── pc/                ← 分区 PC 的参考代码
    └── ...（同结构）
```

规格 JSON（`pa.json`、`pb.json`、`pc.json`）已同步复制至 `DIMA/eval/specs/`，
供评估脚本（`evaluate.py`）使用。

### 各文件职责（与 DIMA 系统完全一致）

| 文件 | 职责 |
|------|------|
| `deployment.h` | 编译期宏：`NB_THREADS`、`NB_SAMPLINGS`、`NB_QUEUEINGS`、`NB_BLACKBOARDS`、`NB_BUFFERS`、`STACKS_SIZE` |
| `deployment.c` | 仅 `#include "deployment.h"` |
| `globals.h/c` | ARINC 653 资源 ID 全局变量声明与定义 |
| `gtypes.h/c` | 分区内通信数据类型定义 |
| `subprograms.h/c` | 子程序接口声明与实现（本系统无子程序调用） |
| `activity.h/c` | **核心**：`appMain()`（资源初始化 + 任务启动）和所有 `taskXX_job()` 函数 |
| `main.c` | 入口：调用 `appMain()`，再调用 `SET_PARTITION_MODE(NORMAL, &ret)` |

### `activity.c` 关键代码模式示例（以 PB 为例）

**appMain() 资源初始化**

```c
void appMain(void) {
    RETURN_CODE_TYPE ret;

    /* 采样端口（SOURCE） */
    CREATE_SAMPLING_PORT("sensor_out", sizeof(int), SOURCE, 100LL, &pb_sensor_out_id, &ret);

    /* 队列端口（DESTINATION） */
    CREATE_QUEUING_PORT("cmd_in", sizeof(int), 4, DESTINATION, FIFO, &pb_cmd_in_id, &ret);

    /* 黑板 */
    CREATE_BLACKBOARD("bb_data_link", sizeof(int), &pb_bb_data_link_id, &ret);

    /* 任务 taskB1 */
    PROCESS_ATTRIBUTE_TYPE tattr;
    memset(&tattr, 0, sizeof(tattr));
    strcpy(tattr.NAME, "taskB1");
    tattr.ENTRY_POINT   = taskB1_job;
    tattr.BASE_PRIORITY = 2;
    tattr.PERIOD        = 25000000ll;   /* 25ms */
    tattr.TIME_CAPACITY = 25000000ll;
    tattr.STACK_SIZE    = 8192;
    CREATE_PROCESS(&tattr, &pb_taskB1_id, &ret);
    START(pb_taskB1_id, &ret);

    /* 任务 taskB2（同上，period=50ms） */
    ...
    SET_PARTITION_MODE(NORMAL, &ret);
}
```

**任务函数**

```c
void taskB1_job(void) {
    int msg = 0;
    RETURN_CODE_TYPE ret;
    while (1) {
        msg++;
        WRITE_SAMPLING_MESSAGE(pb_sensor_out_id, &msg, sizeof(int), &ret);
        DISPLAY_BLACKBOARD(pb_bb_data_link_id, &msg, sizeof(int), &ret);
        PERIODIC_WAIT(&ret);
    }
}

void taskB2_job(void) {
    int msg;
    MESSAGE_SIZE_TYPE len;
    RETURN_CODE_TYPE ret;
    while (1) {
        RECEIVE_QUEUING_MESSAGE(pb_cmd_in_id, 5000000LL, &msg, &len, &ret);
        READ_BLACKBOARD(pb_bb_data_link_id, 0LL, &msg, &len, &ret);
        PERIODIC_WAIT(&ret);
    }
}
```

---

## 5. 分区–资源–C 代码对应速查表

| 分区 | AADL 进程 | C 代码目录 | 任务数 | 采样 | 队列 | 黑板 | 缓冲 | 复杂度 |
|------|----------|-----------|-------|------|------|------|------|-------|
| PA | PA.impl | `IMA2/pa/` | 1 | 0 | 1入 | 0 | 0 | ⭐ |
| PB | PB.impl | `IMA2/pb/` | 2 | 1出 | 1入 | 1 | 0 | ⭐⭐ |
| PC | PC.impl | `IMA2/pc/` | 3 | 1入 | 1出 | 1 | 1 | ⭐⭐⭐ |

---

## 6. 与 DIMA 系统的对比

| 维度 | DIMA 系统 | IMA2 系统 |
|------|----------|----------|
| 模块数 | 3（M1/M2/M3） | 1（MA） |
| 分区数 | 5（ps1–ps5） | 3（PA/PB/PC） |
| 总任务数 | 15 | 6 |
| 最大任务/分区 | 5（ps4） | 3（PC） |
| 跨模块通信 | 是（光纤通道 FC） | 否 |
| AADL 来源 | `DIMA_partitions.aadl` | `DIMA2_partitions.aadl` |
| 生成工具 | IMA2C（Eclipse）+ aadl2c.py | aadl2c.py + --spec-json |
| 用途 | 提示词实验的**评估对象**（ZS/CoT/Combined） | 提示词实验的**FS 测试目标** |
| Few-Shot 角色 | 提供示例（如 ps3 作为 in-context example） | 被预测对象（AI 生成 PA/PB/PC 代码） |

---

## 7. Few-Shot 实验使用方式

```
提示词结构示意：

【系统背景】ARINC 653 IMA 系统代码生成任务说明

【示例分区（来自 DIMA 系统）】
  规格：DIMA/eval/specs/ps3.json
  参考代码：DIMA/C_Code_Gen/m2_impl_instance/ps3/activity.c 等

【目标分区（来自 IMA2 系统）】
  规格：DIMA/eval/specs/pa.json（或 pb.json / pc.json）
  要求：生成对应的 11 个 C 文件

【评估】
  将 AI 输出填入 DIMA/eval/generated/fewshot/pa/（或 pb/ / pc/）
  运行 python3 DIMA/eval/metrics/evaluate.py --exp fewshot --part pa
```
