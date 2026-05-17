## 分区规格

分区名称：ps2（所属模块：M1）

任务配置（3个周期性任务）：
- task21：周期 50 ms，优先级 2
- task22：周期 50 ms，优先级 3
- task23：周期 100 ms，优先级 4

分区间通信端口：
- 采样输入端口：pr2samplingin（DESTINATION，READ_SAMPLING_MESSAGE）
- 队列输出端口：pr2queueingout（SOURCE，队列容量 30，SEND_QUEUING_MESSAGE）

分区内共享资源：
- 黑板（BLACKBOARD）3个：bb_acc3, bb_t2t3, bb_t3t2
- 缓冲区（BUFFER）3个：buf_order, buf_t2tot3, buf_t3tot2

辅助子程序：commandboard_receiveinput_spg, commandboard_printinfos_spg

部署参数：NB_THREADS=3, NB_SAMPLINGS=1, NB_QUEUEINGS=1, NB_BLACKBOARDS=3, NB_BUFFERS=3, STACKS_SIZE=24576

## 分区规格

```aadl
-- 分区定义（DIMA_partitions.aadl）
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

-- 线程定义（DIMA_threads.aadl）
thread Tsk21
    features
        task21sampling: in data port Base_Types::Integer;
        task1_acc3_out: out data port Base_Types::Integer;
        task1_order_in: in event data port Base_Types::Integer;
end Tsk21;
thread implementation Tsk21.impl
    calls Mycalls: {
        receive_spg: subprogram subprograms::commandboard_receiveinput_spg;
        print_spg: subprogram subprograms::commandboard_printinfos_spg;
    };
    properties
        Dispatch_Protocol => Periodic; Period => 50ms; Priority => 2; Stack_Size => 8Kbyte;
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
        Dispatch_Protocol => Periodic; Period => 50ms; Priority => 3; Stack_Size => 8Kbyte;
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
        Dispatch_Protocol => Periodic; Period => 100ms; Priority => 4; Stack_Size => 8Kbyte;
end Tsk23.impl;
```

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
