#!/usr/bin/env python3
"""
IMA3系统拓扑结构图生成脚本
输出: results/figures/fig3_ima3_topology.png
"""

import os
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch
from matplotlib import rcParams

rcParams['font.family'] = 'WenQuanYi Zen Hei'
rcParams['axes.unicode_minus'] = False
rcParams['figure.dpi'] = 150

OUT_DIR = os.path.join(os.path.dirname(__file__), "results", "figures")
os.makedirs(OUT_DIR, exist_ok=True)

# ── 颜色 ──────────────────────────────────────────────────────────────────────
C_MODULE  = "#F0F4F8"   # 模块背景
C_PART    = "#FFFFFF"   # 分区背景
C_BORDER  = "#2E4057"   # 分区边框
C_TITLE   = "#2E4057"   # 分区标题背景
C_TTXT    = "#FFFFFF"   # 分区标题文字
C_TASK    = "#E8F4FD"   # 任务块背景
C_BB      = "#FFF3E0"   # 黑板背景
C_BUF     = "#F3E5F5"   # 缓冲区背景
C_SAMPLE  = "#2E86AB"   # 采样箭头
C_QUEUE   = "#E07A5F"   # 队列箭头
C_INTRA   = "#888888"   # 分区内连接
C_MON_BG  = "#FAFAFA"   # MON特别浅色

fig, ax = plt.subplots(figsize=(13, 8))
ax.set_xlim(0, 13)
ax.set_ylim(0, 8)
ax.axis('off')

# ── 辅助函数 ──────────────────────────────────────────────────────────────────
def draw_partition(ax, x, y, w, h, title, color_title=C_TITLE, color_bg=C_PART):
    """画分区外框+标题栏"""
    box = FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.05",
                         linewidth=1.5, edgecolor=C_BORDER, facecolor=color_bg, zorder=2)
    ax.add_patch(box)
    title_h = 0.42
    tbox = FancyBboxPatch((x, y+h-title_h), w, title_h,
                          boxstyle="round,pad=0.02",
                          linewidth=0, edgecolor='none', facecolor=color_title, zorder=3)
    ax.add_patch(tbox)
    ax.text(x+w/2, y+h-title_h/2, title, ha='center', va='center',
            fontsize=10, fontweight='bold', color=C_TTXT, zorder=4)

def draw_resource_box(ax, x, y, w, h, label, color):
    box = FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.04",
                         linewidth=0.8, edgecolor='#AAAAAA', facecolor=color, zorder=3)
    ax.add_patch(box)
    ax.text(x+w/2, y+h/2, label, ha='center', va='center',
            fontsize=7.5, color='#333333', zorder=4)

def draw_arrow(ax, x1, y1, x2, y2, color, label, label_side='top',
               connectionstyle="arc3,rad=0.0", lw=1.8, fs=8):
    ax.annotate("", xy=(x2, y2), xytext=(x1, y1),
                arrowprops=dict(arrowstyle="-|>", color=color, lw=lw,
                                connectionstyle=connectionstyle),
                zorder=5)
    mx, my = (x1+x2)/2, (y1+y2)/2
    offset = 0.18 if label_side == 'top' else -0.18
    ax.text(mx, my+offset, label, ha='center', va='center',
            fontsize=fs, color=color, fontweight='bold',
            bbox=dict(boxstyle='round,pad=0.15', facecolor='white',
                      edgecolor='none', alpha=0.85), zorder=6)

# ── 模块MC外框 ────────────────────────────────────────────────────────────────
module_box = FancyBboxPatch((0.3, 0.4), 12.4, 7.1, boxstyle="round,pad=0.1",
                            linewidth=2.0, edgecolor='#4A4A4A',
                            facecolor=C_MODULE, zorder=1)
ax.add_patch(module_box)
ax.text(6.5, 7.28, "模块 MC  （主帧周期 100 ms，RMS调度）",
        ha='center', va='center', fontsize=9.5, color='#4A4A4A',
        style='italic')

# ════════════════════════════════════════════════════════════════════════════════
#  NAV 分区  (左上，最复杂)
# ════════════════════════════════════════════════════════════════════════════════
NX, NY, NW, NH = 0.65, 3.8, 3.8, 3.2
draw_partition(ax, NX, NY, NW, NH, "NAV（导航分区）")

