# DIMA 系统说明

## 1. 系统概述

DIMA（Distributed IMA，分布式综合模块化航空电子系统）是一个典型的 ARINC 653 IMA 系统，
由 3 个模块（M1/M2/M3）、5 个分区（ps1–ps5）和 15 个周期性任务组成。
各模块通过光纤通道（Fibre Channel）交换机互联，实现跨模块数据交换。

整体架构来源于 AADL 模型文件（`DIMA/Sys/`），
由 IMA2C 工具链和 `aadl2c.py` 脚本生成对应的 ARINC 653 C 代码。

---

## 2. 模块设计（M1 / M2 / M3）

### 拓扑结构

```
        ┌───────────────────────────────────────────────────────┐
        │           Fibre Channel Switch1 / Switch2             │
        └───────┬───────────────────┬───────────────────┬───────┘
                │                   │                   │
         ┌──────▼──────┐    ┌───────▼──────┐    ┌──────▼──────┐
         │  M1 (es1)   │    │  M2 (es2)    │    │  M3 (es3)   │
         │  ps1  ps2   │    │  ps3  ps5    │    │    ps4      │
         └─────────────┘    └──────────────┘    └─────────────┘
```

### 跨模块通信（采样/队列端口）

| 发送方 | 接收方 | 端口类型 | 说明 |
|--------|--------|---------|------|
| M1.ps1 → | M2 | 采样（Sampling） | `pr1samplingout` → M2 `v1_in` |
| M1.ps1 → | M3 | 采样（Sampling） | `pr1samplingout` → M3 `V1_in` |
| M2.ps2 → | M3 | 采样（Sampling） | `pr2queueingout` → M3 `V4_in` |
| M3.ps4 → | M2 | 队列（Queuing） | M3 `V3_out` → M2 `v3_in` |

### 模块功能定位

| 模块 | 嵌入式系统 | 包含分区 | 功能定位 |
|------|-----------|---------|---------|
| **M1** | es1 | ps1、ps2 | **指令处理模块**：接收外部指令，执行主控逻辑；ps1 对外输出采样数据，ps2 接收采样并向外发送队列消息 |
| **M2** | es2 | ps3、ps5 | **任务分发模块**：接收 M1 的指令并转发给下游分区（ps3/ps5 均接收队列消息执行任务） |
| **M3** | es3 | ps4 | **执行管理模块**：汇聚来自 M1 和 M2 的数据，完成最终计算执行，并将结果返回 M2 |

每个模块运行一个 **ACoreOS653 ARINC 653 内核**（module1/module2/module3），
主帧周期均为 **25ms**，采用 **RMS（速率单调调度）**策略。

---

## 3. 分区设计

### 3.1 ps1（模块 M1，进程 P1）——主控指令分区

**功能**：系统的核心指令处理分区，负责接收外部输入、驱动分区间采样通信，同时通过黑板和缓冲区管理分区内多任务数据共享。

**任务列表**

| 任务名 | 周期 | 优先级 | 主要职责 |
|-------|------|--------|---------|
| task11 | 25ms | 2 | 读取缓冲区（buf_sensor），写两个黑板（bb_acc1/bb_acc2），向外发送采样消息（pr1samplingout） |
| task12 | 50ms | 3 | 读两个黑板，写缓冲区（buf_sensor），通过受保护黑板与 task13 共享数据 |
| task13 | 50ms | 4 | 通过受保护黑板（bb_sem）与 task12 交互，向 task14 发送缓冲区与黑板消息 |
| task14 | 50ms | 5 | 接收分区间队列消息（pr1queueingin），与 task13 互发缓冲区和黑板消息，调用计算子程序 |

**端口与内部资源**

| 类型 | 名称 | 方向 | 说明 |
|------|------|------|------|
| 采样端口 | pr1samplingout | SOURCE（出） | 向 M2/M3 广播传感器数据 |
| 队列端口 | pr1queueingin | DESTINATION（入） | 接收外部指令（max 30） |
| 黑板 × 5 | bb_acc1, bb_acc2, bb_sem, bb_t3t4, bb_t4t3 | 分区内 | 任务间最新值共享 |
| 缓冲区 × 3 | buf_sensor, buf_t3tot4, buf_t4tot3 | 分区内 | 任务间 FIFO 队列通信 |

---

### 3.2 ps2（模块 M1，进程 P2）——数据转发分区

**功能**：读取 M1 内部或外部采样数据，经过计算后向外部（M3）发送队列消息；分区内部维护黑板与缓冲区完成任务协作。

**任务列表**

