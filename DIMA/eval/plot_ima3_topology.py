#!/usr/bin/env python3
"""IMA3系统拓扑结构图 — 端口边缘版"""

import os
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch
from matplotlib import rcParams

rcParams['font.family'] = 'WenQuanYi Zen Hei'
rcParams['axes.unicode_minus'] = False

OUT_DIR = os.path.join(os.path.dirname(__file__), "results", "figures")
os.makedirs(OUT_DIR, exist_ok=True)

C_BORDER = "#2E4057"
C_TITLE  = "#2E4057"
C_TTXT   = "#FFFFFF"
C_TASK   = "#D6EAF8"
C_BB     = "#FDEBD0"
C_BUF    = "#E8DAEF"
C_SAMPLE = "#2471A3"
C_QUEUE  = "#C0392B"
C_MC     = "#EAECEE"

TITLE_H      = 0.38
ITEM_H       = 0.38
ITEM_G       = 0.10
PAD          = 0.20
PORT_W       = 0.76   # 端口框宽
PORT_H       = 0.26   # 端口框高
COL_W        = 1.55   # 内容列宽
COL_G        = 0.28   # 内容列间距
PORT_OFFSET  = PORT_W / 2 + 0.16   # 左边缘端口框→任务列左边距

fig, ax = plt.subplots(figsize=(14, 9))
ax.set_xlim(0, 14)
ax.set_ylim(0, 9)
ax.axis('off')
fig.patch.set_facecolor('white')

# ── 分区坐标 ─────────────────────────────────────────────────────────────────
NX, NY, NW, NH = 0.50, 4.75, 5.80, 3.65   # NAV  左上
DX, DY, DW, DH = 7.70, 4.75, 5.80, 3.65   # DISP 右上
MX, MY, MW, MH = 0.50, 0.50, 5.80, 3.70   # MON  左下
TX, TY, TW, TH = 7.70, 0.50, 5.80, 3.70   # CTRL 右下


def partition(ax, x, y, w, h, title, tc=C_TITLE):
    ax.add_patch(FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.06",
                                lw=1.8, edgecolor=C_BORDER, facecolor='white', zorder=2))
    ax.add_patch(FancyBboxPatch((x, y+h-TITLE_H), w, TITLE_H,
                                boxstyle="round,pad=0.03",
                                lw=0, edgecolor='none', facecolor=tc, zorder=3))
    ax.text(x+w/2, y+h-TITLE_H/2, title, ha='center', va='center',
            fontsize=10, fontweight='bold', color=C_TTXT, zorder=4)


def items_col(ax, col_x, top_y, col_w, labels, color):
    cy = top_y
    for lbl in labels:
        cy -= ITEM_H
        ax.add_patch(FancyBboxPatch((col_x, cy), col_w, ITEM_H,
                                    boxstyle="round,pad=0.04",
                                    lw=0.7, edgecolor='#AAAAAA', facecolor=color, zorder=3))
        ax.text(col_x + col_w/2, cy + ITEM_H/2, lbl,
                ha='center', va='center', fontsize=7.5, color='#222222', zorder=4)
        cy -= ITEM_G
    return cy


def port_box(ax, cx, cy, label, color):
    """端口框，中心锚定在分区边界点 (cx, cy)"""
    ax.add_patch(FancyBboxPatch((cx - PORT_W/2, cy - PORT_H/2), PORT_W, PORT_H,
                                boxstyle="round,pad=0.03",
                                lw=1.3, edgecolor=color, facecolor='white', zorder=6))
    ax.text(cx, cy, label, ha='center', va='center',
            fontsize=7.0, color=color, fontweight='bold', zorder=7)
    return (cx, cy)


def connect(ax, p1, p2, color, label, rad=0.0, lbl_offset=None):
    """从端口框 p1 向端口框 p2 画箭头，label 偏移自动计算（垂直方向偏移）"""
    ax.annotate("", xy=p2, xytext=p1,
                arrowprops=dict(arrowstyle="-|>", color=color, lw=2.0,
                                connectionstyle=f"arc3,rad={rad}"),
                zorder=5)
    mx, my = (p1[0]+p2[0])/2, (p1[1]+p2[1])/2
    if lbl_offset is not None:
        ox, oy = lbl_offset
    else:
        dx, dy = p2[0]-p1[0], p2[1]-p1[1]
        L = (dx**2 + dy**2)**0.5 + 1e-9
        # 左法向量（从 p1 看向 p2 的左侧）
        ox, oy = -dy/L * 0.42, dx/L * 0.42
    ax.text(mx+ox, my+oy, label, ha='center', va='center', fontsize=8.5,
            color=color, fontweight='bold',
            bbox=dict(boxstyle='round,pad=0.18', fc='white', ec='none', alpha=0.93),
            zorder=8)


# ── MC 外框 ──────────────────────────────────────────────────────────────────
ax.add_patch(FancyBboxPatch((0.25, 0.30), 13.5, 8.50, boxstyle="round,pad=0.1",
                             lw=2.2, edgecolor='#555555', facecolor=C_MC, zorder=0))
ax.text(7.0, 8.60, "模块 MC   （主帧周期 100 ms，RMS调度）",
        ha='center', va='center', fontsize=9.5, color='#444444', style='italic')

# ── 分区框 ───────────────────────────────────────────────────────────────────
partition(ax, NX, NY, NW, NH, "NAV（导航分区）")
partition(ax, DX, DY, DW, DH, "DISP（显示分区）")
partition(ax, MX, MY, MW, MH, "MON（监控分区）", tc='#607D8B')
partition(ax, TX, TY, TW, TH, "CTRL（控制分区）")

