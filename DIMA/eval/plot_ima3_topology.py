#!/usr/bin/env python3
"""IMA3系统拓扑结构图 — 简洁重绘版"""

import os
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch
from matplotlib import rcParams

rcParams['font.family'] = 'WenQuanYi Zen Hei'
rcParams['axes.unicode_minus'] = False

OUT_DIR = os.path.join(os.path.dirname(__file__), "results", "figures")
os.makedirs(OUT_DIR, exist_ok=True)

C_BG     = "#F5F7FA"
C_BORDER = "#2E4057"
C_TITLE  = "#2E4057"
C_TTXT   = "#FFFFFF"
C_TASK   = "#D6EAF8"
C_BB     = "#FDEBD0"
C_BUF    = "#E8DAEF"
C_SAMPLE = "#2471A3"
C_QUEUE  = "#C0392B"
C_MC     = "#ECF0F1"

fig, ax = plt.subplots(figsize=(14, 9))
ax.set_xlim(0, 14)
ax.set_ylim(0, 9)
ax.axis('off')
fig.patch.set_facecolor('white')

TITLE_H = 0.38
ITEM_H  = 0.38
ITEM_G  = 0.10
PAD     = 0.18


def partition(ax, x, y, w, h, title, tc=C_TITLE):
    ax.add_patch(FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.06",
                                lw=1.8, edgecolor=C_BORDER, facecolor='white', zorder=2))
    ax.add_patch(FancyBboxPatch((x, y+h-TITLE_H), w, TITLE_H,
                                boxstyle="round,pad=0.03",
                                lw=0, edgecolor='none', facecolor=tc, zorder=3))
    ax.text(x+w/2, y+h-TITLE_H/2, title,
            ha='center', va='center', fontsize=10, fontweight='bold',
            color=C_TTXT, zorder=4)


def items_col(ax, col_x, top_y, col_w, labels, color):
    cy = top_y
    for lbl in labels:
        cy -= ITEM_H
        ax.add_patch(FancyBboxPatch((col_x, cy), col_w, ITEM_H,
                                    boxstyle="round,pad=0.04",
                                    lw=0.7, edgecolor='#AAAAAA', facecolor=color, zorder=3))
        ax.text(col_x+col_w/2, cy+ITEM_H/2, lbl,
                ha='center', va='center', fontsize=7.5, color='#222222', zorder=4)
        cy -= ITEM_G
    return cy


def arrow(ax, x1, y1, x2, y2, color, lbl, rad=0.0, lbl_offset=(0, 0.26)):
    ax.annotate("", xy=(x2, y2), xytext=(x1, y1),
                arrowprops=dict(arrowstyle="-|>", color=color, lw=2.0,
                                connectionstyle=f"arc3,rad={rad}"),
                zorder=5)
    mx = (x1+x2)/2 + lbl_offset[0]
    my = (y1+y2)/2 + lbl_offset[1]
    ax.text(mx, my, lbl, ha='center', va='center', fontsize=7.5,
            color=color, fontweight='bold',
            bbox=dict(boxstyle='round,pad=0.15', fc='white', ec='none', alpha=0.9),
            zorder=6)


# 模块 MC 外框
ax.add_patch(FancyBboxPatch((0.25, 0.35), 13.5, 8.2, boxstyle="round,pad=0.1",
                             lw=2.2, edgecolor='#555555', facecolor=C_MC, zorder=0))
ax.text(7.0, 8.38, "模块 MC   （主帧周期 100 ms，RMS调度）",
        ha='center', va='center', fontsize=9.5, color='#444444', style='italic')

# 分区坐标
NX, NY, NW, NH = 0.55, 4.20, 5.80, 3.70   # NAV 左上
DX, DY, DW, DH = 7.60, 4.20, 5.80, 3.70   # DISP 右上
MX, MY, MW, MH = 0.55, 0.55, 5.80, 3.25   # MON 左下
TX, TY, TW, TH = 7.60, 0.55, 5.80, 3.25   # CTRL 右下

COL_W = 1.55   # 每列宽度
COL_G = 0.28   # 列间距

# ── NAV ──────────────────────────────────────────────────────────────────────
partition(ax, NX, NY, NW, NH, "NAV（导航分区）")
top_nav = NY + NH - TITLE_H - PAD

c1 = NX + PAD
c2 = c1 + COL_W + COL_G
c3 = c2 + COL_W + COL_G

items_col(ax, c1, top_nav, COL_W,
          ["taskN1\n25ms  P2", "taskN2\n50ms  P3",
           "taskN3\n100ms P4", "taskN4\n50ms  P5"], C_TASK)
items_col(ax, c2, top_nav, COL_W, ["黑板\nbb_bb1", "黑板\nbb_bb2"], C_BB)
items_col(ax, c3, top_nav, COL_W, ["缓冲区\nbuf_buf1"], C_BUF)

# NAV端口标签（右侧）
y_pos_out    = top_nav - ITEM_H * 0.5
y_status_out = top_nav - ITEM_H * 1.5 - ITEM_G
y_cmd_out    = top_nav - ITEM_H * 3.5 - ITEM_G * 3
ax.text(NX+NW-0.06, y_pos_out,    "pos_out →\n(采样·源)",    ha='right', va='center', fontsize=7, color=C_SAMPLE)
ax.text(NX+NW-0.06, y_status_out, "status_out →\n(采样·源)", ha='right', va='center', fontsize=7, color=C_SAMPLE)
ax.text(NX+NW-0.06, y_cmd_out,    "cmd_out →\n(队列·源)",    ha='right', va='center', fontsize=7, color=C_QUEUE)

