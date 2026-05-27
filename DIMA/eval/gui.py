#!/usr/bin/env python3
"""
IMA 代码评估工具 — 图形化界面

运行方式：
    python DIMA/eval/gui.py

功能：
    Tab 1  单分区评估    — 选择目录和分区，查看 S/A/C 三维得分
    Tab 2  ARINC 653 合规检查 — 一键扫描全部实验，查看 R1-R8 结果
    Tab 3  编译检查解析  — 选择 compile_check_results.txt，查看错误分类
"""

import os
import sys
import json
import threading
from pathlib import Path

import tkinter as tk
from tkinter import ttk, filedialog, messagebox

# ── 模块路径 ──────────────────────────────────────────────────────────────────
_EVAL_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, _EVAL_DIR)

from evaluate import load_spec, evaluate as _evaluate
from batch_check import run_all as _run_compliance, save_csv as _save_csv
from parse_compile import parse_results as _parse_compile, CATEGORY_NAMES, DEFAULT_LOG

# ── 颜色 ──────────────────────────────────────────────────────────────────────
C_PASS   = "#27ae60"
C_WARN   = "#e67e22"
C_FAIL   = "#c0392b"
C_BG     = "#f0f2f5"
C_CARD   = "#ffffff"
C_SEP    = "#dfe6e9"
C_GROUP  = "#d6eaf8"

PARTITIONS = ["pa", "pb", "pc", "ps1", "ps2", "ps3", "ps4", "ps5"]
DIM_KEYS   = ["structural", "api", "semantic"]
DIM_NAMES  = {
    "structural": "维度1  结构完整性",
    "api":        "维度2  API 正确性",
    "semantic":   "维度3  语义一致性",
}
WEIGHTS = {"structural": 30, "api": 40, "semantic": 30}


def _score_color(score: float) -> str:
    if score >= 80: return C_PASS
    if score >= 50: return C_WARN
    return C_FAIL


# ─────────────────────────────────────────────────────────────────────────────
# 通用：带滚动条的 Treeview 容器
# ─────────────────────────────────────────────────────────────────────────────
def _make_tree(parent, columns, headings, widths, heights=16,
               stretch_last=True, horiz=False):
    frame = tk.Frame(parent, bg=C_BG)
    tree = ttk.Treeview(frame, columns=columns, show="headings", height=heights)
    for col, head, w in zip(columns, headings, widths):
        tree.heading(col, text=head)
        anchor = "center" if col not in (columns[0], columns[-1]) else "w"
        is_last = (col == columns[-1])
        tree.column(col, width=w, stretch=(stretch_last and is_last), anchor=anchor)

    vsb = ttk.Scrollbar(frame, orient="vertical", command=tree.yview)
    tree.configure(yscrollcommand=vsb.set)

    if horiz:
        xsb = ttk.Scrollbar(frame, orient="horizontal", command=tree.xview)
        tree.configure(xscrollcommand=xsb.set)
        xsb.pack(side="bottom", fill="x")

    tree.pack(side="left", fill="both", expand=True)
    vsb.pack(side="right", fill="y")
    return frame, tree


