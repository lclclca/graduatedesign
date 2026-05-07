> **使用说明**：本文档为Markdown格式，可通过 `pandoc thesis_revised.md -o 论文修订版.docx --reference-doc=模板.docx` 转换为Word。
> 表格格式为三线表（无纵线，仅顶线、表头底线、底线），转Word后请在Word中对每个表格应用"三线表"样式。
> 图片位置以【图X-X：…】标注，对应文件见 `DIMA/eval/results/figures/` 目录。

---

# 基于提示工程的ARINC 653 IMA系统C代码自动生成方法研究

**南京航空航天大学**
本科毕业设计（论文）

---

## 摘　　要

综合模块化航空电子（IMA）系统是现代民用航空的核心架构，通过在共享计算平台上隔离部署多个功能分区，实现资源高效复用与强安全隔离。其分区软件须严格遵循ARINC 653标准，涵盖进程管理、分区间采样/队列端口通信、分区内黑板/缓冲区共享等复杂APEX接口调用，每个分区须生成11个C/H源文件。手工完成这一工作不仅开发周期长，且对工程师的专业知识要求极高，亟需自动化工具的支撑。

本文提出一种基于提示工程的ARINC 653分区C代码自动生成方法。该方法首先将分区的AADL架构描述转化为结构化JSON规格，再通过四种精心设计的提示词模板（零样本、思维链、少样本、组合）驱动大语言模型生成完整的分区C代码。实验在DIMA（5个分区）和IMA2（3个分区）两套真实IMA系统上进行，共完成16组实验。本文同时设计了三维评分框架（结构完整性30%、API正确性40%、语义一致性30%）和ARINC 653合规检查模块（R1–R8）对生成代码进行多层次量化评估，并实现了涵盖命令行脚本与图形化界面的自动化评估工具链。

实验结果表明，组合策略在三维综合得分（均值100.0）和ARINC 653合规率（均值94.4%）两项指标上均位居首位；少样本策略三维得分满分，但合规率（83.3%）低于组合策略；思维链策略因对辅助文件结构产生负面影响，综合得分最低（86.4%）。实验揭示，在规范约束密集的专业领域代码生成任务中，提供高质量参考示例是提升代码质量最有效的手段；而链式推理在缺乏示例的条件下对辅助文件生成存在干扰，需在提示设计中加以修正。

**关键词：** 提示工程；大语言模型；ARINC 653；代码生成；IMA

---

## Abstract

Integrated Modular Avionics (IMA) systems isolate multiple software partitions on a shared computing platform, enabling high resource utilization and strong safety guarantees required by modern civil aviation. Partition software must strictly conform to the ARINC 653 standard, which specifies complex APEX interface calls covering process management, inter-partition sampling/queuing port communication, and intra-partition blackboard/buffer sharing. Each partition requires generating 11 C/H source files, making manual implementation both time-consuming and error-prone.

This thesis proposes a prompt engineering-based approach for the automatic generation of ARINC 653 partition C code using Large Language Models (LLMs). The approach first extracts partition architecture information from AADL models into structured JSON specifications, which are then fed into four carefully designed prompt templates — Zero-Shot, Chain-of-Thought (CoT), Few-Shot, and Combined — to drive an LLM to generate complete partition code. Experiments are conducted across 16 trials on two real IMA systems: DIMA (5 partitions) and IMA2 (3 partitions). A three-dimensional quality evaluation framework (structural integrity 30%, API correctness 40%, semantic consistency 30%) and an ARINC 653 compliance check module (R1–R8) are designed for multi-level quantitative assessment, supported by an automated toolchain with both command-line scripts and a graphical user interface.

Results show that the Combined strategy achieves the highest three-dimensional score (mean 100.0) and ARINC 653 compliance rate (94.4%); the Few-Shot strategy achieves a perfect three-dimensional score but lower compliance (83.3%); the CoT strategy scores lowest (86.4%) due to its negative impact on auxiliary file generation. The study demonstrates that in domain-specific, constraint-intensive code generation tasks, providing high-quality reference examples is the most effective means of improving code quality, while chain-of-thought reasoning without examples can interfere with auxiliary file generation and requires targeted correction in prompt design.

**Keywords:** Prompt Engineering; Large Language Models; ARINC 653; Code Generation; IMA

---

## 目　　录

- 第一章 绪论
  - 1.1 研究背景
  - 1.2 国内外研究现状
  - 1.3 研究问题与目标
  - 1.4 研究方法
  - 1.5 论文结构安排
- 第二章 背景知识
  - 2.1 ARINC 653标准
  - 2.2 综合模块化航空电子系统
  - 2.3 AADL架构描述语言
  - 2.4 大语言模型与提示工程
- 第三章 基于提示词模板的IMA代码生成方法
  - 3.1 方法总体设计
  - 3.2 实验系统介绍
  - 3.3 提示词模板设计
  - 3.4 实验结果展示
- 第四章 代码质量评估
  - 4.1 评估框架设计
  - 4.2 评估工具实现
  - 4.3 三维评分实验结果
  - 4.4 ARINC 653合规检查结果
- 第五章 总结与展望
  - 5.1 研究工作总结
  - 5.2 主要结论
  - 5.3 研究局限
  - 5.4 未来工作
- 参考文献
- 在学期间取得的主要成果
- 致　　谢

---

# 第一章 绪论

## 1.1 研究背景

航空电子系统是现代民用飞机的核心组成部分，其可靠性和安全性直接关系到飞行安全。随着飞机功能的不断扩展和集成需求的增加，传统的联合式航空电子架构（Federated Avionics）因资源利用率低、扩展性差等问题，逐渐被综合模块化航空电子（Integrated Modular Avionics，IMA）架构所取代。IMA架构通过共享通用计算平台承载多个功能分区，在降低硬件成本的同时提供更强的系统集成能力，已成为波音787、空客A380等现代民航客机的核心软件平台。

ARINC 653是规范IMA系统分区软件接口的核心标准，由航空无线电技术委员会（AERONAUTICAL RADIO, INCORPORATED）制定。该标准定义了应用程序执行环境（Application EXecutive，APEX）服务接口，包括进程管理、分区间通信（采样端口、队列端口）、分区内通信（黑板、缓冲区）和分区模式管理等核心功能，是保证多功能分区在同一物理平台上安全隔离运行的技术基础[14]。

然而，手工编写符合ARINC 653标准的C代码面临显著挑战。首先，APEX API数量庞大且参数复杂，工程师需要深入理解标准细节；其次，分区代码结构固定但冗长，每个分区通常需要编写包括main.c、activity.c、deployment.h等在内的11个C/H源文件；再者，AADL架构描述与C代码实现之间存在大量需要人工对应的映射关系，极易产生遗漏或不一致。国内外研究者已尝试基于模型驱动工程（MDE）实现从AADL模型到ARINC 653代码的自动生成[5][6][7][8]，但这类方法依赖专用工具链，对模型的形式化程度要求高，在工程实践中推广受限。

近年来，大语言模型（Large Language Model，LLM）在代码生成领域展现出前所未有的能力，为ARINC 653分区代码的自动生成提供了新的技术路径。利用提示工程（Prompt Engineering）引导LLM生成结构化、规范化代码，有望在不依赖专用工具链的情况下实现高质量的ARINC 653代码自动生成。

## 1.2 国内外研究现状

### 1.2.1 大语言模型代码生成

大语言模型在代码生成领域展现出了显著能力。以GPT-4、Claude、Gemini为代表的通用大语言模型，在HumanEval、MBPP等代码基准测试上的通过率已超过人类平均水平[13]。研究表明，LLM在开放领域代码生成中表现优异，但在领域专有代码生成场景下面临显著挑战，特别是当目标代码需遵循特定API规范和隐式约定时，模型的表现会大幅下降[15]。

在提示工程方法方面，Wei等人提出的思维链（Chain-of-Thought，CoT）提示技术通过引导模型逐步推理来提升复杂任务的完成质量[1]；Kojima等人发现仅凭"让我们一步一步思考"这样的零样本触发词即可激发模型的推理能力[2]；Wang等人在此基础上提出了计划-求解（Plan-and-Solve）提示框架，进一步提升零样本推理质量[3]。针对代码生成的结构化思维链（SCoT）方法通过显式引入编程结构（顺序/分支/循环）来指导代码推理，在HumanEval等基准上超越CoT方法达13.79%[4]。少样本情境学习（In-Context Learning）通过选取高质量示例引导LLM生成目标格式代码，示例质量对性能影响显著[17]。面向安全性的提示技术研究表明，不同提示策略对生成代码的安全性和规范符合度有系统性影响，递归批评改进（RCI）等技术可显著减少安全弱点[16]。

