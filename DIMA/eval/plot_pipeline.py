#!/usr/bin/env python3
"""方法总体流程图 — 横向四列，仿参考图风格"""

import os
import matplotlib.pyplot as plt
from matplotlib.patches import FancyBboxPatch
from matplotlib import rcParams

rcParams['font.family'] = 'WenQuanYi Zen Hei'
rcParams['axes.unicode_minus'] = False

OUT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "results", "figures")
os.makedirs(OUT_DIR, exist_ok=True)

# ── 配色 ──────────────────────────────────────────────────────────────────
CH = {
    "input":   ("#2471A3", "#D6EAF8"),   # 蓝：输入层
    "parse":   ("#148F77", "#D1F2EB"),   # 绿：解析层
    "prompt":  ("#1F618D", "#EBF5FB"),   # 深蓝：提示词
    "llm":     ("#6C3483", "#F0EBF8"),   # 紫：LLM
    "eval3d":  ("#1A5276", "#D6EAF8"),   # 蓝：三维评分
    "arinc":   ("#922B21", "#FADBD8"),   # 红：ARINC
}
C_ARR = "#555555"

fig, ax = plt.subplots(figsize=(16, 9))
ax.set_xlim(0, 16)
ax.set_ylim(0, 9)
ax.axis('off')
fig.patch.set_facecolor('white')


# ════════════════════════════════════════════════════════════════════════════
#  辅助函数
# ════════════════════════════════════════════════════════════════════════════

def section_bg(ax, x, y, w, h, label, fc):
    ax.add_patch(FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.15",
                                lw=1.4, edgecolor="#BBBBBB", facecolor=fc,
                                alpha=0.45, zorder=0))
    ax.text(x + w / 2, y + h + 0.18, label, ha='center', va='bottom',
            fontsize=9.5, color='#555555', fontstyle='italic')


def badge(ax, cx, cy, num, hc):
    """编号圆形徽章"""
    ax.add_patch(plt.Circle((cx, cy), 0.26, color=hc, zorder=6))
    ax.text(cx, cy, num, ha='center', va='center',
            fontsize=10, fontweight='bold', color='white', zorder=7)


def header_box(ax, x, y, w, h, num, title, sec_label, key, items):
    """带彩色标题栏 + 编号 + 子项的主框"""
    hc, fc = CH[key]
    HDR = 0.55
    # 主框
    ax.add_patch(FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.07",
                                lw=1.6, edgecolor=hc, facecolor=fc, zorder=2))
    # 标题栏
    ax.add_patch(FancyBboxPatch((x, y + h - HDR), w, HDR,
                                boxstyle="round,pad=0.04",
                                lw=0, edgecolor='none', facecolor=hc, zorder=3))
    # 徽章
    badge(ax, x + 0.35, y + h - HDR / 2, num, hc)
    # 标题
    ax.text(x + 0.68, y + h - HDR / 2, title,
            ha='left', va='center', fontsize=9.5, fontweight='bold',
            color='white', zorder=5)
    # 章节标注（右上角）
    if sec_label:
        ax.text(x + w - 0.12, y + h - HDR / 2, sec_label,
                ha='right', va='center', fontsize=7.5, color='#BEE0F5', zorder=5)
    # 子项
    cy = y + h - HDR - 0.10
    for item in items:
        cy -= 0.355
        ax.text(x + 0.20, cy, item, ha='left', va='top',
                fontsize=8.2, color='#333333', zorder=4)
    return (x + w / 2, y)          # 底部中心