# ─────────────────────────────────────────────────────────────────────────────
# Tab 1：单分区评估
# ─────────────────────────────────────────────────────────────────────────────
class EvalTab(tk.Frame):

    def __init__(self, parent):
        super().__init__(parent, bg=C_BG)
        self._build()

    # ── 布局 ──
    def _build(self):
        # ── 左：参数控制 ──────────────────────────────────
        ctrl = ttk.LabelFrame(self, text="参数设置", padding=12)
        ctrl.pack(side="left", fill="y", padx=(10, 5), pady=10)

        # 目录
        ttk.Label(ctrl, text="生成代码目录").grid(
            row=0, column=0, columnspan=2, sticky="w", pady=(0, 2))
        self._dir_var = tk.StringVar()
        ttk.Entry(ctrl, textvariable=self._dir_var, width=30).grid(
            row=1, column=0, sticky="ew", padx=(0, 4))
        ttk.Button(ctrl, text="浏览", width=6,
                   command=self._browse_dir).grid(row=1, column=1, sticky="w")

        # 分区
        ttk.Label(ctrl, text="分区").grid(
            row=2, column=0, sticky="w", pady=(10, 2))
        self._part_var = tk.StringVar(value="pa")
        ttk.Combobox(ctrl, textvariable=self._part_var,
                     values=PARTITIONS, state="readonly", width=10).grid(
            row=3, column=0, sticky="w")

        # 标签
        ttk.Label(ctrl, text="实验标签（可选）").grid(
            row=4, column=0, sticky="w", pady=(10, 2))
        self._label_var = tk.StringVar()
        ttk.Entry(ctrl, textvariable=self._label_var, width=22).grid(
            row=5, column=0, columnspan=2, sticky="ew")

        # 保存
        self._save_var = tk.BooleanVar(value=False)
        ttk.Checkbutton(ctrl, text="保存结果（JSON）",
                        variable=self._save_var).grid(
            row=6, column=0, columnspan=2, sticky="w", pady=(12, 0))

        # 运行按钮
        self._run_btn = ttk.Button(ctrl, text="▶   开始评估",
                                   command=self._run, width=24)
        self._run_btn.grid(row=7, column=0, columnspan=2,
                           sticky="ew", pady=(14, 0))

        self._status_lbl = ttk.Label(ctrl, text="", foreground="#555",
                                     wraplength=200)
        self._status_lbl.grid(row=8, column=0, columnspan=2,
                               sticky="w", pady=(8, 0))
        ctrl.columnconfigure(0, weight=1)

        # ── 右：结果区 ────────────────────────────────────
        right = tk.Frame(self, bg=C_BG)
        right.pack(side="left", fill="both", expand=True,
                   padx=(5, 10), pady=10)

        # 得分卡
        cards_lf = ttk.LabelFrame(right, text="评分结果", padding=8)
        cards_lf.pack(fill="x")

        self._cards = {}
        card_defs = [
            ("structural", DIM_NAMES["structural"], "权重 30%"),
            ("api",        DIM_NAMES["api"],        "权重 40%"),
            ("semantic",   DIM_NAMES["semantic"],   "权重 30%"),
            ("total",      "综合得分",              "加权平均"),
        ]
        for col, (key, title, sub) in enumerate(card_defs):
            card = self._make_score_card(cards_lf, title, sub)
            card.grid(row=0, column=col, padx=5, pady=4, sticky="nsew")
            self._cards[key] = card
            cards_lf.columnconfigure(col, weight=1)

        self._grade_lbl = ttk.Label(cards_lf, text="",
                                    font=("", 10, "bold"))
        self._grade_lbl.grid(row=1, column=0, columnspan=4,
                             pady=(6, 2), sticky="w", padx=6)

        # 明细
        detail_lf = ttk.LabelFrame(right, text="检查明细", padding=6)
        detail_lf.pack(fill="both", expand=True, pady=(8, 0))

        cols = ("id", "desc", "status", "score", "note")
        heads = ("项目", "检查内容", "状态", "得分", "备注")
        widths = (48, 210, 48, 55, 260)
        tf, self._tree = _make_tree(detail_lf, cols, heads, widths, heights=14)
        self._tree.column("desc", anchor="w")
        self._tree.column("note", anchor="w")
        self._tree.tag_configure("pass",    background="#d5f5e3")
        self._tree.tag_configure("partial", background="#fef9e7")
        self._tree.tag_configure("fail",    background="#fadbd8")
        self._tree.tag_configure("sep",     background=C_SEP,
                                 font=("", 9, "bold"))
        tf.pack(fill="both", expand=True)

    def _make_score_card(self, parent, title, sub_label):
        """返回一个带进度条和分数标签的 Frame。"""
        card = tk.Frame(parent, bg=C_CARD, relief="groove", bd=1)
        tk.Label(card, text=title, bg=C_CARD,
                 font=("", 9), wraplength=160).pack(
            anchor="w", padx=8, pady=(6, 2))

        row = tk.Frame(card, bg=C_CARD)
        row.pack(fill="x", padx=8)

        bar = ttk.Progressbar(row, maximum=100, mode="determinate", length=160)
        bar.pack(side="left", fill="x", expand=True)

        score_lbl = tk.Label(row, text="—", width=5, bg=C_CARD,
                             font=("", 11, "bold"), fg="#aaa")
        score_lbl.pack(side="left", padx=(6, 0))

        tk.Label(card, text=sub_label, bg=C_CARD,
                 font=("", 8), fg="#888").pack(
            anchor="e", padx=8, pady=(2, 6))

        card._bar = bar
        card._lbl = score_lbl
        return card

    # ── 控件操作 ──
    def _browse_dir(self):
        d = filedialog.askdirectory(title="选择包含生成代码的目录")
        if d:
            self._dir_var.set(d)

    def _run(self):
        gen_dir   = self._dir_var.get().strip()
        partition = self._part_var.get()

        if not gen_dir:
            messagebox.showwarning("提示", "请先选择生成代码目录")
            return
        if not os.path.isdir(gen_dir):
            messagebox.showerror("错误", f"目录不存在：\n{gen_dir}")
            return

        # 重置 UI
        self._run_btn.state(["disabled"])
        self._status_lbl.config(text="评估中，请稍候…")
        self._grade_lbl.config(text="")
        for card in self._cards.values():
            card._bar["value"] = 0
            card._lbl.config(text="—", fg="#aaa")
        self._tree.delete(*self._tree.get_children())

        def worker():
            try:
                spec   = load_spec(partition)
                result = _evaluate(gen_dir, spec)
                if self._label_var.get():
                    result["experiment"] = self._label_var.get()
                if self._save_var.get():
                    self._do_save(result, partition)
                self.after(0, lambda: self._show(result))
            except SystemExit:
                self.after(0, lambda: messagebox.showerror(
                    "错误", f"找不到分区 {partition} 的规格文件"))
                self.after(0, self._reset_btn)
            except Exception as e:
                self.after(0, lambda: messagebox.showerror("评估失败", str(e)))
                self.after(0, self._reset_btn)

        threading.Thread(target=worker, daemon=True).start()

    def _reset_btn(self):
        self._run_btn.state(["!disabled"])
        self._status_lbl.config(text="")

    def _do_save(self, result, partition):
        from datetime import datetime
        out_dir = os.path.join(_EVAL_DIR, "results")
        os.makedirs(out_dir, exist_ok=True)
        label = result.get("experiment", "unnamed")
        ts    = datetime.now().strftime("%Y%m%d_%H%M%S")
        path  = os.path.join(out_dir, f"{label}_{partition}_{ts}.json")
        with open(path, "w", encoding="utf-8") as f:
            json.dump(result, f, ensure_ascii=False, indent=2)

    def _show(self, result):
        s = result["scores"]

        # 更新得分卡
        for key in ("structural", "api", "semantic", "total"):
            score = s[key]
            card  = self._cards[key]
            card._bar["value"] = score
            card._lbl.config(text=f"{score:.1f}",
                             fg=_score_color(score))

        # 评级
        total = s["total"]
        if total >= 80:
            grade, clr = "优秀  ─  达到参考代码标准", C_PASS
        elif total >= 60:
            grade, clr = "良好  ─  主要结构正确，存在细节偏差", C_WARN
        elif total >= 40:
            grade, clr = "一般  ─  基本框架正确，API / 语义有较多缺失", C_WARN
        else:
            grade, clr = "较差  ─  结构性错误或大量关键项缺失", C_FAIL
        self._grade_lbl.config(text=f"综合评级：{grade}", foreground=clr)

        # 明细
        for dim in DIM_KEYS:
            self._tree.insert("", "end",
                              values=("", f"── {DIM_NAMES[dim]} ──",
                                      "", "", ""),
                              tags=("sep",))
            for d in result["details"][dim]:
                if d["passed"]:
                    icon, tag = "✓", "pass"
                elif d["partial"] > 0.3:
                    icon, tag = "△", "partial"
                else:
                    icon, tag = "✗", "fail"
                self._tree.insert("", "end",
                                  values=(d["id"], d["desc"], icon,
                                          f"{d['partial']*100:.0f}%",
                                          d.get("note", "")),
                                  tags=(tag,))

        saved_note = "  （已保存）" if self._save_var.get() else ""
        self._status_lbl.config(
            text=f"评估完成  综合得分 {total:.1f} 分{saved_note}")
        self._run_btn.state(["!disabled"])


