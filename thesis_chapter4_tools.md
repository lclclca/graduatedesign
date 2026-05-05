## 4.2 评估工具实现

基于第4.1节设计的评估框架，本文实现了一套自动化评估工具，包括命令行脚本和图形化界面两个层次。工具采用Python编写，核心评估逻辑封装在`DIMA/eval/metrics/`包中，上层脚本和GUI通过模块导入复用同一套计算逻辑，确保命令行与界面输出的一致性。整体架构如图4-3所示。

【图4-3 评估工具架构图 — 待插入】

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

- **arinc_compliance.py**：在去除C语言块注释和行注释后，检查`activity.c`中特定通信API的出现情况（而非仅检查名称字符串），实现R1–R8共8项规则。该模块对代码进行预处理（`_strip_comments`），避免注释中的关键词干扰检测结果。

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

脚本调用`metrics/`中的三个模块，按权重（S:30%、A:40%、C:30%）计算加权综合得分，向终端输出带ANSI颜色的进度条式报告（得分≥80显示绿色，50–80显示黄色，<50显示红色），并在指定`--save`时将完整结果（包含每个检查项的`passed`/`partial`/`note`）序列化为JSON文件保存到`results/`目录。综合得分低于40分时脚本以非零退出码退出，便于集成到CI流水线。

**batch_check.py — ARINC 653合规批量检查**

`batch_check.py`无需指定路径参数，自动遍历`generated/`目录下的全部16组实验结果，对每组调用`arinc_compliance.check()`，汇总输出合规检查表格：

```bash
python DIMA/eval/batch_check.py --verbose   # 显示每条规则详情
python DIMA/eval/batch_check.py --save      # 保存CSV报告
```

输出表格按策略分组，每行显示分区名称、通过/总计、合规率（色彩编码）和失败规则列表；表格底部汇总各策略的平均合规率。`--save`选项将完整的逐条规则结果写入`results/arinc_compliance.csv`，便于进一步分析。

### 4.2.3 图形化评估界面

针对指导教师对"工具可视化"的要求，本文在命令行脚本基础上实现了图形化评估界面（`gui.py`），使用Python标准库`tkinter`+`ttk`构建，无需额外安装第三方依赖，可通过以下命令直接启动：

```bash
python DIMA/eval/gui.py
```

界面采用`ttk.Notebook`多标签页设计，主窗口尺寸为1000×680像素，包含三个功能标签页，分别对应三类评估任务。

**标签页一：单分区评估**

如图4-4所示【图4-4 单分区评估界面截图 — 待插入】，该页面分为左侧参数控制面板和右侧结果展示区。

左侧控制面板提供：生成代码目录选择框（含"浏览"按钮，调用`filedialog.askdirectory()`）、分区选择下拉框（`ttk.Combobox`，选项为pa/pb/pc/ps1–ps5）、实验标签输入框（可选，用于结果文件命名）、保存JSON结果复选框，以及"开始评估"按钮。

右侧结果区在评估完成后更新显示：
- **四个得分卡**：分别展示结构完整性（权重30%）、API正确性（权重40%）、语义一致性（权重30%）和综合得分，每个得分卡包含`ttk.Progressbar`进度条和数字分数标签，分数颜色随得分区间变化（≥80绿色，50–80橙色，<50红色）；
- **综合评级标签**：以文字描述当前代码质量等级（优秀/良好/一般/较差）；
- **检查明细树形表格**（`ttk.Treeview`）：按三个维度分组展示全部30个检查项，每行显示检查项编号、描述、通过标记（✓/△/✗）、得分百分比和备注信息，行背景色区分通过（绿色）、部分通过（黄色）和未通过（红色）三种状态。

**标签页二：ARINC 653合规检查**

如图4-5所示【图4-5 ARINC 653合规检查界面截图 — 待插入】，该页面提供一键批量扫描功能，点击"开始检查"按钮即自动遍历全部16组实验结果，无需手动指定路径。

结果以`ttk.Treeview`表格呈现，按实验策略分组（分组行以蓝色背景突出显示），每个实验分区占一行，展示通过/总计数量、合规率百分比和失败规则列表；合规率100%的行显示绿色背景，75%–100%显示黄色，低于75%显示红色。页面底部汇总显示各策略的平均合规率（带颜色编码）。勾选"保存CSV"复选框后，检查结果将自动写入`results/arinc_compliance.csv`。

**标签页三：编译检查解析**

该页面用于解析编译检查结果文本文件（`compile_check_results.txt`），将原始的编译器输出按策略和分区整理为结构化表格，列出错误数、警告数、通过状态以及各类错误（语法错误、未声明标识符、类型错误等）的分类计数。表格支持水平滚动（`xscrollcommand`），以容纳错误分类的多列数据。

**线程安全设计**

为防止长时间评估任务阻塞界面导致窗口无响应，所有评估计算均在独立的后台线程（`threading.Thread(daemon=True)`）中执行；评估完成后通过`self.after(0, callback)`将UI更新回调调度到主线程执行，符合tkinter的单线程UI模型。在评估期间，"开始"按钮被设置为禁用状态（`btn.state(["disabled"])`），防止重复触发；评估结束或异常后自动恢复。同时对评估模块中的`sys.exit()`调用进行捕获（`except SystemExit`），防止规格文件缺失时导致界面崩溃。