| 任务名 | 周期 | 优先级 | 主要职责 |
|-------|------|--------|---------|
| task21 | 50ms | 2 | 接收采样消息（pr2samplingin），写黑板（bb_acc3） |
| task22 | 50ms | 3 | 读黑板（bb_acc3），写缓冲区（buf_order），与 task23 互发消息 |
| task23 | 100ms | 4 | 接收来自 task22 的缓冲区消息，向外发送队列消息（pr2queueingout） |

**端口与内部资源**

| 类型 | 名称 | 方向 | 说明 |
|------|------|------|------|
| 采样端口 | pr2samplingin | DESTINATION（入） | 接收来自 M1 广播的采样数据 |
| 队列端口 | pr2queueingout | SOURCE（出） | 向 M3 发送处理结果（max 30） |
| 黑板 × 3 | bb_acc3, bb_t2t3, bb_t3t2 | 分区内 | 任务间最新值共享 |
| 缓冲区 × 3 | buf_order, buf_t2tot3, buf_t3tot2 | 分区内 | 任务间 FIFO 队列通信 |

---

### 3.3 ps3（模块 M2，进程 P3）——指令接收分区

**功能**：纯队列接收分区，接收来自 M1 的两路外部队列指令，分别由三个任务处理，无分区内共享资源，结构最为简洁。

**任务列表**

| 任务名 | 周期 | 优先级 | 主要职责 |
|-------|------|--------|---------|
| task31 | 25ms | 2 | 接收队列消息（order_in），执行快速响应逻辑 |
| task32 | 50ms | 3 | 接收队列消息（order_in），执行中频率处理 |
| task33 | 50ms | 4 | 接收队列消息（queueingin），执行中频率处理 |

**端口与内部资源**

| 类型 | 名称 | 方向 | 说明 |
|------|------|------|------|
| 队列端口 | order_in | DESTINATION（入） | 接收 M1 广播指令（max 10） |
| 队列端口 | queueingin | DESTINATION（入） | 接收 M1 第二路指令（max 4） |
| 黑板/缓冲区 | — | — | 无分区内共享资源 |

---

### 3.4 ps4（模块 M3，进程 P4）——综合执行分区

**功能**：系统任务数最多的分区（5 个任务），汇聚来自 M1 和 M2 的多路数据，完成综合计算与执行，并将结果返回 M2；覆盖 25ms～200ms 多种周期任务。

**任务列表**

| 任务名 | 周期 | 优先级 | 主要职责 |
|-------|------|--------|---------|
| task41 | 25ms | 2 | 高频率基础周期任务 |
| task42 | 50ms | 3 | 接收队列消息（order_in），转发至队列出端口（queueingout） |
| task43 | 50ms | 4 | 接收队列消息（queueingin），执行中频率处理 |
| task44 | 100ms | 5 | 低频率数据汇总任务 |
| task45 | 200ms | 6 | 最低频率后台维护任务 |

**端口与内部资源**

| 类型 | 名称 | 方向 | 说明 |
|------|------|------|------|
| 队列端口 | order_in | DESTINATION（入） | 接收 M1 指令（max 1） |
| 队列端口 | queueingout | SOURCE（出） | 向 M2 发送执行结果（max 4） |
| 队列端口 | queueingin | DESTINATION（入） | 接收 M2 的第二路数据（max 4） |
| 黑板/缓冲区 | — | — | 无分区内共享资源 |

---

### 3.5 ps5（模块 M2，进程 P5）——结果输出分区

**功能**：接收来自 M1 的广播指令（order_in），经三个不同周期的任务处理后，向外发送队列消息（queueingout），作为系统输出端点。

**任务列表**

| 任务名 | 周期 | 优先级 | 主要职责 |
|-------|------|--------|---------|
| task51 | 50ms | 1 | 最高优先级，接收队列消息（order_in） |
| task52 | 50ms | 2 | 接收队列消息（order_in），中频率处理 |
| task53 | 200ms | 3 | 低频率汇总，向外发送队列消息（queueingout） |

**端口与内部资源**

| 类型 | 名称 | 方向 | 说明 |
|------|------|------|------|
| 队列端口 | order_in | DESTINATION（入） | 接收 M1 广播指令（max 4） |
| 队列端口 | queueingout | SOURCE（出） | 向外发送处理结果（max 4） |
| 黑板/缓冲区 | — | — | 无分区内共享资源 |

---

## 4. C 代码文件结构

每个分区生成 11 个 C/H 文件，存放在 `DIMA/C_Code_Gen/<模块实例>/<分区名>/` 下：