# ─────────────────────────────────────────────────────────────────────────────
# Tab 2：ARINC 653 合规检查
# ─────────────────────────────────────────────────────────────────────────────
class ComplianceTab(tk.Frame):

    def __init__(self, parent):
        super().__init__(parent, bg=C_BG)
        self._build()

    def _build(self):
        # 控制行
        ctrl = tk.Frame(self, bg=C_BG)
        ctrl.pack(fill="x", padx=10, pady=(10, 4))

        self._save_var = tk.BooleanVar(value=False)
        ttk.Checkbutton(ctrl, text="保存 CSV",
                        variable=self._save_var).pack(side="left", padx=(0, 12))

        self._run_btn = ttk.Button(ctrl, text="▶   开始检查",
                                   command=self._run)
        self._run_btn.pack(side="left")

        self._status_lbl = ttk.Label(ctrl, text="")
        self._status_lbl.pack(side="left", padx=12)

        # 结果表
        tree_lf = ttk.LabelFrame(self, text="合规检查结果（全部实验）",
                                  padding=6)
        tree_lf.pack(fill="both", expand=True, padx=10, pady=(0, 4))

        cols   = ("exp", "part", "passed", "total", "pct", "failed")
        heads  = ("实验策略", "分区", "通过", "总计", "合规率", "失败规则")
        widths = (100, 58, 52, 52, 68, 360)
        tf, self._tree = _make_tree(tree_lf, cols, heads, widths, heights=18)
        self._tree.column("exp",    anchor="w")
        self._tree.column("failed", anchor="w")
        self._tree.tag_configure("pass",  background="#d5f5e3")
        self._tree.tag_configure("warn",  background="#fef9e7")
        self._tree.tag_configure("fail",  background="#fadbd8")
        self._tree.tag_configure("group", background=C_GROUP,
                                 font=("", 9, "bold"))
        tf.pack(fill="both", expand=True)

        # 平均合规率条
        self._avg_lf = ttk.LabelFrame(self, text="各策略平均合规率",
                                       padding=8)
        self._avg_lf.pack(fill="x", padx=10, pady=(0, 10))

    def _run(self):
        self._run_btn.state(["disabled"])
        self._status_lbl.config(text="检查中，请稍候…")
        self._tree.delete(*self._tree.get_children())
        for w in self._avg_lf.winfo_children():
            w.destroy()

        def worker():
            try:
                results = _run_compliance()
                self.after(0, lambda: self._show(results))
            except Exception as e:
                self.after(0, lambda: messagebox.showerror("检查失败", str(e)))
                self.after(0, lambda: self._run_btn.state(["!disabled"]))
                self.after(0, lambda: self._status_lbl.config(text="失败"))

        threading.Thread(target=worker, daemon=True).start()

    def _show(self, results):
        from collections import defaultdict

        prev_exp = None
        for r in results:
            # 实验分组行
            if r["experiment"] != prev_exp:
                prev_exp = r["experiment"]
                self._tree.insert("", "end",
                                  values=(r["experiment"].upper(),
                                          "", "", "", "", ""),
                                  tags=("group",))
            pct = r["pct"]
            tag = "pass" if pct >= 100 else ("warn" if pct >= 75 else "fail")
            failed_str = ("  ".join(
                f"[{f['rule']}] {f['item']}" for f in r["failed"])
                if r["failed"] else "✓ 全部通过")
            self._tree.insert("", "end", values=(
                r["experiment"], r["partition"],
                r["passed"], r["total"],
                f"{pct:.0f}%", failed_str,
            ), tags=(tag,))

        # 策略平均合规率
        exp_stats = defaultdict(list)
        for r in results:
            exp_stats[r["experiment"]].append(r["pct"])

        for col, (exp, pcts) in enumerate(exp_stats.items()):
            avg = sum(pcts) / len(pcts)
            clr = C_PASS if avg >= 100 else (C_WARN if avg >= 75 else C_FAIL)
            ttk.Label(self._avg_lf,
                      text=f"{exp}").grid(row=0, column=col * 2,
                                          padx=(16, 4))
            tk.Label(self._avg_lf, text=f"{avg:.1f}%",
                     fg=clr, font=("", 11, "bold"),
                     bg=C_BG).grid(row=0, column=col * 2 + 1,
                                   padx=(0, 20))

        # 保存 CSV
        if self._save_var.get():
            out = os.path.join(_EVAL_DIR, "results", "arinc_compliance.csv")
            os.makedirs(os.path.dirname(out), exist_ok=True)
            _save_csv(results, out)
            self._status_lbl.config(text=f"完成  CSV 已保存：{out}")
        else:
            n_pass = sum(1 for r in results if r["pct"] >= 100)
            self._status_lbl.config(
                text=f"完成  {n_pass}/{len(results)} 组全部通过")

        self._run_btn.state(["!disabled"])


