#!/usr/bin/env python3
"""
论文图表生成脚本

生成以下图表（保存到 results/figures/）：
  fig4_1_radar.png    — 四种策略三维+合规率雷达图
  fig4_2_bar.png      — 综合得分与合规率对比柱状图
  fig4_3_dim_bar.png  — 各维度得分分组柱状图
  fig4_4_compliance_line.png — 分区合规率折线图（DIMA策略）
  fig4_5_partition_score.png — 分区综合得分折线图（DIMA策略）

用法：
    python DIMA/eval/plot_results.py
"""

import os
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib import rcParams

# ── 字体配置 ──────────────────────────────────────────────────────────────────
rcParams['font.family'] = 'WenQuanYi Zen Hei'
rcParams['axes.unicode_minus'] = False
rcParams['figure.dpi'] = 150

OUT_DIR = os.path.join(os.path.dirname(__file__), "results", "figures")
os.makedirs(OUT_DIR, exist_ok=True)

# ── 颜色方案 ──────────────────────────────────────────────────────────────────
COLORS = {
    "零样本":  "#2E86AB",
    "思维链":  "#E07A5F",
    "少样本":  "#3D9970",
    "组合":    "#8B5CF6",
}
STRATEGY_ORDER = ["零样本", "思维链", "少样本", "组合"]

# ── 实验数据 ──────────────────────────────────────────────────────────────────
# 三维均值得分
SCORES = {
    "零样本": {"结构": 100.0, "API": 98.4, "语义": 86.8, "综合": 95.4},
    "思维链": {"结构": 77.8,  "API": 96.8, "语义": 81.2, "综合": 86.4},
    "少样本": {"结构": 100.0, "API": 100.0, "语义": 100.0, "综合": 100.0},
    "组合":   {"结构": 100.0, "API": 100.0, "语义": 100.0, "综合": 100.0},
}

# ARINC 653 合规率均值
COMPLIANCE = {
    "零样本": 79.2,
    "思维链": 86.7,
    "少样本": 83.3,
    "组合":   94.4,
}

# DIMA 分区逐个合规率（零样本 & 思维链）
COMPLIANCE_DIMA = {
    "零样本": {"ps1": 66.7, "ps2": 66.7, "ps3": 100.0, "ps4": 62.5, "ps5": 100.0},
    "思维链": {"ps1": 50.0, "ps2": 83.3, "ps3": 100.0, "ps4": 100.0, "ps5": 100.0},
}

# DIMA 分区综合得分（零样本 & 思维链）
TOTAL_DIMA = {
    "零样本": {"ps1": 94.4, "ps2": 97.2, "ps3": 94.4, "ps4": 94.0, "ps5": 97.2},
    "思维链": {"ps1": 81.4, "ps2": 87.7, "ps3": 87.7, "ps4": 87.7, "ps5": 87.7},
}

# IMA2 分区合规率（少样本 & 组合）
COMPLIANCE_IMA2 = {
    "少样本": {"PA": 100.0, "PB": 83.3, "PC": 66.7},
    "组合":   {"PA": 100.0, "PB": 100.0, "PC": 83.3},
}

# ─────────────────────────────────────────────────────────────────────────────
# 图4-1  雷达图：四维指标对比
# ─────────────────────────────────────────────────────────────────────────────
def plot_radar():
    categories = ["结构完整性", "API正确性", "语义一致性", "ARINC 653\n合规率"]
    N = len(categories)
    angles = [n / float(N) * 2 * np.pi for n in range(N)]
    angles += angles[:1]

    fig, ax = plt.subplots(figsize=(6, 5.5),
                           subplot_kw=dict(polar=True))
    ax.set_theta_offset(np.pi / 2)
    ax.set_theta_direction(-1)

    ax.set_xticks(angles[:-1])
    ax.set_xticklabels(categories, size=10.5)
    ax.set_ylim(0, 100)
    ax.set_yticks([20, 40, 60, 80, 100])
    ax.set_yticklabels(["20", "40", "60", "80", "100"], size=8, color="#888")
    ax.yaxis.set_tick_params(labelsize=8)
    ax.tick_params(axis='x', pad=12)

    for strategy in STRATEGY_ORDER:
        s = SCORES[strategy]
        vals = [s["结构"], s["API"], s["语义"], COMPLIANCE[strategy]]
        vals += vals[:1]
        clr = COLORS[strategy]
        ax.plot(angles, vals, linewidth=2, color=clr, label=strategy)
        ax.fill(angles, vals, alpha=0.12, color=clr)

    ax.legend(loc="upper right", bbox_to_anchor=(1.38, 1.18),
              framealpha=0.9, fontsize=10)
    ax.set_title("四种提示策略四维指标对比", size=12, pad=20, fontweight='bold')
    ax.grid(color='#ccc', linewidth=0.6)

    path = os.path.join(OUT_DIR, "fig4_1_radar.png")
    fig.savefig(path, dpi=200, bbox_inches="tight", facecolor="white")
    plt.close(fig)
    print(f"  保存: {path}")