除提示工程外，参数高效微调（PEFT）和检索增强代码生成（RAG）也是当前LLM代码生成的两条主要路线。PEFT方法（如LoRA、Adapter）通过仅调整少量参数对LLM进行领域适应，在多个代码生成数据集上超越基于提示的方法[18]；RAG方法通过检索相关代码片段并注入提示来增强代码生成能力，在语义差距较大的领域任务中效果显著[20]。然而，这两类方法均需要领域训练数据或代码库，在ARINC 653这类代码样本极为稀少的专业领域中，构建此类数据集的代价远高于设计提示词模板。

### 1.2.2 领域专用代码自动生成

在航空领域，基于模型驱动工程的代码生成方法已有较成熟的研究。Hugues和Delange提出利用AADL模型和Ocarina工具链自动生成ARINC 653分区代码[7]；Lukić等人则提出了基于系统架构模型自动生成和验证ARINC 653合规航空代码的流水线方法[5][6]；AADL653形式化语言提供了专门针对ARINC 653建模和验证的语言扩展[8]。国内，南京航空航天大学安全关键软件研究课题组在AADL模型到ARINC 653 C代码自动生成方面开展了系统性研究，提出了HMC4ARINC653属性集扩展和从IMA模型到C代码的映射规则，生成的代码符合MISRA C安全编码规范[15+]。

在嵌入式和航天领域LLM应用方面，Englhardt等人系统考察了LLM在嵌入式系统开发中的能力边界[9]；Fakih等人提出了LLM4PLC方法，将LLM与形式化验证相结合用于工业控制代码生成[10]；Babiuch等人对多个LLM在微控制器驱动应用编程任务上进行了系统基准测试[11]；面向航天领域的LLM代码生成研究表明，LLM在结构化规范翻译任务中具有应用潜力，但需要专业提示策略的支撑[12]。

现有研究在面向ARINC 653这类对规范符合性要求极高的安全关键代码生成方面尚缺乏系统性研究，特别是针对不同提示策略的定量比较和面向ARINC 653标准的专用评估框架设计方面存在空白，这正是本文的研究切入点。

## 1.3 研究问题与目标

本文围绕以下核心研究问题展开：

（1）能否借助大语言模型以AADL分区规格为输入，自动生成结构完整、API正确、语义一致的ARINC 653 IMA分区C代码？

（2）四种典型提示策略（零样本、思维链、少样本、组合）在ARINC 653代码生成质量上存在哪些显著差异？其失效模式是什么？

（3）如何设计面向ARINC 653域特征的代码质量评估框架，全面衡量生成代码的规范符合程度？

## 1.4 研究方法

本文主要工作围绕如何设计有效的提示策略使大语言模型生成结构完整、API正确、语义一致的ARINC 653 IMA分区C代码这一核心问题展开：首先构建了从AADL系统架构描述到ARINC 653 C代码的自动生成流水线，实现结构化语义信息的有效传递；然后设计了四种提示词策略并在两个真实IMA系统上开展对比实验；最后提出了三维评分框架和ARINC 653合规检查模块，对生成代码进行多维度定量评估。

## 1.5 论文结构安排

本论文的组织结构如下：

第一章（绪论）：阐明课题的研究背景，介绍课题的研究现状和来源，并概述论文的主要工作。

第二章（背景知识）：介绍ARINC 653标准、IMA架构、AADL语言和提示工程的背景知识。

第三章（基于提示词模板的IMA代码生成方法）：描述代码生成架构、提示词模板设计和实验结果展示。

第四章（代码质量评估）：介绍评估框架设计、评估工具实现和各策略评估结果分析。

第五章（总结与展望）：总结全文工作并展望未来研究方向。

---

# 第二章 背景知识

## 2.1 ARINC 653标准

### 2.1.1 分区模型

ARINC 653将一个物理计算节点划分为若干分区（Partition），每个分区拥有独占的内存空间和周期性的CPU时间窗口。分区调度由操作系统根据主帧（Major Frame，MAF）配置循环执行，每个分区在每个MAF周期内获得固定的时间片，分区之间在时间和空间上完全隔离，任何分区的故障均不会直接影响其他分区的运行状态。这种强隔离机制是IMA架构实现安全关键系统混合部署的基础[14]。

### 2.1.2 APEX服务接口

ARINC 653 Part 1定义了APEX接口，规范了分区软件与底层RTOS之间的交互方式。核心API可分为以下几类：

**进程管理**：`CREATE_PROCESS`用于在分区初始化阶段创建周期性任务，参数包括任务名称、入口函数指针、优先级、周期（纳秒）、栈大小等；`START`启动已创建的进程；`PERIODIC_WAIT`是任务主循环的最后一条语句，使任务挂起直至下一个调度周期到来。

**分区间通信**：采样端口（`CREATE_SAMPLING_PORT`）用于传输最新值，源端调用`WRITE_SAMPLING_MESSAGE`写入，目标端调用`READ_SAMPLING_MESSAGE`读取，旧值会被新值覆盖；队列端口（`CREATE_QUEUING_PORT`）以FIFO方式传输消息序列，源端调用`SEND_QUEUING_MESSAGE`，目标端调用`RECEIVE_QUEUING_MESSAGE`。

**分区内通信**：黑板（`CREATE_BLACKBOARD`）用于分区内任务间共享最新数据，写入调用`DISPLAY_BLACKBOARD`，读取调用`READ_BLACKBOARD`；缓冲区（`CREATE_BUFFER`）用于分区内任务间的消息队列传递，发送调用`SEND_BUFFER`，接收调用`RECEIVE_BUFFER`。

**分区模式管理**：`SET_PARTITION_MODE(NORMAL)`是分区初始化函数`appMain`的最后一条语句，标志分区从初始化模式切换到正常运行模式。

### 2.1.3 健康监控机制

ARINC 653要求每个分区实现模块级和分区级健康监控回调函数（`module_HM_callback`和`partition_HM_callback`），用于响应错误事件并执行恢复动作。这两个函数必须在分区代码中定义，否则在运行时无法响应系统错误。

### 2.1.4 代码错误检查宏

在实际工程实践中，每次APEX API调用后通常跟随错误检查。本文采用`CHECK_CODE(msg, code)`宏，当返回码为`NO_ERROR`时打印成功信息，否则打印实际错误码，便于调试。

## 2.2 综合模块化航空电子系统

### 2.2.1 分区软件文件结构

基于ACoreOS653实时操作系统的IMA分区软件采用固定的11文件结构：

- `deployment.h/c`：定义分区配置常量，包括线程数（`ACoreOS653_CONFIG_NB_THREADS`）、采样端口数、队列端口数、黑板数、缓冲区数及栈总大小；
- `gtypes.h/c`：定义基本类型映射，如`typedef int integer`；
- `globals.h/c`：定义全局宏，如`CHECK_CODE`错误检查宏；
- `subprograms.h/c`：声明和实现子程序（功能函数）；
- `activity.h/c`：声明和实现所有周期性任务函数；
- `main.c`：实现`appMain`初始化函数和HM回调函数。

这种固定结构既规范了代码组织，也为自动生成提供了明确的目标格式。

## 2.3 AADL架构描述语言

AADL（Architecture Analysis and Design Language）是SAE International制定的架构描述语言标准，广泛应用于航空电子、汽车电子等安全关键系统的建模与分析。AADL能够精确描述系统的硬件资源、软件组件及其交互关系，支持多种非功能属性（时序、安全、资源占用等）的建模与分析。

在本文所涉及的范围内，AADL的关键语法元素包括：

- `process`：对应一个分区，其`features`中声明该分区对外的通信端口；
- `thread`：对应分区内的一个周期性任务，其`properties`中包含调度属性（`Dispatch_Protocol`、`Period`、`Priority`、`Stack_Size`等）；
- `process implementation`：在`subcomponents`中列出该分区包含的所有线程实例，在`connections`中描述端口连接关系；
- ARINC 653相关属性：`Queue_Size`、`ARINC653::Sampling_Refresh_Period`、`ARINC653::Queueing_Discipline`等。