def strategy_box(ax, x, y, w, h, title, sub, hc, fc):
    """策略小框"""
    HDR = 0.40
    ax.add_patch(FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.05",
                                lw=1.2, edgecolor=hc, facecolor=fc, zorder=3))
    ax.add_patch(FancyBboxPatch((x, y + h - HDR), w, HDR,
                                boxstyle="round,pad=0.03",
                                lw=0, edgecolor='none', facecolor=hc, zorder=4))
    ax.text(x + w / 2, y + h - HDR / 2, title,
            ha='center', va='center', fontsize=8.5, fontweight='bold',
            color='white', zorder=5)
    ax.text(x + w / 2, y + (h - HDR) / 2, sub,
            ha='center', va='center', fontsize=7.6, color='#444444',
            zorder=4, linespacing=1.5)


def arrow(ax, x1, y1, x2, y2, lbl=''):
    ax.annotate('', xy=(x2, y2), xytext=(x1, y1),
                arrowprops=dict(arrowstyle='-|>', color=C_ARR, lw=2.0), zorder=6)
    if lbl:
        mx, my = (x1 + x2) / 2, (y1 + y2) / 2
        ax.text(mx + 0.12, my, lbl, fontsize=7.5, color='#555555', va='center')


def fan_arrow(ax, x1, y1, targets_x, y2):
    for tx in targets_x:
        ax.annotate('', xy=(tx, y2), xytext=(x1, y1),
                    arrowprops=dict(arrowstyle='-|>', color=C_ARR, lw=1.6), zorder=6)


def fan_in_arrow(ax, sources_x, y1, x2, y2):
    for sx in sources_x:
        ax.annotate('', xy=(x2, y2), xytext=(sx, y1),
                    arrowprops=dict(arrowstyle='-|>', color=C_ARR, lw=1.6), zorder=6)


# ════════════════════════════════════════════════════════════════════════════
#  Section 背景（三个阶段）
# ════════════════════════════════════════════════════════════════════════════
section_bg(ax, 0.20, 0.30, 4.90, 8.10, "输入与规格提取", "#EBF5FB")
section_bg(ax, 5.30, 0.30, 4.60, 8.10, "提示词工程",    "#EAF7EF")
section_bg(ax, 10.10, 0.30, 5.65, 8.10, "代码生成与评估", "#F5EEF8")

# ════════════════════════════════════════════════════════════════════════════
#  列1：实验系统（§3.3）
# ════════════════════════════════════════════════════════════════════════════
header_box(ax, 0.40, 4.55, 2.20, 3.70, "①", "实验系统", "§3.3", "input", [
    "· AADL 系统架构模型",
    "",
    "· DIMA 系统  5 个分区",
    "· IMA2 系统  3 个分区",
    "· IMA3 系统  4 个分区",
    "",
    "  共 12 个分区",
    "  合计 48 组实验",
])

# ════════════════════════════════════════════════════════════════════════════
#  列2：AADL 解析（§3.2）
# ════════════════════════════════════════════════════════════════════════════
header_box(ax, 0.40, 0.55, 2.20, 3.65, "②", "规格解析", "§3.2", "parse", [
    "· aadl2c.py 解析器",
    "",
    "  分区规格提取",
    "  任务（周期/优先级）",
    "  端口（采样/队列）",
    "  资源（黑板/缓冲区）",
    "",
    "· 输出：JSON 规格",
])

# 系统 → 解析
arrow(ax, 1.50, 4.55, 1.50, 4.20)

# ── JSON规格 → 提示词工程（水平箭头）────────────────────────────────────────
arrow(ax, 2.60, 2.35, 5.40, 2.35, "JSON规格")

# 系统也有箭头向右
arrow(ax, 2.60, 6.40, 3.20, 6.40)
ax.annotate('', xy=(5.40, 4.65), xytext=(3.20, 6.40),
            arrowprops=dict(arrowstyle='-|>', color=C_ARR, lw=1.6,
                            connectionstyle="arc3,rad=-0.25"), zorder=6)

# ════════════════════════════════════════════════════════════════════════════
#  列3：提示词工程（§3.4）
# ════════════════════════════════════════════════════════════════════════════
hc_p, fc_p = CH["prompt"]

