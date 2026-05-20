# DIMA 评估工具使用说明

所有命令均在项目根目录 `graduatedesign/` 下运行。

---

## 目录结构

```
DIMA/eval/
├── scripts/
│   └── aadl2c.py              # AADL → JSON 规格转换
├── prompts/                   # 提示词模板（纯提示词，无说明）
│   ├── zeroshot_template.md
│   ├── cot_template.md
│   ├── fewshot_template.md
│   ├── combined_template.md
│   └── ...
├── specs/                     # 各分区 JSON 规格
├── generated/                 # LLM 生成的 C 代码
├── results/                   # 评估结果 JSON + 图表
├── metrics/                   # 评估模块（不直接调用）
│   ├── structural.py
│   ├── api_check.py
│   ├── semantic.py
│   └── arinc_compliance.py
├── evaluate.py                # 单分区三维评分
├── batch_check.py             # 批量 ARINC 653 合规检查
├── compare.py                 # 多实验结果对比
├── score_format_cmp.py        # 输入格式对比评分
├── save_response.py           # 解析 Claude 网页回复 → 11 个文件
├── parse_compile.py           # 解析编译检查结果
├── plot_results.py            # 生成论文实验结果图表
├── plot_pipeline.py           # 生成方法总体流程图
├── plot_ima3_topology.py      # 生成 IMA3 系统拓扑图
└── gui.py                     # 图形化评估界面
```

---

## 评估脚本

### evaluate.py — 单分区三维评分

```bash
python3 DIMA/eval/evaluate.py --generated <生成代码目录> --partition <psX>

# 示例
python3 DIMA/eval/evaluate.py --generated DIMA/eval/generated/zeroshot/ps2 --partition ps2
python3 DIMA/eval/evaluate.py --generated DIMA/eval/generated/cot/ps1 --partition ps1 --save
```

输出：结构S / API A / 语义C 三维得分及综合得分。`--save` 将结果写入 `results/`。

---

### batch_check.py — 批量 ARINC 653 合规检查

```bash
python3 DIMA/eval/batch_check.py
python3 DIMA/eval/batch_check.py --verbose   # 显示每条规则详情
python3 DIMA/eval/batch_check.py --save      # 保存 CSV 报告
```

对 `generated/` 下所有已完成实验运行 R1–R8 合规规则，输出汇总表。

---

### compare.py — 多实验结果对比

```bash
# 对比同一分区的四种策略
python3 DIMA/eval/compare.py --dir DIMA/eval/results/ --partition ps2

# 生成 CSV（可导入 Excel）
python3 DIMA/eval/compare.py --dir DIMA/eval/results/ --csv output.csv
```

---

### score_format_cmp.py — 输入格式对比评分

```bash
python3 DIMA/eval/score_format_cmp.py
```

对 `generated/format_cmp/{nl,aadl,json}/ps2/` 三个目录评估，打印对比表，结果保存至 `results/format_cmp_results.json`。

前置条件：三个目录下已有生成的 C/H 文件。

---

### save_response.py — 解析 Claude 网页回复

将 Claude 网页端回复文本保存为 `.txt` 后运行：

```bash
# 处理全部三种格式
python3 DIMA/eval/save_response.py

# 只处理指定格式
python3 DIMA/eval/save_response.py --fmt nl
python3 DIMA/eval/save_response.py --fmt aadl
python3 DIMA/eval/save_response.py --fmt json
```

输入：`responses/{nl,aadl,json}_ps2.txt`  
输出：`generated/format_cmp/{nl,aadl,json}/ps2/` 下的 11 个文件

---

### parse_compile.py — 编译检查结果解析

```bash
python3 DIMA/eval/parse_compile.py
python3 DIMA/eval/parse_compile.py --input my_results.txt
python3 DIMA/eval/parse_compile.py --verbose    # 显示每条错误详情
python3 DIMA/eval/parse_compile.py --reports    # 生成各分区独立报告
```

---

## 生图脚本

所有图表保存至 `DIMA/eval/results/figures/`。

### plot_results.py — 论文实验结果图表

```bash
python3 DIMA/eval/plot_results.py
```

生成：
- `fig4_1_radar.png` — 四种策略三维+合规率雷达图
- `fig4_2_bar.png` — 综合得分与合规率对比柱状图
- `fig4_3_dim_bar.png` — 各维度得分分组柱状图
- `fig4_4_compliance_line.png` — 分区合规率折线图
- `fig4_5_partition_score.png` — 分区综合得分折线图

---

### plot_pipeline.py — 方法总体流程图

```bash
python3 DIMA/eval/plot_pipeline.py
```

生成：`results/figures/fig3_pipeline.png`  
内容：AADL → aadl2c.py → 四种策略 → LLM → 三维评分 + ARINC 653 合规检查

---

### plot_ima3_topology.py — IMA3 系统拓扑图

```bash
python3 DIMA/eval/plot_ima3_topology.py
```

生成：`results/figures/fig3_ima3_topology.png`  
内容：NAV / DISP / MON / CTRL 四分区端口连接拓扑

---

## 图形化界面

### gui.py — 评估 GUI

```bash
python3 DIMA/eval/gui.py
```

提供可视化的评估操作界面，功能等同于 `evaluate.py`。

---

## 规格转换工具

### scripts/aadl2c.py — AADL → JSON 规格提取

```bash
python3 DIMA/scripts/aadl2c.py --partition P2 \
    --partitions DIMA/Sys/DIMA_partitions.aadl \
    --threads    DIMA/Sys/DIMA_threads.aadl \
    --output     DIMA/eval/specs/ps2.json
```

从 AADL 模型文件中提取指定分区的规格，生成标准化 JSON 文件供评估脚本使用。