AADL模型作为系统架构的权威描述，其中包含了生成分区代码所需的全部语义信息：任务数量与参数、端口类型与方向、资源数量与命名。

## 2.4 大语言模型与提示工程

### 2.4.1 大语言模型

大语言模型是基于Transformer架构在海量文本语料上预训练的语言模型，具有强大的文本理解和生成能力。当前主流的大语言模型（如GPT-4、Claude 3.5等）在代码生成、逻辑推理、知识问答等任务上已达到甚至超越人类平均水平[13]。近年来，LLM的代码生成能力成为重要研究方向，代码评估方法也在不断演进——从基于文本匹配的方法（如BLEU）到基于执行语义的方法（如CodeScore[19]），评估粒度不断向功能正确性靠近。

### 2.4.2 四类提示策略

**零样本提示（Zero-Shot，ZS）**：仅向模型提供任务描述和输入数据，不提供任何示例。模型依赖预训练知识直接生成输出。零样本提示实现简单，但对模型的领域知识储备要求较高，在专业领域任务中效果往往不稳定[2]。

**思维链提示（Chain-of-Thought，CoT）**：在提示词中要求模型在给出最终答案前进行逐步推理，将复杂问题分解为多个子步骤[1]。研究表明，思维链显著提升了模型在数学推理和逻辑推理任务上的表现。针对代码生成，结构化思维链（SCoT）通过显式引入编程结构指导推理过程，进一步提升了CoT的代码生成效果[4]。

**少样本提示（Few-Shot，FS）**：在提示词中提供若干输入-输出示例对，引导模型学习目标格式和风格。LLM感知的示例质量对少样本性能有决定性影响，高质量示例的选取策略是In-Context Learning研究的核心问题[17]。少样本提示能够有效传递领域特定的代码风格和约束，对于格式固定的代码生成任务尤为有效。

**组合策略（Combined）**：将少样本示例与思维链推理步骤相结合，同时利用示例的格式引导和推理链的逻辑分析优势。理论上，组合策略能够在继承少样本格式学习能力的同时，通过逐步分析减少因模型"直接套用"示例而产生的错误。

### 2.4.3 结构化输入对代码生成的影响

代码生成任务中，提示词的输入格式对生成质量有显著影响。结构化的JSON格式去除了自然语言和形式化语言（如AADL）中的语法噪声，以机器友好的方式精确传递语义信息，使模型能够直接读取所需的数值参数（任务周期、优先级等），减少解析错误。

---

# 第三章 基于提示词模板的IMA代码生成方法

## 3.1 方法总体设计

本文研究的核心问题是：能否借助大语言模型（LLM），以AADL分区规格为输入，自动生成符合ARINC 653标准的IMA系统C代码？为此，本章提出一种基于提示词模板的IMA代码生成方法，其基本思路是：将分区的AADL设计信息提炼为结构化的JSON规格文件，再通过不同设计策略的提示词模板引导LLM生成完整的11个C/H源文件。

整体流程分为三个阶段。第一阶段为规格提取：从AADL模型中解析出分区的任务参数、端口定义、内部资源配置等信息，序列化为标准JSON格式；第二阶段为提示词生成：将JSON规格填入预设模板，形成完整的LLM输入；第三阶段为代码评估：对LLM输出的代码执行自动化质量检查，评估其与参考实现的符合程度。

【图3-1：AADL到C代码的生成流程图 — 待插入】

该方法的关键设计决策在于提示词模板的选择。不同的提示策略会显著影响LLM对ARINC 653规范约束的理解程度，进而影响生成代码的质量。本章设计了四种提示词策略（零样本、思维链、少样本、组合），并在两个真实IMA系统上开展实验，量化各策略的效果差异。

## 3.2 实验系统介绍

实验在两个规模和复杂度不同的IMA系统上进行，分别用于测试DIMA系统的零样本/思维链实验和IMA2系统的少样本/组合实验。

### 3.2.1 DIMA系统

DIMA（Distributed IMA，分布式综合模块化航空电子系统）是一个典型的多模块IMA系统，由3个物理模块（M1/M2/M3）、5个分区（ps1–ps5）和15个周期性任务构成。各模块之间通过光纤通道（Fibre Channel）交换机互联，实现跨模块的采样端口与队列端口通信。

DIMA系统的整体拓扑如图3-2所示。模块M1（嵌入式系统es1）承载分区ps1和ps2，定位为指令处理模块；模块M2（es2）承载ps3和ps5，定位为任务分发模块；模块M3（es3）承载ps4，定位为执行管理模块。主帧周期均为25ms，采用速率单调调度（RMS）策略。

【图3-2：DIMA系统拓扑结构 — 待插入】

5个分区在资源组合上覆盖了从复杂到简单的多种配置，为实验提供了充分的多样性。

| 分区 | 模块 | 任务数 | 采样端口 | 队列端口 | 黑板数 | 缓冲区数 |
|------|------|--------|----------|----------|--------|----------|
| ps1  | M1   | 4      | 1出      | 1入      | 5      | 3        |
| ps2  | M1   | 3      | 1入      | 1出      | 3      | 3        |
| ps3  | M2   | 3      | —        | 2入      | 0      | 0        |
| ps4  | M3   | 5      | —        | 2入+1出  | 0      | 0        |
| ps5  | M2   | 3      | —        | 1入+1出  | 0      | 0        |

**表3-1 DIMA系统分区资源配置**

ps3是DIMA中结构最简单的分区，仅包含队列接收端口和周期任务，无分区内共享资源；ps1是最复杂的分区，同时使用采样端口、队列端口、5块黑板和3个缓冲区，覆盖全部4类ARINC 653资源。DIMA系统由AADL模型文件经`aadl2c.py`脚本生成参考代码，作为评估基准。

### 3.2.2 IMA2系统

IMA2系统是为少样本实验专门设计的新IMA系统，包含1个模块（MA）、3个分区（PA/PB/PC）和6个周期性任务，采用单模块架构，分区间通信通过模块内部连接实现。

IMA2系统的设计原则是使三个分区在资源复杂度上呈梯度分布，从而评估LLM在见过参考示例后对不同复杂度目标的迁移能力。

- **PA**（极简）：1个任务，仅有1个队列接收端口，无分区内共享资源，是验证LLM掌握ARINC 653最基本代码框架的基准用例。
- **PB**（中等）：2个任务，引入采样输出端口、队列输入端口和1块黑板，覆盖两类分区间通信和一类分区内共享。
- **PC**（复杂）：3个任务，同时使用采样输入、队列输出、1块黑板和1个缓冲区，覆盖全部4类ARINC 653资源，是IMA2系统中最全面的测试用例。

| 分区 | 任务数 | 采样端口 | 队列端口 | 黑板数 | 缓冲区数 | 复杂度 |
|------|--------|----------|----------|--------|----------|--------|
| PA   | 1      | —        | 1入      | 0      | 0        | 低     |
| PB   | 2      | 1出      | 1入      | 1      | 0        | 中     |
| PC   | 3      | 1入      | 1出      | 1      | 1        | 高     |

**表3-2 IMA2系统分区资源配置**

少样本实验以DIMA分区ps3的规格JSON和参考C代码作为in-context示例，以IMA2的PA/PB/PC作为待生成目标，评估LLM利用示例迁移到新系统的能力。

## 3.3 提示词模板设计

本文设计了四种提示词模板，对应四种不同的提示工程策略：零样本（Zero-Shot）、思维链（Chain-of-Thought, CoT）、少样本（Few-Shot）和组合（Combined）。四种策略的核心设计差异体现在三个维度：是否提供参考示例、是否引导分步推理、是否提供API参考。

| 策略     | 参考示例 | API参考 | 分步推理引导 | 主要作用机制         |
|----------|----------|---------|--------------|----------------------|
| 零样本   | 无       | 无      | 无           | 依赖LLM内化的ARINC 653知识 |
| 思维链   | 无       | 有      | 有           | 通过分步推理降低遗漏率 |
| 少样本   | 1个      | 无      | 无           | 示例模仿，降低格式偏差 |
| 组合     | 1个      | 有      | 有           | 示例+推理双重引导     |

