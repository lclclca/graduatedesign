#!/usr/bin/env python3
"""方法总体流程图 — AADL → 提示策略 → LLM → 双维评估"""

import os
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch
from matplotlib import rcParams

rcParams['font.family'] = 'WenQuanYi Zen Hei'
rcParams['axes.unicode_minus'] = False

OUT_DIR = os.path.join(os.path.dirname(__file__), "results", "figures")
os.makedirs(OUT_DIR, exist_ok=True)

# ── 配色 ─────────────────────────────────────────────────────────────────────
C_AADL_E  = "#2E4057";  C_AADL_F  = "#D6EAF8"
C_TOOL_E  = "#1A5276";  C_TOOL_F  = "#EBF5FB"
C_PRM_E   = "#1F618D";  C_PRM_F   = "#EBF5FB"
C_LLM_E   = "#4A235A";  C_LLM_F   = "#E8DAEF"
C_EVAL_E  = "#1A5276";  C_EVAL_F  = "#D6EAF8"
C_ARINC_E = "#922B21";  C_ARINC_F = "#FADBD8"
C_RES_E   = "#145A32";  C_RES_F   = "#D5F5E3"
C_RES2_E  = "#7B241C";  C_RES2_F  = "#FDEDEC"
C_SUB_E   = "#AAAAAA";  C_SUB_F   = "#FDFEFE"
C_ARR     = "#555555"
C_PHASE   = "#999999"

fig, ax = plt.subplots(figsize=(12, 12))
ax.set_xlim(0, 12)
ax.set_ylim(4.7, 14.5)
ax.axis('off')
fig.patch.set_facecolor('white')


# ── 辅助函数 ──────────────────────────────────────────────────────────────────
def box(ax, x, y, w, h, title, ec, fc, title_h=0.55, fs=10, fw='bold', tc='white',
        subtitle=None, sub_fs=8.5):
    """圆角矩形，带标题栏（深色顶条 + 内容区）"""
    ax.add_patch(FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.06",
                                lw=1.6, edgecolor=ec, facecolor=fc, zorder=2))
    # 标题顶栏
    ax.add_patch(FancyBboxPatch((x, y+h-title_h), w, title_h,
                                boxstyle="round,pad=0.03",
                                lw=0, edgecolor='none', facecolor=ec, zorder=3))
    ax.text(x + w/2, y + h - title_h/2, title,
            ha='center', va='center', fontsize=fs, fontweight=fw,
            color=tc, zorder=4)
    if subtitle:
        ax.text(x + w/2, y + (h - title_h)/2, subtitle,
                ha='center', va='center', fontsize=sub_fs,
                color='#444444', zorder=4)


def simple_box(ax, x, y, w, h, text, ec, fc, fs=8.5, fw='normal'):
    """无标题栏的简单框"""
    ax.add_patch(FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.05",
                                lw=1.0, edgecolor=ec, facecolor=fc, zorder=3))
    ax.text(x + w/2, y + h/2, text,
            ha='center', va='center', fontsize=fs, fontweight=fw,
            color='#333333', zorder=4)


def arrow(ax, x1, y1, x2, y2, color=C_ARR, lw=1.8):
    ax.annotate("", xy=(x2, y2), xytext=(x1, y1),
                arrowprops=dict(arrowstyle="-|>", color=color, lw=lw,
                                connectionstyle="arc3,rad=0.0"),
                zorder=5)


def fan_arrows(ax, x_src, y_src, targets_x, y_dst, color=C_ARR):
    """从单点扇出到多个目标点"""
    for tx in targets_x:
        ax.annotate("", xy=(tx, y_dst), xytext=(x_src, y_src),
                    arrowprops=dict(arrowstyle="-|>", color=color, lw=1.6,
                                    connectionstyle="arc3,rad=0.0"),
                    zorder=5)


def fan_in_arrows(ax, sources_x, y_src, x_dst, y_dst, color=C_ARR):
    """从多个源点汇聚到单点"""
    for sx in sources_x:
        ax.annotate("", xy=(x_dst, y_dst), xytext=(sx, y_src),
                    arrowprops=dict(arrowstyle="-|>", color=color, lw=1.6,
                                    connectionstyle="arc3,rad=0.0"),
                    zorder=5)


# ── 阶段标签（左侧）──────────────────────────────────────────────────────────
def phase_label(ax, y_mid, text):
    ax.text(0.22, y_mid, text, ha='center', va='center',
            fontsize=8.5, color=C_PHASE, rotation=90,
            fontstyle='italic', zorder=1)


# ════════════════════════════════════════════════════════════════════════════
#  第1层：AADL模型文件
# ════════════════════════════════════════════════════════════════════════════
BW1 = 4.5;  BX1 = (12 - BW1) / 2    # 居中
BY1 = 13.3;  BH1 = 0.75
simple_box(ax, BX1, BY1, BW1, BH1, "AADL 系统架构模型",
           C_AADL_E, C_AADL_F, fs=10, fw='bold')
phase_label(ax, BY1 + BH1/2, "输  入")

# ════════════════════════════════════════════════════════════════════════════
#  第2层：aadl2c.py 解析器
# ════════════════════════════════════════════════════════════════════════════
BW2 = 5.2;  BX2 = (12 - BW2) / 2
BY2 = 12.05;  BH2 = 0.75
simple_box(ax, BX2, BY2, BW2, BH2, "aadl2c.py   AADL 解析与规格提取",
           C_TOOL_E, C_TOOL_F, fs=9.5, fw='bold')