# ── DISP ─────────────────────────────────────────────────────────────────────
partition(ax, DX, DY, DW, DH, "DISP（显示分区）")
top_disp = DY + DH - TITLE_H - PAD

d1 = DX + PAD
d2 = d1 + COL_W + COL_G
items_col(ax, d1, top_disp, COL_W, ["taskD1\n50ms  P2", "taskD2\n100ms P3"], C_TASK)
items_col(ax, d2, top_disp, COL_W, ["黑板\nbb_disp"], C_BB)

y_posin  = top_disp - ITEM_H * 0.5
y_ctrlin = top_disp - ITEM_H * 1.5 - ITEM_G
ax.text(DX+0.06, y_posin,  "← pos_in\n(采样·目标)",  ha='left', va='center', fontsize=7, color=C_SAMPLE)
ax.text(DX+0.06, y_ctrlin, "← ctrl_in\n(队列·目标)", ha='left', va='center', fontsize=7, color=C_QUEUE)

# ── MON ──────────────────────────────────────────────────────────────────────
partition(ax, MX, MY, MW, MH, "MON（监控分区）", tc='#607D8B')
top_mon = MY + MH - TITLE_H - PAD

items_col(ax, MX + PAD, top_mon, COL_W, ["taskM1\n100ms P2"], C_TASK)
ax.text(MX+MW/2, MY+MH*0.35, "（无分区内共享资源）",
        ha='center', va='center', fontsize=8.5, color='#888888', style='italic')
ax.text(MX+MW-0.06, top_mon - ITEM_H*0.5, "← status_in\n(采样·目标)",
        ha='right', va='center', fontsize=7, color=C_SAMPLE)

# ── CTRL ─────────────────────────────────────────────────────────────────────
partition(ax, TX, TY, TW, TH, "CTRL（控制分区）")
top_ctrl = TY + TH - TITLE_H - PAD

t1 = TX + PAD
t2 = t1 + COL_W + COL_G
items_col(ax, t1, top_ctrl, COL_W,
          ["taskT1\n25ms  P2", "taskT2\n50ms  P3", "taskT3\n100ms P4"], C_TASK)
items_col(ax, t2, top_ctrl, COL_W, ["缓冲区\nbuf_ctrl"], C_BUF)

y_navin   = top_ctrl - ITEM_H * 0.5
y_ctrlout = top_ctrl - ITEM_H * 2.5 - ITEM_G * 2
ax.text(TX+0.06, y_navin,   "← nav_in\n(队列·目标)",  ha='left', va='center', fontsize=7, color=C_QUEUE)
ax.text(TX+0.06, y_ctrlout, "ctrl_out →\n(队列·源)",   ha='left', va='center', fontsize=7, color=C_QUEUE)

# ══════════════════════════════════════════════════════════════════════════════
#  分区间通信箭头（坐标与端口标签对齐）
# ══════════════════════════════════════════════════════════════════════════════

# NAV → DISP  pos_out → pos_in  水平
arrow(ax, NX+NW, y_pos_out, DX, y_posin,
      C_SAMPLE, "pos_out → pos_in\n采样端口", lbl_offset=(0, 0.30))

# NAV → MON  status_out → status_in  垂直（沿左列中线）
x_vert_l = NX + PAD + COL_W * 0.5
arrow(ax, x_vert_l, NY, x_vert_l, MY+MH,
      C_SAMPLE, "status_out → status_in\n采样端口", lbl_offset=(1.9, 0))

# NAV → CTRL  cmd_out → nav_in  斜线（右侧出→右上角入）
arrow(ax, NX+NW, y_cmd_out, TX, y_navin,
      C_QUEUE, "cmd_out → nav_in\n队列端口", rad=-0.12, lbl_offset=(0, 0.34))

# CTRL → DISP  ctrl_out → ctrl_in  垂直（右列中线）
x_vert_r = TX + TW * 0.55
arrow(ax, x_vert_r, y_ctrlout, x_vert_r, DY,
      C_QUEUE, "ctrl_out → ctrl_in\n队列端口", lbl_offset=(1.9, 0))

# ══════════════════════════════════════════════════════════════════════════════
#  图例
# ══════════════════════════════════════════════════════════════════════════════
legend_items = [
    mpatches.Patch(facecolor=C_TASK,   label="周期性任务（period / priority）"),
    mpatches.Patch(facecolor=C_BB,     label="黑板（data port连接）"),
    mpatches.Patch(facecolor=C_BUF,    label="缓冲区（event data port连接）"),
    mpatches.Patch(facecolor=C_SAMPLE, label="采样端口通信（最新值语义）"),
    mpatches.Patch(facecolor=C_QUEUE,  label="队列端口通信（FIFO语义）"),
]
ax.legend(handles=legend_items, loc='lower center',
          bbox_to_anchor=(0.5, -0.03), ncol=3,
          fontsize=8.5, framealpha=0.95, edgecolor='#CCCCCC')

plt.title("图3-3  IMA3导航计算模块系统拓扑结构",
          fontsize=12, fontweight='bold', pad=12, color='#2E4057')
plt.tight_layout()

out_path = os.path.join(OUT_DIR, "fig3_ima3_topology.png")
plt.savefig(out_path, bbox_inches='tight', dpi=180)
print(f"[DONE] 已保存: {out_path}")
plt.close()