**表3-3 四种提示词策略核心设计对比**

### 3.3.1 零样本提示词模板

零样本模板（Zero-Shot Template）是最简洁的提示策略，仅向LLM提供角色定义、分区规格JSON和代码规范要求，不包含任何示例代码或API参考，完全依赖LLM在预训练阶段积累的ARINC 653知识[2]。

模板结构如下：首先明确角色（"你是一名熟悉ARINC 653标准和ACoreOS653实时操作系统的嵌入式航空软件工程师"），然后以JSON块嵌入分区规格，最后列出11个输出文件的具体要求和代码规范（任务函数命名规则、头文件保护、API调用模式等）。

零样本模板的优势在于输入简洁、无歧义，适合测试模型对ARINC 653规范的内化程度。其局限在于：当分区资源组合复杂时（如ps1有5块黑板+3个缓冲区），模型可能遗漏部分资源的初始化或产生端口API方向错误。

### 3.3.2 思维链提示词模板

思维链模板（CoT Template）在零样本基础上增加了两项关键设计：一是在提示中嵌入完整的ARINC 653 APEX API参考（包括端口创建、黑板/缓冲区创建、任务通信API和周期等待调用的完整签名）；二是设计四步推理引导序列，要求模型在生成代码前依次完成：

1. **分析端口和通信资源**：列出所有需创建的端口类型、名称、方向；
2. **规划appMain的CREATE顺序**：明确资源创建序列；
3. **分析每个任务的通信行为**：确定各任务读写哪些资源；
4. **生成11个文件的完整代码**。

API参考的嵌入使模型无需依赖记忆即可正确填写API签名，推理引导则有助于减少资源遗漏和任务通信逻辑错误。然而，CoT模板要求更长的输出，且有时模型在推理阶段产生正确分析但在代码阶段遗漏部分约束（如`deployment.h`中的宏值）[1][4]。

### 3.3.3 少样本提示词模板

少样本模板（Few-Shot Template）以DIMA系统ps3分区的完整规格JSON和对应的11个参考C文件作为in-context示例嵌入提示词，然后在示例之后提供目标分区的规格JSON。模板要求LLM"仔细学习该示例的代码结构和风格，然后为新的目标分区生成风格一致的代码"[17]。

选择ps3作为示例的依据：ps3是DIMA中结构最简单的分区（3个任务，2个队列接收端口，无黑板/缓冲区），其代码简洁且完整体现了ARINC 653 C代码的基本框架（11文件结构、`appMain`初始化序列、任务`while`循环、`PERIODIC_WAIT`、HM回调），适合作为迁移学习的基准。

少样本模板的核心机制是模式迁移：LLM通过示例学习全局变量命名规则（分区名前缀）、`extern`声明模式、`CHECK_CODE`宏的定义和使用、`deployment.h`的宏命名约定等隐式规范，这些细节在零样本和CoT模板中均需通过文字描述传达，而示例可以直接呈现。

### 3.3.4 组合提示词模板

组合模板（Combined Template）将少样本模板的参考示例与CoT模板的API参考和推理引导合并，形成三部分结构：第一部分为ps3分区的完整示例，第二部分为针对示例中未覆盖的资源类型（采样端口、黑板、缓冲区）补充的API代码片段，第三部分为目标分区规格和五步推理引导。

五步推理序列在CoT四步基础上增加了"**对照示例列出差异**"步骤，引导模型先识别目标分区与示例的异同，再进行资源规划和代码生成。这一设计使模型能以示例为锚点，有针对性地调整生成内容，而非从零推理完整框架。

组合模板是四种策略中信息量最丰富的，也是设计最复杂的，其目标是通过示例、API参考和推理链的协同作用，最大化代码生成的完整性和规范符合度[1][4][17]。

## 3.4 实验结果展示

### 3.4.1 实验配置

实验选用Claude claude-sonnet-4-6（claude-sonnet-4-6）作为LLM后端，温度参数设为默认值（temperature=1.0）。对于DIMA系统，零样本和思维链两种策略各在ps1–ps5五个分区上执行一次，共生成10组代码；对于IMA2系统，少样本和组合两种策略各在PA/PB/PC三个分区上执行一次，共生成6组代码。所有生成代码存放于`DIMA/eval/generated/<策略>/<分区>/`目录，共计16组实验结果。

| 策略   | 测试系统 | 测试分区       | 实验次数 | 参考示例       |
|--------|----------|----------------|----------|----------------|
| 零样本 | DIMA     | ps1–ps5        | 5        | 无             |
| 思维链 | DIMA     | ps1–ps5        | 5        | 无（含API参考）|
| 少样本 | IMA2     | PA、PB、PC     | 3        | ps3完整代码    |
| 组合   | IMA2     | PA、PB、PC     | 3        | ps3完整代码    |

**表3-4 实验策略与测试系统对应关系**

### 3.4.2 AADL规格到C代码的对应关系

ARINC 653 C代码的核心结构直接映射自AADL分区模型中的三类设计元素：进程（Process）对应分区任务，特性（Properties）对应周期和优先级，端口与共享资源（Port/Feature）对应APEX通信API。

【图3-3：AADL分区规格到C代码的对应关系图 — 待插入】

从AADL规格到C代码的映射规则可以归纳如下：

| AADL元素 | 规格JSON字段 | 生成的C代码结构 |
|----------|-------------|----------------|
| 线程（Thread） | `tasks[].name` | `void *<name>_job(void)` 任务函数 |
| 线程周期（Period） | `tasks[].period_ms` | `tattr.PERIOD = <ms>×1000000ll`（纳秒） |
| 线程优先级（Priority） | `tasks[].priority` | `tattr.BASE_PRIORITY = <priority>` |
| 采样端口 SOURCE | `sampling_ports[].direction=SOURCE` | `CREATE_SAMPLING_PORT(…, SOURCE, …)` + `WRITE_SAMPLING_MESSAGE` |
| 采样端口 DESTINATION | `sampling_ports[].direction=DESTINATION` | `CREATE_SAMPLING_PORT(…, DESTINATION, …)` + `READ_SAMPLING_MESSAGE` |
| 队列端口 SOURCE | `queuing_ports[].direction=SOURCE` | `CREATE_QUEUING_PORT(…, SOURCE, …)` + `SEND_QUEUING_MESSAGE` |
| 队列端口 DESTINATION | `queuing_ports[].direction=DESTINATION` | `CREATE_QUEUING_PORT(…, DESTINATION, …)` + `RECEIVE_QUEUING_MESSAGE` |
| 黑板（Blackboard） | `blackboards[]` | `CREATE_BLACKBOARD` + `DISPLAY_BLACKBOARD`/`READ_BLACKBOARD` |
| 缓冲区（Buffer） | `buffers[]` | `CREATE_BUFFER` + `SEND_BUFFER`/`RECEIVE_BUFFER` |
| 资源数量 | `deployment.nb_*` | `deployment.h`中的`ACoreOS653_CONFIG_NB_*`宏 |

**表3-5 AADL设计元素与C代码的对应关系**

每个分区生成的代码包含固定的11个文件，其中`activity.c`和`main.c`是包含业务逻辑的核心文件，其余文件提供类型定义、全局变量声明和部署配置。

| 文件 | 主要内容 | 复杂度相关性 |
|------|----------|-------------|
| `deployment.h` | 编译期常量（线程数、端口数等） | 随资源增加 |
| `deployment.c` | 仅`#include "deployment.h"` | 固定 |
| `globals.h` | `CHECK_CODE`宏定义 | 固定 |
| `globals.c` | 仅`#include "globals.h"` | 固定 |
| `gtypes.h` | `typedef int integer` | 固定 |
| `gtypes.c` | 仅`#include "gtypes.h"` | 固定 |
| `subprograms.h/c` | 子程序声明与桩实现 | 随子程序增加 |
| `activity.h` | 所有任务函数声明 | 随任务数增加 |
| `activity.c` | 任务函数实现（`while(1)`循环） | 随资源+任务增加 |
| `main.c` | `appMain()`初始化 + HM回调 | 随资源增加 |

**表3-6 分区C代码文件组成**

各策略的代码质量量化评估结果详见第四章。

---

# 第四章 代码质量评估