```
DIMA/C_Code_Gen/
├── m1_impl_instance/
│   ├── ps1/   ← 分区 P1 的代码
│   └── ps2/   ← 分区 P2 的代码
├── m2_impl_instance/
│   ├── ps3/   ← 分区 P3 的代码
│   └── ps5/   ← 分区 P5 的代码
└── m3_impl_instance/
    └── ps4/   ← 分区 P4 的代码
```

### 各文件职责

| 文件 | 职责 |
|------|------|
| `deployment.h` | 编译期常量宏：`NB_THREADS`、`NB_SAMPLINGS`、`NB_QUEUEINGS`、`NB_BLACKBOARDS`、`NB_BUFFERS`、`STACKS_SIZE` 等；由 `deployment.c` 仅 include，无独立符号 |
| `deployment.c` | 仅 `#include "deployment.h"`，供构建系统生成空目标文件 |
| `globals.h` | 声明分区全局 ARINC 653 资源 ID 变量：`SAMPLING_PORT_ID_TYPE`、`QUEUING_PORT_ID_TYPE`、`BLACKBOARD_ID_TYPE`、`BUFFER_ID_TYPE`、`PROCESS_ID_TYPE` |
| `globals.c` | 定义上述全局变量 |
| `gtypes.h` | 声明分区内通信数据类型（整数/结构体），供各任务函数共用 |
| `gtypes.c` | 定义数据类型（通常为空或含简单类型定义） |
| `subprograms.h` | 声明子程序接口（对应 AADL calls 中引用的 subprogram，如 `commandboard_receiveinput_spg`） |
| `subprograms.c` | 实现子程序函数体（用户业务逻辑填写处） |
| `activity.h` | 声明所有任务函数：`void taskXX_job(void)`，以及分区初始化函数 `void appMain(void)` |
| `activity.c` | **核心文件**：实现 `appMain()`（调用 APEX API 初始化所有资源并启动任务）和所有 `taskXX_job()` 函数（执行周期性通信逻辑） |
| `main.c` | 程序入口：调用 `appMain()`，随后调用 `SET_PARTITION_MODE(NORMAL, &ret)` 进入正常运行模式 |

### `activity.c` 中的关键 APEX API 调用模式

**资源初始化（在 `appMain()` 中）**

```c
/* 创建采样端口 */
CREATE_SAMPLING_PORT("pr1samplingout", MSG_SIZE, SOURCE, REFRESH_PERIOD, &port_id, &ret);

/* 创建队列端口 */
CREATE_QUEUING_PORT("pr1queueingin", MSG_SIZE, MAX_NB, DESTINATION, FIFO, &port_id, &ret);

/* 创建黑板 */
CREATE_BLACKBOARD("bb_acc1", MSG_SIZE, &bb_id, &ret);

/* 创建缓冲区 */
CREATE_BUFFER("buf_sensor", MSG_SIZE, MAX_NB, FIFO, &buf_id, &ret);

/* 创建任务 */
tattr.ENTRY_POINT = task11_job;
tattr.PERIOD = 25000000ll;   /* 25ms，单位 ns */
CREATE_PROCESS(&tattr, &proc_id, &ret);
START(proc_id, &ret);
```

**任务函数（在 `taskXX_job()` 中）**

```c
void task11_job(void) {
    while (1) {
        /* 读缓冲区 */
        RECEIVE_BUFFER(ps1_buf_sensor_id, TIMEOUT, &msg, &len, &ret);
        /* 写采样端口 */
        WRITE_SAMPLING_MESSAGE(ps1_pr1samplingout_id, &msg, len, &ret);
        /* 写黑板 */
        DISPLAY_BLACKBOARD(ps1_bb_acc1_id, &msg, len, &ret);
        /* 等待下一周期 */
        PERIODIC_WAIT(&ret);
    }
}
```

---

## 5. 分区–模块–C 代码对应速查表

| 分区 | 模块 | AADL 进程 | C 代码目录 | 任务数 | 采样 | 队列 | 黑板 | 缓冲 |
|------|------|----------|-----------|-------|------|------|------|------|
| ps1 | M1 | P1.impl | `m1_impl_instance/ps1/` | 4 | 1出 | 1入 | 5 | 3 |
| ps2 | M1 | P2.impl | `m1_impl_instance/ps2/` | 3 | 1入 | 1出 | 3 | 3 |
| ps3 | M2 | P3.impl | `m2_impl_instance/ps3/` | 3 | 0 | 2入 | 0 | 0 |
| ps4 | M3 | P4.impl | `m3_impl_instance/ps4/` | 5 | 0 | 2入+1出 | 0 | 0 |
| ps5 | M2 | P5.impl | `m2_impl_instance/ps5/` | 3 | 0 | 1入+1出 | 0 | 0 |
