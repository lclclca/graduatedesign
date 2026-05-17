# 输入格式对比实验 — 三份提示词

分区：ps2（DIMA 模块 M1）  
策略：零样本（三份提示词除"规格部分"外完全相同）

---

## 操作流程

1. 在 Claude 网页端**新开三个对话**（互相独立）
2. 每个对话粘贴下方对应的完整提示词
3. 将回复全文保存为文本文件：
   - 格式1回复 → `DIMA/eval/responses/nl_ps2.txt`
   - 格式2回复 → `DIMA/eval/responses/aadl_ps2.txt`
   - 格式3回复 → `DIMA/eval/responses/json_ps2.txt`
4. 运行：`python3 DIMA/eval/save_response.py`（自动解析并保存11个文件）
5. 运行：`python3 DIMA/eval/score_format_cmp.py`（输出对比评分表）

---

## 提示词 1 ——自然语言格式（NL）

> 复制下方全部内容粘贴入网页对话框

```
你是一名专业的嵌入式软件工程师，精通 ACoreOS653 实时操作系统和 ARINC 653 标准。

请根据以下分区规格，生成该分区的全部 11 个 C/H 源文件：
  activity.c / activity.h
  deployment.c / deployment.h
  globals.c / globals.h
  gtypes.c / gtypes.h
  main.c
  subprograms.c / subprograms.h

每个文件用如下格式输出：
```filename.c
/* 内容 */
```

要求：
- 严格遵守 ARINC 653 APEX API
- 所有 CREATE/READ/WRITE 操作必须双端配对
- HM 回调函数（module_HM_callback / partition_HM_callback）必须实现
- 所有 API 返回值必须用 CHECK_CODE 宏检查

【分区规格（自然语言描述）】

分区名称：ps2（所属模块：M1）

任务配置（3 个周期性任务）：
- task21：周期 50 ms，优先级 2
- task22：周期 50 ms，优先级 3
- task23：周期 100 ms，优先级 4

分区间通信端口：
- 采样输入端口：pr2samplingin（接收整数数据，READ_SAMPLING_MESSAGE）
- 队列输出端口：pr2queueingout（发送整数数据，队列容量 30，SEND_QUEUING_MESSAGE）

分区内共享资源：
- 黑板（BLACKBOARD）3 个：bb_acc3, bb_t2t3, bb_t3t2
- 缓冲区（BUFFER）3 个：buf_order, buf_t2tot3, buf_t3tot2

辅助子程序：commandboard_receiveinput_spg, commandboard_printinfos_spg

部署常量：
- NB_THREADS = 3
- NB_SAMPLING_PORTS = 1
- NB_QUEUING_PORTS = 1
- NB_BLACKBOARDS = 3
- NB_BUFFERS = 3
- STACKS_SIZE = 24576
```

---

## 提示词 2 ——AADL 原文格式（AADL）

> 复制下方全部内容粘贴入网页对话框