## 4.1 评估框架设计

为量化LLM生成的ARINC 653 C代码与参考实现之间的差距，本文构建了一套自动化代码评估框架。该框架由两个互补的模块组成：面向代码质量的三维评分框架和面向标准符合性的ARINC 653合规检查模块。

### 4.1.1 三维评分框架

三维评分框架从三个相互独立、互为补充的维度对生成代码进行评分，每个维度均有明确的权重和检查项集合，最终加权得到综合得分。这一分层评估思路借鉴了代码功能正确性与形式特征相结合的评估方法[19]。

**维度一：结构完整性（权重30%）**

结构完整性评估代码文件层面和基础代码模式层面的完整程度，共8个检查项（S1–S8）。

| 检查项 | 描述 |
|--------|------|
| S1 | 必需文件完整性（11个文件全部存在） |
| S2 | `main.c`中定义了`appMain(void)` |
| S3 | `activity.h`声明了所有任务函数 |
| S4 | `activity.c`实现了所有任务函数 |
| S5 | 所有`.h`文件有头文件保护（`#ifndef`/`#define`/`#endif`） |
| S6 | `activity.c`和`main.c`包含`apexLib.h` |
| S7 | `globals.h`定义了`CHECK_CODE`宏（else分支打印真实错误码） |
| S8 | `gtypes.h`中有`typedef int integer` |

**表4-1 结构完整性检查项**

**维度二：API正确性（权重40%）**

API正确性评估代码对ARINC 653 APEX服务接口的调用是否正确，共12个检查项（A1–A12）。该维度权重最高，因为API调用错误将直接导致运行时异常。

| 检查项 | 描述 |
|--------|------|
| A1–A4 | `CREATE_SAMPLING_PORT`/`CREATE_QUEUING_PORT`/`CREATE_BLACKBOARD`/`CREATE_BUFFER`调用次数与规格一致 |
| A5–A6 | `CREATE_PROCESS`和`START`调用次数与任务数一致 |
| A7 | `main.c`末尾调用`SET_PARTITION_MODE(NORMAL)` |
| A8 | 每个任务函数体包含`PERIODIC_WAIT` |
| A9 | `CHECK_CODE`调用次数合理（≥APEX调用总数） |
| A10 | 采样端口方向与API匹配（SOURCE→`WRITE_SAMPLING_MESSAGE`，DESTINATION→`READ_SAMPLING_MESSAGE`） |
| A11 | 队列端口方向与API匹配（SOURCE→`SEND_QUEUING_MESSAGE`，DESTINATION→`RECEIVE_QUEUING_MESSAGE`） |
| A12 | `activity.c`中不含`GET_QUEUING_PORT_ID`（端口ID应在`appMain`创建时获得） |

**表4-2 API正确性检查项**

**维度三：语义一致性（权重30%）**

语义一致性评估生成代码中的具体数值和标识符是否与AADL规格精确一致，共10个检查项（C1–C10）。

| 检查项 | 描述 |
|--------|------|
| C1 | 任务数量与规格一致 |
| C2 | 任务名称与AADL中的`tasks[].name`完全匹配 |
| C3 | 任务周期值（纳秒）与规格一致 |
| C4 | 任务优先级与规格一致 |
| C5 | 分区间端口名称出现在代码中 |
| C6 | 黑板名称出现在代码中 |
| C7 | 缓冲区名称出现在代码中 |
| C8 | 子程序名称出现在代码中 |
| C9 | `deployment.h`中`NB_THREADS`值与规格一致 |
| C10 | `deployment.h`中`NB_QUEUEINGS`和`NB_SAMPLINGS`值与规格一致 |

**表4-3 语义一致性检查项**

三个维度的分数均映射到0–100区间，综合得分按以下公式计算：

$$S_{total} = 0.3 \times S_{struct} + 0.4 \times S_{API} + 0.3 \times S_{sem}$$

综合得分≥80判定为"优秀"（达到参考代码标准），60–80为"良好"，40–60为"一般"，<40为"较差"。

### 4.1.2 ARINC 653合规检查模块

三维评分框架侧重于代码的文本特征匹配，对于"资源被创建但从未被任务读写"这类深层语义问题的检测能力有限。例如，C6项（黑板名称出现在代码中）通过检查名称字符串的存在性来判断，但若代码中仅有`extern BLACKBOARD_ID_TYPE bb_xx_id;`声明而无任何`DISPLAY_BLACKBOARD`/`READ_BLACKBOARD`调用，C6仍会通过，却说明黑板资源实际上从未被使用。

为弥补这一盲点，本文设计了8条ARINC 653合规规则（R1–R8），检查生成代码在资源实际使用层面的符合性。

| 规则 | 检查对象 | 判定依据 |
|------|----------|----------|
| R1 | 黑板实际读写 | `activity.c`中同时存在`DISPLAY_BLACKBOARD`（写入）和`READ_BLACKBOARD`（读取）调用 |
| R2 | 缓冲区实际收发 | `activity.c`中同时存在`SEND_BUFFER`（发送）和`RECEIVE_BUFFER`（接收）调用 |
| R3 | 采样SOURCE端口 | `activity.c`中存在`WRITE_SAMPLING_MESSAGE`调用 |
| R4 | 采样DESTINATION端口 | `activity.c`中存在`READ_SAMPLING_MESSAGE`调用 |
| R5 | 队列SOURCE端口 | `activity.c`中存在`SEND_QUEUING_MESSAGE`调用 |
| R6 | 队列DESTINATION端口 | `activity.c`中存在`RECEIVE_QUEUING_MESSAGE`调用 |
| R7 | HM回调函数 | `main.c`中定义`module_HM_callback`和`partition_HM_callback` |
| R8 | 禁止运行时初始化 | `activity.c`的`while(1)`循环后无`CREATE_*`调用 |

**表4-4 ARINC 653合规检查规则**

R1–R6检查特定通信API是否出现在`activity.c`中，R7检查`main.c`中的函数定义，R8检查禁止在任务主循环中动态创建资源的约束（ARINC 653规定所有资源必须在分区进入NORMAL模式前完成初始化）。当分区不含某类资源时（如无黑板），对应规则自动视为通过。

两个评估模块互为补充：三维评分框架评估代码的**结构和格式正确性**，合规检查模块评估**资源的实际使用完整性**。两者结合才能全面评价LLM生成代码的质量。

## 4.2 评估工具实现

基于第4.1节设计的评估框架，本文实现了一套自动化评估工具，包括命令行脚本和图形化界面两个层次。工具采用Python编写，核心评估逻辑封装在`DIMA/eval/metrics/`包中，上层脚本和GUI通过模块导入复用同一套计算逻辑，确保命令行与界面输出的一致性。整体架构如图4-1所示。

【图4-1：评估工具架构图 — 待插入】

工具的目录结构如下：

```
DIMA/eval/
├── metrics/
│   ├── structural.py       # S1–S8 结构完整性检查
│   ├── api_check.py        # A1–A12 API正确性检查
│   ├── semantic.py         # C1–C10 语义一致性检查
│   └── arinc_compliance.py # R1–R8 ARINC 653合规检查
├── specs/                  # 各分区规格JSON（pa/pb/pc/ps1–ps5）
├── generated/              # LLM生成代码（zeroshot/cot/fewshot/combined）
├── results/                # 评估结果输出（JSON/CSV）
├── evaluate.py             # 单分区三维评分命令行脚本
├── batch_check.py          # ARINC 653合规批量检查脚本
└── gui.py                  # 图形化评估界面
```

### 4.2.1 核心评估模块

`metrics/`包中的四个模块分别实现第4.1节定义的四组检查规则，均以`check(gen_dir, spec)`为统一接口：输入为生成代码目录和规格字典，返回包含`id`、`desc`、`passed`、`partial`、`note`字段的检查项列表。

- **structural.py**：通过文件系统操作（`os.listdir`）和正则表达式检查11个文件的存在性、函数定义的完整性、头文件保护宏的格式等结构特征，实现S1–S8共8项检查。

- **api_check.py**：通过正则表达式计数各类`CREATE_*`、`START`、`PERIODIC_WAIT`等APEX API在代码中的调用次数，与规格JSON中的资源数量对照，并检查端口方向与读写API的匹配关系，实现A1–A12共12项检查。