phase_label(ax, BY2 + BH2/2, "解  析")

# ── 箭头 1→2 ─────────────────────────────────────────────────────────────────
arrow(ax, 6.0, BY1, 6.0, BY2 + BH2)

# ════════════════════════════════════════════════════════════════════════════
#  第3层：4个提示策略框
# ════════════════════════════════════════════════════════════════════════════
PW = 2.55;  PH = 1.55;  PY = 10.05
PXS = [0.35, 3.10, 5.85, 8.60]   # 左边 x

prompt_titles  = ["零样本策略", "思维链策略", "少样本策略", "组合策略"]
prompt_subs    = [
    "角色定义\n11文件清单",
    "4步推理链\nAPI参考",
    "ps3完整示例\n模式迁移",
    "示例+推理链\n+API片段",
]

for px, title, sub in zip(PXS, prompt_titles, prompt_subs):
    box(ax, px, PY, PW, PH, title,
        C_PRM_E, C_PRM_F, title_h=0.45, fs=9, sub_fs=8, subtitle=sub)

phase_label(ax, PY + PH/2, "提示策略")

# ── 扇出箭头 2→3（从 aadl2c 底部中心 → 各提示框顶部中心）────────────────────
src_y = BY2
targets_x3 = [px + PW/2 for px in PXS]
fan_arrows(ax, 6.0, src_y, targets_x3, PY + PH)

# ════════════════════════════════════════════════════════════════════════════
#  第4层：大语言模型（横跨）
# ════════════════════════════════════════════════════════════════════════════
LX = 0.55;  LW = 10.9;  LY = 8.55;  LH = 0.90
simple_box(ax, LX, LY, LW, LH, "大  语  言  模  型  （ LLM ）",
           C_LLM_E, C_LLM_F, fs=11, fw='bold')

phase_label(ax, LY + LH/2, "生  成")

# ── 扇入箭头 3→4（各提示框底部中心 → LLM 顶部对应位置）──────────────────────
fan_in_arrows(ax, targets_x3, PY, 6.0, LY + LH)

# ════════════════════════════════════════════════════════════════════════════
#  第5层：两个评估框（并排）
# ════════════════════════════════════════════════════════════════════════════
EY = 6.25;  EH = 1.90;  EW = 5.40
EX_L = 0.40;  EX_R = 6.20

# 左：三维评分
box(ax, EX_L, EY, EW, EH, "三维评分框架",
    C_EVAL_E, C_EVAL_F, title_h=0.50, fs=9.5)
# 内部3个子框
sub_labels_l = ["结构完整性  S × 30%", "API 正确性   A × 40%", "语义一致性  C × 30%"]
sub_h = 0.36;  sub_gap = 0.06;  sub_w = EW - 0.40
sub_y = EY + EH - 0.50 - sub_gap
for lbl in sub_labels_l:
    sub_y -= sub_h
    simple_box(ax, EX_L + 0.20, sub_y, sub_w, sub_h, lbl,
               C_SUB_E, C_SUB_F, fs=8.0)
    sub_y -= sub_gap

# 右：ARINC 653合规检查
box(ax, EX_R, EY, EW, EH, "ARINC 653 合规检查",
    C_ARINC_E, C_ARINC_F, title_h=0.50, fs=9.5)
sub_labels_r = ["R1–R8   共 8 条规则", "双端 API · HM回调 · 11文件"]
sub_y = EY + EH - 0.50 - sub_gap
for lbl in sub_labels_r:
    sub_y -= sub_h
    simple_box(ax, EX_R + 0.20, sub_y, sub_w, sub_h, lbl,
               "#E8BCBA", "#FEF9F9", fs=8.0)
    sub_y -= sub_gap

phase_label(ax, EY + EH/2, "评  估")

# ── 扇出箭头 4→5（LLM底部 → 两评估框顶部中心）────────────────────────────────
arrow(ax, 6.0, LY, EX_L + EW/2, EY + EH)
arrow(ax, 6.0, LY, EX_R + EW/2, EY + EH)

# ════════════════════════════════════════════════════════════════════════════
#  第6层：结果输出框（两个）
# ════════════════════════════════════════════════════════════════════════════
RY = 5.10;  RH = 0.65;  RW = EW

simple_box(ax, EX_L, RY, RW, RH, "三 维 综 合 得 分  （ S / A / C ）",
           C_RES_E, C_RES_F, fs=8.5, fw='bold')
simple_box(ax, EX_R, RY, RW, RH, "ARINC 653  合 规 率",
           C_RES2_E, C_RES2_F, fs=8.5, fw='bold')

phase_label(ax, RY + RH/2, "结  果")

# ── 箭头 5→6 ─────────────────────────────────────────────────────────────────
arrow(ax, EX_L + EW/2, EY, EX_L + EW/2, RY + RH)
arrow(ax, EX_R + EW/2, EY, EX_R + EW/2, RY + RH)

# ── 图标题 ───────────────────────────────────────────────────────────────────
plt.title("图3-1  基于提示词工程的IMA分区代码自动生成方法总体流程",
          fontsize=11, fontweight='bold', pad=10, color='#2E4057')

plt.tight_layout()
out_path = os.path.join(OUT_DIR, "fig3_pipeline.png")
plt.savefig(out_path, bbox_inches='tight', dpi=180)
print(f"[DONE] 已保存: {out_path}")
plt.close()