# 大框
ax.add_patch(FancyBboxPatch((5.40, 0.55), 4.20, 8.0,
                             boxstyle="round,pad=0.08",
                             lw=1.6, edgecolor=hc_p, facecolor=fc_p, zorder=2))
# header
HDR = 0.55
ax.add_patch(FancyBboxPatch((5.40, 8.00), 4.20, HDR,
                             boxstyle="round,pad=0.04",
                             lw=0, edgecolor='none', facecolor=hc_p, zorder=3))
badge(ax, 5.75, 8.00 + HDR / 2, "③", hc_p)
ax.text(6.08, 8.00 + HDR / 2, "提示词工程",
        ha='left', va='center', fontsize=9.5, fontweight='bold', color='white', zorder=5)
ax.text(9.42, 8.00 + HDR / 2, "§3.4",
        ha='right', va='center', fontsize=7.5, color='#BEE0F5', zorder=5)

# 四种策略（纵向排列）
strategies = [
    ("零样本策略", "角色定义\n+ 文件清单"),
    ("思维链策略", "4步推理链\n+ API参考片段"),
    ("少样本策略", "完整示例代码\n+ 模式迁移"),
    ("组合策略",   "示例 + 推理链\n+ API片段"),
]
sy = 7.75
strat_centers = []
for title, sub in strategies:
    strategy_box(ax, 5.58, sy - 1.68, 3.84, 1.55, title, sub, hc_p, "#F0F6FF")
    strat_centers.append(5.58 + 3.84 / 2)
    sy -= 1.75

# ════════════════════════════════════════════════════════════════════════════
#  列4：大语言模型
# ════════════════════════════════════════════════════════════════════════════
header_box(ax, 10.25, 5.00, 5.10, 3.55, "④", "大语言模型", "", "llm", [
    "· Claude 3.5 Sonnet",
    "",
    "· 输入：提示词 + JSON规格",
    "· 输出：11 个 C/H 文件 / 分区",
    "",
    "  每分区代码约 500–1200 行",
])

# ════════════════════════════════════════════════════════════════════════════
#  列4下：双评估框（§3.5/第4章）
# ════════════════════════════════════════════════════════════════════════════
header_box(ax, 10.25, 0.55, 2.40, 4.10, "⑤", "三维评分", "§3.5", "eval3d", [
    "· S  结构完整性 ×30%",
    "     11文件 / HM回调",
    "· A  API 正确性  ×40%",
    "     CREATE/READ/WRITE",
    "· C  语义一致性 ×30%",
    "     任务/端口/资源命名",
    "",
    "  → 综合得分（0–100）",
])

header_box(ax, 12.90, 0.55, 2.40, 4.10, "⑥", "ARINC 653", "§3.5", "arinc", [
    "· R1  黑板双端操作",
    "· R2  缓冲区双端操作",
    "· R3/R4  采样端口方向",
    "· R5/R6  队列端口方向",
    "· R7  HM 回调实现",
    "· R8  禁止循环内CREATE",
    "",
    "  → 合规率（0–100%）",
])

# ════════════════════════════════════════════════════════════════════════════
#  箭头连接
# ════════════════════════════════════════════════════════════════════════════
# 提示词大框右侧 → LLM 左侧
arrow(ax, 9.60, 4.55, 10.25, 6.77)

# LLM → 三维评分
arrow(ax, 11.45, 5.00, 11.45, 4.65)
# LLM → ARINC653
arrow(ax, 13.90, 5.00, 14.10, 4.65)

# ── 图标题 ────────────────────────────────────────────────────────────────
plt.title("图3-1  基于提示词工程的IMA分区代码自动生成方法总体流程",
          fontsize=11, fontweight='bold', pad=14, color='#2E4057')

out_path = os.path.join(OUT_DIR, "fig3_pipeline.png")
plt.savefig(out_path, bbox_inches='tight', dpi=180)
print(f"[DONE] 已保存: {out_path}")
plt.close()