- **semantic.py**：提取任务函数名、周期值、优先级等标识符和数值，与规格JSON中的`tasks[]`字段精确对比，检查`deployment.h`中的宏值是否与`deployment.*`字段一致，实现C1–C10共10项检查。

- **arinc_compliance.py**：在去除C语言块注释和行注释后（`_strip_comments`预处理），检查`activity.c`中特定通信API的实际出现情况（而非仅检查名称字符串），实现R1–R8共8项规则。预处理步骤确保注释中的API关键词不会干扰检测结果。

### 4.2.2 命令行脚本

**evaluate.py — 单分区三维评分**

`evaluate.py`对指定目录中的生成代码执行三维评估，输出格式化报告。使用方式如下：

```bash
python DIMA/eval/evaluate.py \
    --generated DIMA/eval/generated/combined/pc \
    --partition pc \
    --label combined \
    --save
```

脚本调用`metrics/`中的三个模块，按权重（S:30%、A:40%、C:30%）计算加权综合得分，向终端输出带ANSI颜色的进度条式报告（得分≥80显示绿色，50–80显示黄色，<50显示红色），并在指定`--save`时将完整结果序列化为JSON文件保存到`results/`目录。综合得分低于40分时脚本以非零退出码退出，便于集成到CI流水线。

**batch_check.py — ARINC 653合规批量检查**

`batch_check.py`无需指定路径参数，自动遍历`generated/`目录下的全部16组实验结果，对每组调用`arinc_compliance.check()`，汇总输出合规检查表格。输出表格按策略分组，每行显示分区名称、通过/总计、合规率（色彩编码）和失败规则列表；表格底部汇总各策略的平均合规率。`--save`选项将完整的逐条规则结果写入`results/arinc_compliance.csv`。

### 4.2.3 图形化评估界面

针对评估工具可视化需求，本文在命令行脚本基础上实现了图形化评估界面（`gui.py`），使用Python标准库`tkinter+ttk`构建，无需额外安装第三方依赖：

```bash
python DIMA/eval/gui.py
```

界面采用`ttk.Notebook`多标签页设计，主窗口尺寸为1000×680像素，包含三个功能标签页。

**标签页一：单分区评估**

如图4-2所示，该页面分为左侧参数控制面板和右侧结果展示区。左侧控制面板提供：生成代码目录选择框（含"浏览"按钮）、分区选择下拉框（选项为pa/pb/pc/ps1–ps5）、实验标签输入框、保存JSON结果复选框，以及"开始评估"按钮。

右侧结果区在评估完成后更新显示：四个得分卡（分别展示结构完整性、API正确性、语义一致性和综合得分，每个得分卡含`ttk.Progressbar`进度条）；综合评级标签（优秀/良好/一般/较差）；检查明细树形表格（`ttk.Treeview`，按三个维度分组展示全部30个检查项，行背景色区分通过/部分通过/未通过三种状态）。

【图4-2：单分区评估界面截图 — 待插入（运行 python DIMA/eval/gui.py 后截图）】

**标签页二：ARINC 653合规检查**

如图4-3所示，该页面提供一键批量扫描功能，点击"开始检查"按钮即自动遍历全部16组实验结果。结果以`ttk.Treeview`表格呈现，按实验策略分组（分组行以蓝色背景突出显示），合规率颜色编码：100%绿色，75%–100%黄色，低于75%红色。

【图4-3：ARINC 653合规检查界面截图 — 待插入（运行 python DIMA/eval/gui.py 后截图）】

**标签页三：编译检查解析**

该页面用于解析编译检查结果文本文件，将原始编译器输出按策略和分区整理为结构化表格，列出错误数、警告数、通过状态以及各类错误的分类计数。

**线程安全设计**

为防止长时间评估任务阻塞界面，所有评估计算均在独立的后台线程（`threading.Thread(daemon=True)`）中执行；评估完成后通过`self.after(0, callback)`将UI更新回调调度到主线程执行，符合tkinter的单线程UI模型。

## 4.3 三维评分实验结果

### 4.3.1 各策略综合得分

表4-5汇总了四种提示策略在各分区的三维得分及综合得分。

| 策略   | 分区 | 结构(S) | API(A) | 语义(C) | 综合(T) |
|--------|------|---------|--------|---------|---------|
| 零样本 | ps1  | 100.0   | 100.0  | 81.2    | 94.4    |
| 零样本 | ps2  | 100.0   | 100.0  | 90.6    | 97.2    |
| 零样本 | ps3  | 100.0   | 100.0  | 81.2    | 94.4    |
| 零样本 | ps4  | 100.0   | 92.1   | 90.6    | 94.0    |
| 零样本 | ps5  | 100.0   | 100.0  | 90.6    | 97.2    |
| **零样本均值** | — | **100.0** | **98.4** | **86.8** | **95.4** |
| 思维链 | ps1  | 77.8    | 84.2   | 81.2    | 81.4    |
| 思维链 | ps2  | 77.8    | 100.0  | 81.2    | 87.7    |
| 思维链 | ps3  | 77.8    | 100.0  | 81.2    | 87.7    |
| 思维链 | ps4  | 77.8    | 100.0  | 81.2    | 87.7    |
| 思维链 | ps5  | 77.8    | 100.0  | 81.2    | 87.7    |
| **思维链均值** | — | **77.8** | **96.8** | **81.2** | **86.4** |
| 少样本 | PA   | 100.0   | 100.0  | 100.0   | 100.0   |
| 少样本 | PB   | 100.0   | 100.0  | 100.0   | 100.0   |
| 少样本 | PC   | 100.0   | 100.0  | 100.0   | 100.0   |
| **少样本均值** | — | **100.0** | **100.0** | **100.0** | **100.0** |
| 组合   | PA   | 100.0   | 100.0  | 100.0   | 100.0   |
| 组合   | PB   | 100.0   | 100.0  | 100.0   | 100.0   |
| 组合   | PC   | 100.0   | 100.0  | 100.0   | 100.0   |
| **组合均值** | — | **100.0** | **100.0** | **100.0** | **100.0** |

**表4-5 各策略三维评分结果**

### 4.3.2 结构完整性分析

零样本、少样本和组合三种策略的结构完整性均达到满分100.0，表明LLM在明确的文件清单提示下能够可靠地生成所有11个必需文件，并正确实现`appMain`函数、任务声明、头文件保护等基础结构。

思维链策略的结构完整性仅为77.8，原因在于所有5个分区均在S7和S8两项失分：`globals.h`中未定义`CHECK_CODE`宏，以及`gtypes.h`中缺少`typedef int integer`。分析CoT模板可知，其API参考部分聚焦于APEX调用签名，对`globals.h`和`gtypes.h`的具体内容描述不如零样本模板的文件清单明确，导致模型对这两个辅助文件的处理不够充分。

### 4.3.3 API正确性分析

API正确性是四种策略差异最显著的维度。零样本策略在ps4上出现A10失分（采样端口API方向匹配错误），其余均满分，平均98.4；思维链策略在ps1上同时出现A10和A11失分，平均96.8；少样本和组合策略均满分100.0。

思维链ps1的端口方向错误说明，即使提示词中包含完整的API参考，当分区资源组合非常复杂（ps1有4种资源类型共15个资源实例）时，模型仍可能在推理步骤3（分析任务通信行为）中产生混淆，将SOURCE端口的写操作误用为读操作。少样本策略通过具体代码示例规避了这一风险——示例中的端口使用模式为模型提供了直接可复用的代码模式，而非仅凭文字描述推断。

### 4.3.4 语义一致性分析

语义一致性是零样本和思维链策略的共同短板，均为81.2–90.6区间。主要失分点集中在C9和C10（`deployment.h`中的数量宏），部分分区中模型生成的`NB_THREADS`、`NB_QUEUEINGS`等宏值与规格不符，推测是模型将周期时间或其他数值误读为部署常量。

少样本和组合策略的语义一致性均达到满分100.0，这得益于ps3示例中展示了`deployment.h`的完整宏命名和取值约定，模型通过示例直接学习了从`deployment.nb_*`字段到C宏的映射规则。

### 4.3.5 综合得分对比

【图4-4：四种策略综合得分对比雷达图 — 见附件 DIMA/eval/results/figures/fig4_1_radar.png】