# ════════════════════════════════════════════════════════════════════════════
#  NAV  内容：三列（任务 | 黑板 | 缓冲区）
# ════════════════════════════════════════════════════════════════════════════
top_nav = NY + NH - TITLE_H - PAD   # 7.82
c1 = NX + PAD                        # 0.70
c2 = c1 + COL_W + COL_G             # 2.53
c3 = c2 + COL_W + COL_G             # 4.36  (右端 5.91，不超过 NAV 右边 6.30)

items_col(ax, c1, top_nav, COL_W,
          ["taskN1\n25ms  P2", "taskN2\n50ms  P3",
           "taskN3\n100ms P4", "taskN4\n50ms  P5"], C_TASK)
items_col(ax, c2, top_nav, COL_W, ["黑板\nbb_bb1", "黑板\nbb_bb2"], C_BB)
items_col(ax, c3, top_nav, COL_W, ["缓冲区\nbuf_buf1"], C_BUF)

# ════════════════════════════════════════════════════════════════════════════
#  DISP 内容：左留端口区，右排任务+黑板
# ════════════════════════════════════════════════════════════════════════════
top_disp = DY + DH - TITLE_H - PAD   # 7.82
d1 = DX + PORT_OFFSET                  # 8.24
d2 = d1 + COL_W + COL_G               # 10.07

items_col(ax, d1, top_disp, COL_W, ["taskD1\n50ms  P2", "taskD2\n100ms P3"], C_TASK)
items_col(ax, d2, top_disp, COL_W, ["黑板\nbb_disp"], C_BB)

# ════════════════════════════════════════════════════════════════════════════
#  MON  内容
# ════════════════════════════════════════════════════════════════════════════
top_mon = MY + MH - TITLE_H - PAD   # 3.62
items_col(ax, MX + PAD, top_mon, COL_W, ["taskM1\n100ms P2"], C_TASK)
ax.text(MX + MW/2, MY + MH*0.32, "（无分区内共享资源）",
        ha='center', va='center', fontsize=8.5, color='#888888', style='italic')

# ════════════════════════════════════════════════════════════════════════════
#  CTRL 内容：左留端口区，右排任务+缓冲区
# ════════════════════════════════════════════════════════════════════════════
top_ctrl = TY + TH - TITLE_H - PAD   # 3.62
t1 = TX + PORT_OFFSET                  # 8.24
t2 = t1 + COL_W + COL_G               # 10.07

items_col(ax, t1, top_ctrl, COL_W,
          ["taskT1\n25ms  P2", "taskT2\n50ms  P3", "taskT3\n100ms P4"], C_TASK)
items_col(ax, t2, top_ctrl, COL_W, ["缓冲区\nbuf_ctrl"], C_BUF)

# ════════════════════════════════════════════════════════════════════════════
#  端口框 + 通信箭头
#
#  四条连接：
#    NAV.pos_out  → DISP.pos_in   采样  水平（NAV右 → DISP左）
#    NAV.status_out→ MON.status_in 采样  垂直（NAV底 → MON顶）
#    NAV.cmd_out  → CTRL.nav_in   队列  斜线（NAV右 → CTRL左）
#    CTRL.ctrl_out→ DISP.ctrl_in  队列  垂直（CTRL顶→ DISP底）
# ════════════════════════════════════════════════════════════════════════════

# 1. pos_out → pos_in   水平，上方
Y_POS = NY + NH * 0.80        # 7.67  (NAV / DISP 右/左 边中上)
p_pos_out = port_box(ax, NX + NW,  Y_POS, "pos_out", C_SAMPLE)
p_pos_in  = port_box(ax, DX,       Y_POS, "pos_in",  C_SAMPLE)
connect(ax, p_pos_out, p_pos_in, C_SAMPLE, "采样端口", lbl_offset=(0, 0.32))

# 2. status_out → status_in   垂直，左侧（NAV底边→MON顶边）
X_ST = NX + NW * 0.28          # 2.12
p_status_out = port_box(ax, X_ST, NY,      "status_out", C_SAMPLE)
p_status_in  = port_box(ax, X_ST, MY + MH, "status_in",  C_SAMPLE)
connect(ax, p_status_out, p_status_in, C_SAMPLE, "采样端口", lbl_offset=(0.75, 0))

# 3. cmd_out → nav_in   队列斜线（NAV右边→CTRL左边）
Y_CMD   = NY + NH * 0.40       # 6.21  NAV 右边中下
Y_NAVIN = TY + TH * 0.78       # 3.39  CTRL 左边中上
p_cmd_out = port_box(ax, NX + NW, Y_CMD,   "cmd_out", C_QUEUE)
p_nav_in  = port_box(ax, TX,      Y_NAVIN, "nav_in",  C_QUEUE)
connect(ax, p_cmd_out, p_nav_in, C_QUEUE, "队列端口",
        rad=-0.08, lbl_offset=(0.65, 0.28))

# 4. ctrl_out → ctrl_in   垂直，右侧（CTRL顶边→DISP底边）
X_CT = TX + TW * 0.55           # 10.89
p_ctrl_out = port_box(ax, X_CT, TY + TH, "ctrl_out", C_QUEUE)
p_ctrl_in  = port_box(ax, X_CT, DY,      "ctrl_in",  C_QUEUE)
connect(ax, p_ctrl_out, p_ctrl_in, C_QUEUE, "队列端口", lbl_offset=(-0.75, 0))

# ── 图例 ────────────────────────────────────────────────────────────────────
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