# ─────────────────────────────────────────────────────────────────────────────
# Tab 3：编译检查解析
# ─────────────────────────────────────────────────────────────────────────────
class CompileTab(tk.Frame):

    def __init__(self, parent):
        super().__init__(parent, bg=C_BG)
        self._build()

    def _build(self):
        # 控制行
        ctrl = tk.Frame(self, bg=C_BG)
        ctrl.pack(fill="x", padx=10, pady=(10, 4))

        ttk.Label(ctrl, text="结果文件：").pack(side="left")
        self._file_var = tk.StringVar(value=str(DEFAULT_LOG))
        ttk.Entry(ctrl, textvariable=self._file_var, width=46).pack(
            side="left", padx=(0, 4))
        ttk.Button(ctrl, text="浏览", width=6,
                   command=self._browse).pack(side="left", padx=(0, 14))
        self._run_btn = ttk.Button(ctrl, text="▶   解析",
                                   command=self._run)
        self._run_btn.pack(side="left")
        self._status_lbl = ttk.Label(ctrl, text="")
        self._status_lbl.pack(side="left", padx=10)

        # 结果表（带水平滚动）
        tree_lf = ttk.LabelFrame(self, text="编译检查结果", padding=6)
        tree_lf.pack(fill="both", expand=True, padx=10, pady=(0, 10))

        base_cols = ["strategy", "partition", "errors", "warnings", "passed"]
        all_cols  = base_cols + CATEGORY_NAMES
        base_heads = ["策略", "分区", "错误", "警告", "通过"]
        all_heads  = base_heads + CATEGORY_NAMES
        base_w     = [90, 58, 52, 52, 52]
        cat_w      = [68] * len(CATEGORY_NAMES)
        all_w      = base_w + cat_w

        tf, self._tree = _make_tree(
            tree_lf, all_cols, all_heads, all_w,
            heights=20, stretch_last=False, horiz=True)
        self._tree.column("strategy", anchor="w", width=90)
        self._tree.tag_configure("pass", background="#d5f5e3")
        self._tree.tag_configure("fail", background="#fadbd8")
        tf.pack(fill="both", expand=True)

    def _browse(self):
        f = filedialog.askopenfilename(
            title="选择编译检查结果文件",
            filetypes=[("文本文件", "*.txt"), ("所有文件", "*.*")])
        if f:
            self._file_var.set(f)

    def _run(self):
        fpath = self._file_var.get().strip()
        if not os.path.isfile(fpath):
            messagebox.showerror("错误", f"文件不存在：\n{fpath}")
            return

        self._run_btn.state(["disabled"])
        self._status_lbl.config(text="解析中…")
        self._tree.delete(*self._tree.get_children())

        def worker():
            try:
                results = _parse_compile(Path(fpath))
                self.after(0, lambda: self._show(results))
            except Exception as e:
                self.after(0, lambda: messagebox.showerror("解析失败", str(e)))
                self.after(0, lambda: self._run_btn.state(["!disabled"]))

        threading.Thread(target=worker, daemon=True).start()

    def _show(self, results):
        n_pass = 0
        for r in results:
            cats = [r["cat_counts"].get(n, 0) for n in CATEGORY_NAMES]
            tag  = "pass" if r["passed"] else "fail"
            if r["passed"]:
                n_pass += 1
            self._tree.insert("", "end", values=(
                r["strategy"], r["partition"],
                r["errors"], r["warnings"],
                "✓" if r["passed"] else "✗",
                *[c if c else "" for c in cats],
            ), tags=(tag,))

        self._status_lbl.config(
            text=f"解析完成  {n_pass}/{len(results)} 组通过编译")
        self._run_btn.state(["!disabled"])