【图4-5：四种策略三维评分各维度详细对比 — 见附件 DIMA/eval/results/figures/fig4_3_dim_bar.png】

从综合得分来看，少样本（100.0）和组合（100.0）策略以满分领先，零样本（95.4）次之，思维链（86.4）最低。这一结果揭示了一个反直觉的规律：引入参考示例（少样本/组合）的提升效果远大于引入推理引导（CoT），而CoT策略甚至因为影响了辅助文件的生成质量而导致综合得分低于零样本。

该结论表明，对于ARINC 653这类规范约束密集、隐式约定多的领域代码生成任务，**提供高质量的代码示例是提升生成质量最有效的手段**；而链式推理在缺少示例的情况下，对提升较简单分区的效果有限，且可能对辅助文件的格式产生干扰[4][15][16]。

## 4.4 ARINC 653合规检查结果

### 4.4.1 各策略合规率汇总

表4-6给出了四种策略在16组实验中的ARINC 653合规检查结果。

| 策略   | 分区 | 通过/总计 | 合规率 | 未通过规则 |
|--------|------|-----------|--------|------------|
| 零样本 | ps1  | 4/6       | 66.7%  | R1（黑板未读写）、R2（缓冲区未收发） |
| 零样本 | ps2  | 4/6       | 66.7%  | R1（黑板未读写）、R2（缓冲区未收发） |
| 零样本 | ps3  | 7/7       | 100.0% | — |
| 零样本 | ps4  | 5/8       | 62.5%  | R5（队列SOURCE未发送）、R6（2个队列DEST未接收） |
| 零样本 | ps5  | 7/7       | 100.0% | — |
| **零样本均值** | — | — | **79.2%** | — |
| 思维链 | ps1  | 3/6       | 50.0%  | R2（缓冲区未收发）、R3（采样SOURCE未写）、R6（队列DEST未接收） |
| 思维链 | ps2  | 5/6       | 83.3%  | R2（缓冲区未收发） |
| 思维链 | ps3  | 7/7       | 100.0% | — |
| 思维链 | ps4  | 8/8       | 100.0% | — |
| 思维链 | ps5  | 7/7       | 100.0% | — |
| **思维链均值** | — | — | **86.7%** | — |
| 少样本 | PA   | 6/6       | 100.0% | — |
| 少样本 | PB   | 5/6       | 83.3%  | R1（黑板未读写） |
| 少样本 | PC   | 4/6       | 66.7%  | R1（黑板未读写）、R2（缓冲区未收发） |
| **少样本均值** | — | — | **83.3%** | — |
| 组合   | PA   | 6/6       | 100.0% | — |
| 组合   | PB   | 6/6       | 100.0% | — |
| 组合   | PC   | 5/6       | 83.3%  | R2（缓冲区未收发） |
| **组合均值** | — | — | **94.4%** | — |

**表4-6 各策略ARINC 653合规检查结果**

注：规则总数因分区资源组合不同而有所差异（ps3/ps5为7条，ps4为8条，其余为6条）。

### 4.4.2 典型问题分析

合规检查结果揭示了三类典型的代码质量问题，这些问题无法通过三维评分框架有效检测。

**问题一：资源完全闲置（零样本 ps1/ps2，R1+R2双失）**

在零样本策略生成的ps1和ps2代码中，`activity.c`的`extern`声明中出现了黑板和缓冲区的ID变量（如`extern BLACKBOARD_ID_TYPE ps1_bb_acc1_id`），导致三维框架的C6/C7检查通过；然而代码中完全没有`DISPLAY_BLACKBOARD`、`READ_BLACKBOARD`、`SEND_BUFFER`、`RECEIVE_BUFFER`等实际调用。这说明模型正确理解了资源需要声明，但遗漏了任务函数中对资源的实际读写操作。

该问题在ps1中同时影响5块黑板和3个缓冲区，是资源利用率最低的案例——资源在`appMain`中正常创建，在`activity.c`头部正常声明，但在任务的`while(1)`循环中从未被使用。

**问题二：系统性缺失缓冲区操作（思维链策略，R2持续失分）**

思维链策略在所有含缓冲区的分区（ps1、ps2）中均出现R2失分，表现为`SEND_BUFFER`或`RECEIVE_BUFFER`调用的系统性缺失。即使CoT推理步骤3要求模型"分析每个任务的通信行为"，模型仍倾向于遗漏缓冲区的双端操作——通常只实现写端（`SEND_BUFFER`）而遗漏读端（`RECEIVE_BUFFER`），或反之。

这一问题反映了缓冲区不同于黑板的FIFO语义（一次写对应一次读）在推理引导中难以被充分强调，而在有代码示例的情况下（少样本/组合），模型能够通过示例的API使用模式直接学习到双端操作的必要性。

**问题三：单端实现（少样本 PB/PC，R1失分）**

少样本策略在PB和PC中出现黑板单端实现问题：`activity.c`中存在`DISPLAY_BLACKBOARD`（写入）调用，但缺少`READ_BLACKBOARD`（读取）调用；或存在`READ_BLACKBOARD`但缺少`DISPLAY_BLACKBOARD`。这意味着模型在参考ps3示例时（ps3无黑板），对黑板的双端操作语义理解不完整，仅实现了一侧而遗漏另一侧。

组合策略在PB中修正了这一问题（通过在API参考部分显式展示`DISPLAY_BLACKBOARD`和`READ_BLACKBOARD`的配对使用），但在PC的缓冲区上仍出现单端实现，合规率83.3%。

### 4.4.3 合规检查与三维评分的对比分析

三维评分与ARINC 653合规检查在部分实验上出现分数背离，这种背离本身具有重要的诊断价值。

| 策略/分区     | 三维综合得分 | ARINC 653合规率 | 背离原因 |
|---------------|-------------|-----------------|----------|
| 零样本/ps1   | 94.4        | 66.7%           | C6/C7通过（名称出现在extern中），但R1/R2失败（从未实际调用API） |
| 少样本/PB    | 100.0       | 83.3%           | C6通过（黑板名出现），但R1失败（缺少READ_BLACKBOARD调用） |
| 思维链/ps1   | 81.4        | 50.0%           | A10/A11失分与R3/R6失分来自同一根本问题（端口API方向错误） |

**表4-7 三维评分与合规检查的典型背离案例**

零样本/ps1和少样本/PB的案例说明，三维评分中的语义一致性检查（C6/C7）存在一个设计局限：它检查资源名称的出现，而非资源的实际使用。ARINC 653合规检查正是为填补这一空白而设计的补充评估维度[19]。

思维链/ps1的案例则说明，当端口API方向错误时，三维评分的A10/A11和合规检查的R3/R6所捕获的是同一类错误，两者的失分具有一致性，相互印证了问题的真实存在。

### 4.4.4 综合评估结论

【图4-6：四种策略综合得分与合规率对比 — 见附件 DIMA/eval/results/figures/fig4_2_bar.png】

【图4-7：DIMA系统各分区合规率对比 — 见附件 DIMA/eval/results/figures/fig4_4_dima_compliance.png】

【图4-8：DIMA系统各分区三维综合得分 — 见附件 DIMA/eval/results/figures/fig4_5_dima_score.png】

【图4-9：IMA2系统各分区合规率对比 — 见附件 DIMA/eval/results/figures/fig4_6_ima2_compliance.png】

综合三维评分和ARINC 653合规检查结果，四种策略的整体质量排序为：

**组合（Combined）> 少样本（Few-Shot）> 零样本（Zero-Shot）> 思维链（CoT）**

组合策略在三维评分（均值100.0）和合规率（均值94.4%）两项指标上均位居首位，是本研究中质量最高的提示策略。其相对少样本策略的额外提升（合规率从83.3%升至94.4%）来自于API参考和推理引导对黑板双端操作理解的补强。

少样本策略三维评分满分，但合规率（83.3%）低于组合策略，说明参考示例的迁移效果受到示例覆盖资源类型的限制——ps3示例中无黑板和缓冲区，导致模型对这两类资源的双端操作语义理解不足。

零样本策略综合表现稳健（三维均值95.4%，合规率79.2%），在没有示例的条件下表现优于思维链，说明对于DIMA这类资源组合丰富的分区，简洁的提示加上完整的规格JSON足以引导模型生成主体正确的代码框架。