# 任务列
tasks_nav = [
    ("taskN1\n25ms  P2", 0.75, 6.5),
    ("taskN2\n50ms  P3", 0.75, 5.85),
    ("taskN3\n100ms P4", 0.75, 5.2),
    ("taskN4\n50ms  P5", 0.75, 4.55),
]
for label, tx, ty in tasks_nav:
    draw_resource_box(ax, NX+0.12, ty-0.24, 1.35, 0.44, label, C_TASK)

# 黑板
draw_resource_box(ax, NX+1.65, 6.26, 1.12, 0.44, "黑板\nbb_bb1", C_BB)
draw_resource_box(ax, NX+1.65, 5.62, 1.12, 0.44, "黑板\nbb_bb2", C_BB)
# 缓冲区
draw_resource_box(ax, NX+1.65, 4.98, 1.12, 0.44, "缓冲区\nbuf_buf1", C_BUF)

# 分区内连接标注（细虚线）
for yi in [6.48, 5.84]:
    ax.annotate("", xy=(NX+1.65, yi), xytext=(NX+1.47, yi),
                arrowprops=dict(arrowstyle="-|>", color=C_INTRA, lw=0.9,
                                linestyle='dashed'), zorder=4)
ax.annotate("", xy=(NX+1.65, 5.2), xytext=(NX+1.47, 5.2),
            arrowprops=dict(arrowstyle="-|>", color=C_INTRA, lw=0.9,
                            linestyle='dashed'), zorder=4)

# 端口标签（分区右侧）
ax.text(NX+NW-0.06, 6.72, "pos_out\n(采样·源)", ha='right', va='center',
        fontsize=7, color=C_SAMPLE)
ax.text(NX+NW-0.06, 6.3, "status_out\n(采样·源)", ha='right', va='center',
        fontsize=7, color=C_SAMPLE)
ax.text(NX+NW-0.06, 5.0, "cmd_out\n(队列·源)", ha='right', va='center',
        fontsize=7, color=C_QUEUE)

# ════════════════════════════════════════════════════════════════════════════════
#  DISP 分区  (右上)
# ════════════════════════════════════════════════════════════════════════════════
DX, DY, DW, DH = 8.6, 3.8, 3.8, 3.2
draw_partition(ax, DX, DY, DW, DH, "DISP（显示分区）")

draw_resource_box(ax, DX+0.12, 6.5-0.24,  1.35, 0.44, "taskD1\n50ms  P2", C_TASK)
draw_resource_box(ax, DX+0.12, 5.85-0.24, 1.35, 0.44, "taskD2\n100ms P3", C_TASK)

draw_resource_box(ax, DX+1.65, 6.26, 1.12, 0.44, "黑板\nbb_disp", C_BB)

ax.annotate("", xy=(DX+1.65, 6.48), xytext=(DX+1.47, 6.48),
            arrowprops=dict(arrowstyle="-|>", color=C_INTRA, lw=0.9,
                            linestyle='dashed'), zorder=4)

ax.text(DX+0.06, 6.72, "pos_in\n(采样·目标)", ha='left', va='center',
        fontsize=7, color=C_SAMPLE)
ax.text(DX+0.06, 5.62, "ctrl_in\n(队列·目标)", ha='left', va='center',
        fontsize=7, color=C_QUEUE)

# ════════════════════════════════════════════════════════════════════════════════
#  CTRL 分区  (右下)
# ════════════════════════════════════════════════════════════════════════════════
TX, TY, TW, TH = 8.6, 0.65, 3.8, 2.85
draw_partition(ax, TX, TY, TW, TH, "CTRL（控制分区）")

draw_resource_box(ax, TX+0.12, 3.05-0.24, 1.35, 0.44, "taskT1\n25ms  P2", C_TASK)
draw_resource_box(ax, TX+0.12, 2.4-0.24,  1.35, 0.44, "taskT2\n50ms  P3", C_TASK)
draw_resource_box(ax, TX+0.12, 1.75-0.24, 1.35, 0.44, "taskT3\n100ms P4", C_TASK)

