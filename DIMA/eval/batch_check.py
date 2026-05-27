#!/usr/bin/env python3
"""
DIMA 全实验 ARINC 653 合规批量检查

对所有已完成实验（zeroshot/cot × ps1-ps5，fewshot/combined × pa/pb/pc）
运行 ARINC 653 合规规则检查（R1-R8），打印汇总报告。

用法：
    python DIMA/eval/batch_check.py
    python DIMA/eval/batch_check.py --verbose   # 显示每条规则详情
    python DIMA/eval/batch_check.py --save      # 保存 CSV 报告

输出示例：
    实验         分区   通过/总计  合规率   问题
    zeroshot     ps1    8/8      100.0%
    fewshot      pb     7/8       87.5%  [R1] 黑板 bb_data_link 未出现在 activity.c
"""

import argparse
import csv
import json
import os
import sys

# 确保可以 import metrics 包
sys.path.insert(0, os.path.dirname(__file__))
from metrics import arinc_compliance

# ── 实验配置 ──────────────────────────────────────────────────────────────────
BASE = os.path.dirname(os.path.abspath(__file__))

EXPERIMENTS = {
    "zeroshot": ["ps1", "ps2", "ps3", "ps4", "ps5"],
    "cot":      ["ps1", "ps2", "ps3", "ps4", "ps5"],
    "fewshot":  ["pa",  "pb",  "pc",  "nav", "disp", "ctrl", "mon"],
    "combined": ["pa",  "pb",  "pc",  "nav", "disp", "ctrl", "mon"],
}

# ANSI 颜色（Windows cmd 可能不支持，自动降级）
try:
    import ctypes
    ctypes.windll.kernel32.SetConsoleMode(
        ctypes.windll.kernel32.GetStdHandle(-11), 7)
except Exception:
    pass

GREEN  = "\033[92m"
YELLOW = "\033[93m"
RED    = "\033[91m"
BOLD   = "\033[1m"
RESET  = "\033[0m"


def load_spec(part: str) -> dict:
    path = os.path.join(BASE, "specs", f"{part}.json")
    if not os.path.isfile(path):
        print(f"[WARN] 找不到规格文件: {path}，跳过", file=sys.stderr)
        return None
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def color_pct(pct: float) -> str:
    if pct >= 100:
        return f"{GREEN}{pct:.1f}%{RESET}"
    elif pct >= 75:
        return f"{YELLOW}{pct:.1f}%{RESET}"
    else:
        return f"{RED}{pct:.1f}%{RESET}"


def run_all(verbose: bool = False) -> list:
    """运行所有实验的合规检查，返回结果列表"""
    all_results = []

    for exp, parts in EXPERIMENTS.items():
        for part in parts:
            gen_dir = os.path.join(BASE, "generated", exp, part)
            spec = load_spec(part)
            if spec is None:
                continue

            if not os.path.isdir(gen_dir):
                print(f"[WARN] 目录不存在: {gen_dir}，跳过", file=sys.stderr)
                continue

            checks = arinc_compliance.check(gen_dir, spec)
            passed = sum(1 for c in checks if c["pass"])
            total  = len(checks)
            failed = [c for c in checks if not c["pass"]]

            all_results.append({
                "experiment": exp,
                "partition":  part,
                "passed":     passed,
                "total":      total,
                "pct":        passed / total * 100 if total else 100.0,
                "failed":     failed,
                "checks":     checks,
            })

            if verbose:
                print(f"\n{'─'*60}")
                print(f"{BOLD}{exp}/{part}{RESET}")
                for c in checks:
                    mark = f"{GREEN}✓{RESET}" if c["pass"] else f"{RED}✗{RESET}"
                    print(f"  {mark} [{c['rule']:5s}] {c['msg']}")

    return all_results


def print_summary(all_results: list):
    """打印汇总表格"""
    print(f"\n{BOLD}{'='*72}{RESET}")
    print(f"{BOLD}  ARINC 653 合规检查汇总{RESET}")
    print(f"{BOLD}{'='*72}{RESET}")
    print(f"  {'实验':<12} {'分区':<5} {'通过/总计':>8}  {'合规率':>7}  问题")
    print(f"  {'-'*65}")

    prev_exp = None
    for r in all_results:
        if r["experiment"] != prev_exp:
            if prev_exp is not None:
                print()
            prev_exp = r["experiment"]

        problems = "  ".join(
            f"[{f['rule']}] {f['item']}" for f in r["failed"]
        ) if r["failed"] else ""

        pct_str = color_pct(r["pct"])
        print(f"  {r['experiment']:<12} {r['partition']:<5} "
              f"{r['passed']:>4}/{r['total']:<4}  {pct_str:>7}  {problems}")

    print(f"\n{BOLD}{'='*72}{RESET}")

    # 按实验策略统计平均合规率
    from collections import defaultdict
    exp_stats = defaultdict(list)
    for r in all_results:
        exp_stats[r["experiment"]].append(r["pct"])

    print(f"\n{BOLD}  策略平均合规率{RESET}")
    print(f"  {'-'*35}")
    for exp, pcts in exp_stats.items():
        avg = sum(pcts) / len(pcts)
        print(f"  {exp:<12}  {color_pct(avg)}")
    print()


def save_csv(all_results: list, path: str):
    """保存详细结果到 CSV"""
    rows = []
    for r in all_results:
        for c in r["checks"]:
            rows.append({
                "experiment": r["experiment"],
                "partition":  r["partition"],
                "rule":       c["rule"],
                "item":       c["item"],
                "pass":       "PASS" if c["pass"] else "FAIL",
                "message":    c["msg"],
            })

    with open(path, "w", newline="", encoding="utf-8-sig") as f:
        writer = csv.DictWriter(f, fieldnames=["experiment","partition","rule","item","pass","message"])
        writer.writeheader()
        writer.writerows(rows)

    print(f"[INFO] 合规检查结果已保存到: {path}")


def main():
    parser = argparse.ArgumentParser(
        description="DIMA 全实验 ARINC 653 合规批量检查")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="显示每条规则的详细结果")
    parser.add_argument("--save", "-s", action="store_true",
                        help="保存 CSV 报告到 results/arinc_compliance.csv")
    args = parser.parse_args()

    print(f"\n{BOLD}[INFO] 扫描所有实验结果...{RESET}")
    all_results = run_all(verbose=args.verbose)

    if not all_results:
        print("[ERROR] 未找到任何生成代码，请先运行实验。")
        sys.exit(1)

    print_summary(all_results)

    if args.save:
        out_dir = os.path.join(BASE, "results")
        os.makedirs(out_dir, exist_ok=True)
        csv_path = os.path.join(out_dir, "arinc_compliance.csv")
        save_csv(all_results, csv_path)


if __name__ == "__main__":
    main()