思维链策略因结构完整性的系统性失分（S7/S8在所有分区均失分），拉低了三维综合均值至86.4，低于零样本的95.4。尽管其API参考和推理链设计有助于模型理解端口语义，但对辅助文件格式的负面影响是CoT提示设计中需要改进的关键问题[4]。

---

# 第五章 总结与展望

## 5.1 研究工作总结

本文围绕"基于提示工程的ARINC 653 IMA系统C代码自动生成"这一核心问题，开展了系统性的研究工作，主要贡献如下：

**在工程工具方面**，本文开发了AADL解析工具`aadl2c.py`，实现了从系统架构模型到结构化规格JSON的自动转换；构建了DIMA和IMA2两套完整的IMA测试系统及其AADL模型；开发了自动化评估框架，支持三维评分、ARINC 653合规检查两个维度，并实现了包含命令行脚本（`evaluate.py`、`batch_check.py`）和图形化界面（`gui.py`）的完整工具链。

**在实验研究方面**，本文完成了四类提示策略在16组实验上的系统性对比，覆盖从最简（1任务1端口）到最复杂（3任务4类资源）的分区配置。实验揭示了各策略的适用场景与失效模式：少样本策略在格式固定的专业代码生成中效果最稳定；思维链策略对固定骨架代码的辅助文件生成具有负面影响；组合策略在所有评估维度上表现最全面。

**在评估方法方面**，本文提出了三维量化评估框架和ARINC 653合规检查的分层评估体系。通过少样本/PB分区"黑板创建未使用"的具体案例，证明了合规检查对三维评分的重要补充价值，为后续引入第四评估维度提供了实验依据。

## 5.2 主要结论

**结论一**：在四类提示策略中，组合策略（Few-Shot+CoT）通过了最完整的验证链，在三维评分（100.0）、ARINC 653合规检查（94.4%）两个层次均表现最佳，适合作为工程实践中ARINC 653代码生成的首选策略。

**结论二**：思维链策略在固定模板代码生成中的效果不及零样本策略，原因是思维链将模型的注意力引导到功能逻辑推理上，导致框架辅助文件（`globals.h`、`gtypes.h`、`deployment.h`）被忽略，结构完整性下降。

**结论三**：三维评分体系存在系统性盲区——它能确认资源被创建（API调用存在）和名称出现在代码中（语义一致），但无法确认资源在任务函数中被实际读写。ARINC 653合规检查是必要的补充手段。

**结论四**：结构化JSON输入的优势初步得到验证：将AADL的形式化模型转换为扁平JSON后，模型能够准确读取周期纳秒值、优先级等精确数值，而示例的作用部分在于示范正确的数值使用方式。

## 5.3 研究局限

**单一模型的局限性**：本文全部实验使用Claude claude-sonnet-4-6（claude-sonnet-4-6），结论的跨模型泛化性尚待通过多模型对比实验验证。

**运行时验证缺失**：受实验环境限制，未能在ACoreOS653或POK等真实ARINC 653 RTOS上运行生成代码，行为正确性的运行时验证是当前验证体系的主要缺口。

**测试系统规模有限**：DIMA和IMA2系统共8个分区，对更大规模IMA系统（如含10个以上分区、复杂分区间通信拓扑）的适用性有待进一步验证。

## 5.4 未来工作

**运行时验证**：计划在POK开源ARINC 653 RTOS或ACoreOS653真实环境上对生成代码进行运行时验证，测试分区调度、端口通信和健康监控回调的实际行为，构建完整的从代码生成到运行验证的端到端流水线。

**多模型对比**：计划扩展实验至GPT-4o、Gemini等主流模型，基于相同的提示策略和评估框架进行横向比较，分析不同模型在ARINC 653专业代码生成任务上的能力差异[15]。

**输入格式对比**：计划完成自然语言输入、AADL原文输入与结构化JSON输入的系统性对比实验，量化分析输入格式的结构化程度对生成代码质量的影响。

**评估体系扩展**：将ARINC 653合规检查正式纳入评估框架的第四维度，进一步完善分层评估体系，并探索将编译检查结果（错误数量和类型）量化为可比较的评分指标[19][20]。

---

## 参考文献

[1] Wei J, Wang X, Schuurmans D, et al. Chain-of-Thought Prompting Elicits Reasoning in Large Language Models[C]//Advances in Neural Information Processing Systems 35, 2022: 24824-24837.

[2] Kojima T, Gu S S, Reid M, et al. Large Language Models are Zero-Shot Reasoners[C]//Advances in Neural Information Processing Systems 35, 2022: 22199-22213.

[3] Wang L, Xu W, Lan Y, et al. Plan-and-Solve Prompting: Improving Zero-Shot Chain-of-Thought Reasoning by Large Language Models[C]//Proceedings of the 61st Annual Meeting of the Association for Computational Linguistics, 2023: 2609-2634.

[4] Li J, Li G, Li Y, et al. Structured Chain-of-Thought Prompting for Code Generation[J]. ACM Transactions on Software Engineering and Methodology, 2025, 34(2).

[5] Lukić B, Friedrich S, Schubert T, et al. Automated Configuration of ARINC 653-Compliant Avionics Architectures[C]//AIAA/IEEE Digital Avionics Systems Conference (DASC), 2018.

[6] Lukić B, et al. A Streamlined Approach Toward Automated Generation and Validation of ARINC 653-Compliant Avionics Code[C]//AIAA/IEEE Digital Avionics Systems Conference (DASC), 2019.

[7] Hugues J, Delange J. Model-Based Design and Automated Validation of ARINC653 Architectures Using the AADL[M]//Cyber-Physical Systems: Foundations, Principles and Applications. Elsevier, 2017.

[8] AADL653: An AADL-Based Formal Language for Modeling and Validation of ARINC653-Based Avionics Software[C]//IEEE, 2017.

[9] Englhardt Z, Li R, Nissanka D, et al. Exploring and Characterizing Large Language Models for Embedded System Development[C]//Proceedings of the 61st Design Automation Conference, 2024.

[10] Fakih M, Dharmaji R, Moghaddas Y, et al. LLM4PLC: Harnessing Large Language Models for Verifiable Programming of PLCs in Industrial Control Systems[C]//ICSE, 2024.

[11] Babiuch M, Smutný P. Benchmarking Large Language Models for Embedded Systems Programming in Microcontroller-Driven Applications[J]. Electronics, 2024.

[12] Using Large Language Models for Aerospace Code Generation: Methods, Benchmarks, and Potential Values[J]. Aerospace, 2024.

[13] Jiang J, Wang F, Shen J, et al. A Survey on Large Language Models for Code Generation[J]. ACM Transactions on Software Engineering and Methodology, 2024.

[14] 陈娟. ARINC653分区操作系统在综合模块化航空电子系统中的应用[J]. 电信工程技术与标准化, 2009, 49(5): 1-4.

[15] Gu X, Chen M, Lin Y, et al. On the Effectiveness of Large Language Models in Domain-Specific Code Generation[J]. ACM Transactions on Software Engineering and Methodology, 2025, 34(3).

[16] Tony C, Díaz Ferreyra N E, Mutas M, et al. Prompting Techniques for Secure Code Generation: A Systematic Investigation[J]. ACM Transactions on Software Engineering and Methodology, 2025, 34(8).

[17] Li J, Li G, Li Y, et al. Large Language Model-Aware In-Context Learning for Code Generation[J]. ACM Transactions on Software Engineering and Methodology, 2025, 34(7).

[18] Weyssow M, Sahraoui H, Zhou X, et al. Exploring Parameter-Efficient Fine-Tuning Techniques for Code Generation with Large Language Models[J]. ACM Transactions on Software Engineering and Methodology, 2025, 34(7).

[19] Dong Y, Ding J, Jiang X, et al. CodeScore: Evaluating Code Generation by Learning Code Execution[J]. ACM Transactions on Software Engineering and Methodology, 2025, 34(3).

[20] Yang Z, Chen S, Gao C, et al. An Empirical Study of Retrieval-Augmented Code Generation: Challenges and Opportunities[J]. ACM Transactions on Software Engineering and Methodology, 2025, 34(7).

---

## 在学期间取得的主要成果

【请根据实际情况填写】

---

## 致　　谢

【请根据实际情况填写】