# ─────────────────────────────────────────────────────────────────────────────
# 图4-2  柱状图：综合得分与合规率对比
# ─────────────────────────────────────────────────────────────────────────────
def plot_score_compliance_bar():
    strategies = STRATEGY_ORDER
    x = np.arange(len(strategies))
    width = 0.32

    totals     = [SCORES[s]["综合"]  for s in strategies]
    compliance = [COMPLIANCE[s]      for s in strategies]

    fig, ax = plt.subplots(figsize=(7, 4.5))

    bars1 = ax.bar(x - width/2, totals,     width, label="三维综合得分",
                   color=[COLORS[s] for s in strategies], alpha=0.88,
                   edgecolor='white', linewidth=0.8)
    bars2 = ax.bar(x + width/2, compliance, width, label="ARINC 653合规率",
                   color=[COLORS[s] for s in strategies], alpha=0.45,
                   edgecolor=[COLORS[s] for s in strategies], linewidth=1.5,
                   hatch="///")

    # 数值标签
    for bar in bars1:
        h = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2, h + 0.6,
                f"{h:.1f}", ha='center', va='bottom', fontsize=9.5)
    for bar in bars2:
        h = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2, h + 0.6,
                f"{h:.1f}%", ha='center', va='bottom', fontsize=9.5)

    ax.set_xticks(x)
    ax.set_xticklabels(strategies, fontsize=11)
    ax.set_ylim(0, 115)
    ax.set_ylabel("得分 / 合规率（%）", fontsize=10.5)
    ax.set_title("四种提示策略综合得分与ARINC 653合规率对比", fontsize=12, fontweight='bold', pad=10)
    ax.legend(fontsize=10, loc="upper left")
    ax.yaxis.grid(True, linestyle='--', alpha=0.5)
    ax.set_axisbelow(True)
    ax.spines[['top', 'right']].set_visible(False)

    path = os.path.join(OUT_DIR, "fig4_2_bar.png")
    fig.savefig(path, dpi=200, bbox_inches="tight", facecolor="white")
    plt.close(fig)
    print(f"  保存: {path}")


# ─────────────────────────────────────────────────────────────────────────────
# 图4-3  分组柱状图：三个维度细分得分
# ─────────────────────────────────────────────────────────────────────────────
def plot_dimension_bar():
    dims = ["结构完整性\n(权重30%)", "API正确性\n(权重40%)", "语义一致性\n(权重30%)", "综合得分"]
    dim_keys = ["结构", "API", "语义", "综合"]
    strategies = STRATEGY_ORDER
    x = np.arange(len(dims))
    n = len(strategies)
    width = 0.18
    offsets = np.linspace(-(n-1)/2*width, (n-1)/2*width, n)

    fig, ax = plt.subplots(figsize=(8.5, 4.8))

    for i, strategy in enumerate(strategies):
        vals = [SCORES[strategy][k] for k in dim_keys]
        bars = ax.bar(x + offsets[i], vals, width,
                      label=strategy, color=COLORS[strategy],
                      alpha=0.88, edgecolor='white', linewidth=0.6)
        for bar, v in zip(bars, vals):
            if v < 100:
                ax.text(bar.get_x() + bar.get_width()/2, v + 0.5,
                        f"{v:.1f}", ha='center', va='bottom',
                        fontsize=7.5, color=COLORS[strategy])

    ax.set_xticks(x)
    ax.set_xticklabels(dims, fontsize=10.5)
    ax.set_ylim(0, 115)
    ax.set_ylabel("得分", fontsize=10.5)
    ax.set_title("四种提示策略三维评分细分对比", fontsize=12, fontweight='bold', pad=10)
    ax.legend(fontsize=10, loc="lower right")
    ax.yaxis.grid(True, linestyle='--', alpha=0.45)
    ax.set_axisbelow(True)
    ax.spines[['top', 'right']].set_visible(False)
    ax.axhline(100, color='#aaa', linewidth=0.8, linestyle=':')

    path = os.path.join(OUT_DIR, "fig4_3_dim_bar.png")
    fig.savefig(path, dpi=200, bbox_inches="tight", facecolor="white")
    plt.close(fig)
    print(f"  保存: {path}")


# ─────────────────────────────────────────────────────────────────────────────
# 图4-4  折线图：DIMA各分区合规率（零样本 vs 思维链）
# ─────────────────────────────────────────────────────────────────────────────
def plot_dima_compliance_line():
    partitions = ["ps1", "ps2", "ps3", "ps4", "ps5"]

    fig, ax = plt.subplots(figsize=(7, 4.2))

    for strategy in ["零样本", "思维链"]:
        vals = [COMPLIANCE_DIMA[strategy][p] for p in partitions]
        clr  = COLORS[strategy]
        ax.plot(partitions, vals, marker='o', markersize=7,
                linewidth=2, color=clr, label=strategy)
        for p, v in zip(partitions, vals):
            offset = 3.5 if strategy == "零样本" else -8
            va     = 'bottom' if strategy == "零样本" else 'top'
            ax.annotate(f"{v:.0f}%", (p, v),
                        textcoords="offset points", xytext=(0, offset),
                        ha='center', va=va, fontsize=9, color=clr)

    ax.set_ylim(35, 115)
    ax.set_ylabel("ARINC 653合规率（%）", fontsize=10.5)
    ax.set_xlabel("DIMA分区", fontsize=10.5)
    ax.set_title("DIMA系统各分区ARINC 653合规率（零样本 vs 思维链）",
                 fontsize=11.5, fontweight='bold', pad=10)
    ax.legend(fontsize=10)
    ax.yaxis.grid(True, linestyle='--', alpha=0.45)
    ax.set_axisbelow(True)
    ax.spines[['top', 'right']].set_visible(False)
    ax.axhline(100, color='#3D9970', linewidth=0.8, linestyle=':', alpha=0.6)

    path = os.path.join(OUT_DIR, "fig4_4_dima_compliance.png")
    fig.savefig(path, dpi=200, bbox_inches="tight", facecolor="white")
    plt.close(fig)
    print(f"  保存: {path}")