# ─────────────────────────────────────────────────────────────────────────────
# 主窗口
# ─────────────────────────────────────────────────────────────────────────────
def main():
    root = tk.Tk()
    root.title("IMA 代码评估工具")
    root.geometry("1000x680")
    root.configure(bg=C_BG)
    root.resizable(True, True)

    style = ttk.Style()
    try:
        style.theme_use("clam")
    except tk.TclError:
        pass
    style.configure("TNotebook.Tab",    padding=(14, 5), font=("", 10))
    style.configure("TLabelframe.Label", font=("", 9, "bold"))
    style.configure("TButton",           padding=(6, 4))

    nb = ttk.Notebook(root)
    nb.pack(fill="both", expand=True, padx=6, pady=(6, 4))

    tab1 = EvalTab(nb)
    tab2 = ComplianceTab(nb)
    tab3 = CompileTab(nb)

    nb.add(tab1, text="  单分区评估  ")
    nb.add(tab2, text="  ARINC 653 合规检查  ")
    nb.add(tab3, text="  编译检查解析  ")

    # 底部版本信息
    tk.Label(root, text="IMA 代码评估工具  v1.0   —   DIMA/eval/gui.py",
             bg=C_BG, fg="#aaa", font=("", 8)).pack(
        side="bottom", anchor="e", padx=10, pady=2)

    root.mainloop()


if __name__ == "__main__":
    main()