draw_resource_box(ax, TX+1.65, 2.65, 1.12, 0.44, "缓冲区\nbuf_ctrl", C_BUF)

ax.annotate("", xy=(TX+1.65, 2.87), xytext=(TX+1.47, 2.87),
            arrowprops=dict(arrowstyle="-|>", color=C_INTRA, lw=0.9,
                            linestyle='dashed'), zorder=4)

ax.text(TX+0.06, 3.18, "nav_in\n(队列·目标)", ha='left', va='center',
        fontsize=7, color=C_QUEUE)
ax.text(TX+0.06, 1.52, "ctrl_out\n(队列·源)", ha='left', va='center',
        fontsize=7, color=C_QUEUE)

# ════════════════════════════════════════════════════════════════════════════════
#  MON 分区  (左下，最简)
# ════════════════════════════════════════════════════════════════════════════════
MX, MY, MW, MH = 0.65, 0.65, 3.8, 2.85
draw_partition(ax, MX, MY, MW, MH, "MON（监控分区）", color_title='#607D8B')

draw_resource_box(ax, MX+0.12, 1.8, 1.35, 0.44, "taskM1\n100ms P2", C_TASK)
ax.text(MX+MW/2, 1.3, "（无分区内共享资源）", ha='center', va='center',
        fontsize=8, color='#888888', style='italic')

ax.text(MX+MW-0.06, 2.85, "status_in\n(采样·目标)", ha='right', va='center',
        fontsize=7, color=C_SAMPLE)

# ════════════════════════════════════════════════════════════════════════════════
#  分区间通信箭头
# ════════════════════════════════════════════════════════════════════════════════

# NAV → DISP  采样（位置数据）pos_out → pos_in，水平
draw_arrow(ax, NX+NW, 6.72, DX, 6.72,
           color=C_SAMPLE, label="pos_out → pos_in\n采样端口",
           label_side='top', lw=2.0, fs=8)

# NAV → MON  采样（状态数据）垂直向下
draw_arrow(ax, NX+NW*0.4, NY, MX+MW*0.4, MY+MH,
           color=C_SAMPLE, label="status_out→status_in\n采样端口",
           label_side='top',
           connectionstyle="arc3,rad=0.0", lw=2.0, fs=8)

# NAV → CTRL  队列（导航指令），斜线
draw_arrow(ax, NX+NW, 5.0, TX, 3.05,
           color=C_QUEUE, label="cmd_out → nav_in\n队列端口",
           label_side='top',
           connectionstyle="arc3,rad=-0.18", lw=2.0, fs=8)

# CTRL → DISP  队列（显示控制），垂直
draw_arrow(ax, TX+TW*0.5, TY+TH, DX+DW*0.5, DY,
           color=C_QUEUE, label="ctrl_out→ctrl_in\n队列端口",
           label_side='top',
           connectionstyle="arc3,rad=0.0", lw=2.0, fs=8)

# ── 图例 ──────────────────────────────────────────────────────────────────────
legend_items = [
    mpatches.Patch(facecolor=C_SAMPLE, label="采样端口通信（最新值语义）"),
    mpatches.Patch(facecolor=C_QUEUE,  label="队列端口通信（FIFO语义）"),
    mpatches.Patch(facecolor=C_BB,     label="黑板（分区内，data port连接）"),
    mpatches.Patch(facecolor=C_BUF,    label="缓冲区（分区内，event data port连接）"),
    mpatches.Patch(facecolor=C_TASK,   label="周期性任务（period / priority）"),
]
ax.legend(handles=legend_items, loc='lower center',
          bbox_to_anchor=(0.5, -0.04), ncol=3,
          fontsize=8, framealpha=0.9, edgecolor='#CCCCCC')

plt.title("图3-3  IMA3导航计算模块系统拓扑结构", fontsize=11,
          fontweight='bold', pad=10, color='#2E4057')
plt.tight_layout()

out_path = os.path.join(OUT_DIR, "fig3_ima3_topology.png")
plt.savefig(out_path, bbox_inches='tight', dpi=180)
print(f"[DONE] 已保存: {out_path}")
plt.close()