```
你是一名专业的嵌入式软件工程师，精通 ACoreOS653 实时操作系统和 ARINC 653 标准。

请根据以下分区规格，生成该分区的全部 11 个 C/H 源文件：
  activity.c / activity.h
  deployment.c / deployment.h
  globals.c / globals.h
  gtypes.c / gtypes.h
  main.c
  subprograms.c / subprograms.h

每个文件用如下格式输出：
```filename.c
/* 内容 */
```

要求：
- 严格遵守 ARINC 653 APEX API
- 所有 CREATE/READ/WRITE 操作必须双端配对
- HM 回调函数（module_HM_callback / partition_HM_callback）必须实现
- 所有 API 返回值必须用 CHECK_CODE 宏检查

【分区规格（AADL 模型原文）】

-- ===== 分区定义（DIMA_partitions.aadl 节选）=====
package DIMA_partitions
public
  with DIMA_threads; with ARINC653; with Base_Types;

process P2
    features
        pr2samplingin: in data port Base_Types::Integer {ARINC653::Sampling_Refresh_Period => 100ns;};
        pr2queueingout: out event data port Base_Types::Integer {Queue_Size => 30;
            ARINC653::Timeout => 5ns;
            ARINC653::Queueing_Discipline => FIFO;};
end P2;

process implementation P2.impl
    subcomponents
        task21: thread DIMA_threads::Tsk21.impl;
        task22: thread DIMA_threads::Tsk22.impl;
        task23: thread DIMA_threads::Tsk23.impl;
    connections
        sampleconnect: port pr2samplingin -> task21.task21sampling;
        queueconnect: port task23.task3Queuing -> pr2queueingout;
        task1to2BlackBoard: port task21.task1_acc3_out -> task22.task2_acc3_in;
        task2to1Buffer: port task22.task2_order_out -> task21.task1_order_in;
        task2to3buffer: port task22.task2_buffer_send -> task23.task3_buffer_receive;
        task3to2buffer: port task23.task3_buffer_send -> task22.task2_buffer_receive;
        task2to3blackboard: port task22.task2_blackboard_write -> task23.task3_blackboard_read;
        task3to2blackboard: port task23.task3_blackboard_write -> task22.task2_blackboard_read;
    properties
        ARINC653::DAL => LEVEL_A;
end P2.impl;

end DIMA_partitions;

-- ===== 线程定义（DIMA_threads.aadl 节选）=====
package DIMA_threads
public
  with ARINC653; with Base_Types;

thread Tsk21
    features
        task21sampling : in data port Base_Types::Integer;
        task1_acc3_out: out data port Base_Types::Integer;
        task1_order_in: in event data port Base_Types::Integer;
end Tsk21;

thread implementation Tsk21.impl
    calls Mycalls: {
        receive_spg: subprogram subprograms::commandboard_receiveinput_spg;
        print_spg: subprogram subprograms::commandboard_printinfos_spg;
    };
    properties
        Dispatch_Protocol => Periodic;
        Period => 50ms;
        Priority => 2;
        Stack_Size => 8Kbyte;
end Tsk21.impl;

thread Tsk22
    features
        task2_acc3_in: in data port Base_Types::Integer;
        task2_order_out: out event data port Base_Types::Integer;
        task2_buffer_send: out event data port Base_Types::Integer;
        task2_buffer_receive: in event data port Base_Types::Integer;
        task2_blackboard_read: in event data port Base_Types::Integer;
        task2_blackboard_write: out event data port Base_Types::Integer;
end Tsk22;

thread implementation Tsk22.impl
    properties
        Dispatch_Protocol => Periodic;
        Period => 50ms;
        Priority => 3;
        Stack_Size => 8Kbyte;
end Tsk22.impl;

thread Tsk23
    features
        task3_buffer_send: out event data port Base_Types::Integer;
        task3_buffer_receive: in event data port Base_Types::Integer;
        task3_blackboard_read: in event data port Base_Types::Integer;
        task3_blackboard_write: out event data port Base_Types::Integer;
        task3Queuing: out event data port Base_Types::Integer;
end Tsk23;

thread implementation Tsk23.impl
    calls Mycalls: {
        receive_spg: subprogram subprograms::commandboard_receiveinput_spg;
        print_spg: subprogram subprograms::commandboard_printinfos_spg;
    };
    properties
        Dispatch_Protocol => Periodic;
        Period => 100ms;
        Priority => 4;
        Stack_Size => 8Kbyte;
end Tsk23.impl;

end DIMA_threads;
```

---

## 提示词 3 ——结构化 JSON 格式（JSON）

> 复制下方全部内容粘贴入网页对话框

```
你是一名专业的嵌入式软件工程师，精通 ACoreOS653 实时操作系统和 ARINC 653 标准。

请根据以下分区规格，生成该分区的全部 11 个 C/H 源文件：
  activity.c / activity.h
  deployment.c / deployment.h
  globals.c / globals.h
  gtypes.c / gtypes.h
  main.c
  subprograms.c / subprograms.h

每个文件用如下格式输出：
```filename.c
/* 内容 */
```

要求：
- 严格遵守 ARINC 653 APEX API
- 所有 CREATE/READ/WRITE 操作必须双端配对
- HM 回调函数（module_HM_callback / partition_HM_callback）必须实现
- 所有 API 返回值必须用 CHECK_CODE 宏检查

【分区规格（结构化 JSON）】

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
```