# ─────────────────────────────────────────────────────────────────────────────
# 图4-5  折线图：DIMA各分区综合得分（零样本 vs 思维链）
# ─────────────────────────────────────────────────────────────────────────────
def plot_dima_score_line():
    partitions = ["ps1", "ps2", "ps3", "ps4", "ps5"]

    fig, ax = plt.subplots(figsize=(7, 4.2))

    for strategy in ["零样本", "思维链"]:
        vals = [TOTAL_DIMA[strategy][p] for p in partitions]
        clr  = COLORS[strategy]
        ax.plot(partitions, vals, marker='s', markersize=7,
                linewidth=2, color=clr, label=strategy)
        for p, v in zip(partitions, vals):
            offset = 3.5 if strategy == "零样本" else -9
            va     = 'bottom' if strategy == "零样本" else 'top'
            ax.annotate(f"{v:.1f}", (p, v),
                        textcoords="offset points", xytext=(0, offset),
                        ha='center', va=va, fontsize=9, color=clr)

    ax.set_ylim(70, 108)
    ax.set_ylabel("三维综合得分", fontsize=10.5)
    ax.set_xlabel("DIMA分区", fontsize=10.5)
    ax.set_title("DIMA系统各分区三维综合得分（零样本 vs 思维链）",
                 fontsize=11.5, fontweight='bold', pad=10)
    ax.legend(fontsize=10)
    ax.yaxis.grid(True, linestyle='--', alpha=0.45)
    ax.set_axisbelow(True)
    ax.spines[['top', 'right']].set_visible(False)

    path = os.path.join(OUT_DIR, "fig4_5_dima_score.png")
    fig.savefig(path, dpi=200, bbox_inches="tight", facecolor="white")
    plt.close(fig)
    print(f"  保存: {path}")


# ─────────────────────────────────────────────────────────────────────────────
# 图4-6  柱状图：IMA2各分区合规率（少样本 vs 组合）
# ─────────────────────────────────────────────────────────────────────────────
def plot_ima2_compliance_bar():
    partitions = ["PA", "PB", "PC"]
    x = np.arange(len(partitions))
    width = 0.32

    fig, ax = plt.subplots(figsize=(5.5, 4.2))

    for i, strategy in enumerate(["少样本", "组合"]):
        vals    = [COMPLIANCE_IMA2[strategy][p] for p in partitions]
        offset  = -width/2 if i == 0 else width/2
        bars    = ax.bar(x + offset, vals, width,
                         label=strategy, color=COLORS[strategy],
                         alpha=0.88, edgecolor='white', linewidth=0.8)
        for bar, v in zip(bars, vals):
            ax.text(bar.get_x() + bar.get_width()/2, v + 1,
                    f"{v:.0f}%", ha='center', va='bottom',
                    fontsize=10.5, color=COLORS[strategy], fontweight='bold')

    ax.set_xticks(x)
    ax.set_xticklabels([f"IMA2\n{p}（复杂度{'低' if p=='PA' else '中' if p=='PB' else '高'}）"
                        for p in partitions], fontsize=10)
    ax.set_ylim(0, 118)
    ax.set_ylabel("ARINC 653合规率（%）", fontsize=10.5)
    ax.set_title("IMA2系统各分区ARINC 653合规率\n（少样本 vs 组合）",
                 fontsize=11.5, fontweight='bold', pad=10)
    ax.legend(fontsize=10)
    ax.yaxis.grid(True, linestyle='--', alpha=0.45)
    ax.set_axisbelow(True)
    ax.spines[['top', 'right']].set_visible(False)

    path = os.path.join(OUT_DIR, "fig4_6_ima2_compliance.png")
    fig.savefig(path, dpi=200, bbox_inches="tight", facecolor="white")
    plt.close(fig)
    print(f"  保存: {path}")


# ─────────────────────────────────────────────────────────────────────────────
# 主程序
# ─────────────────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    print("生成论文图表...")
    plot_radar()
    plot_score_compliance_bar()
    plot_dimension_bar()
    plot_dima_compliance_line()
    plot_dima_score_line()
    plot_ima2_compliance_bar()
    print(f"\n全部完成，图片保存于: {OUT_DIR}/")
