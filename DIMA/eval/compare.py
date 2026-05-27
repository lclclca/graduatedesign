#!/usr/bin/env python3
"""
DIMA 实验结果比较脚本

用法：
    # 比较同一分区的4种实验结果
    python compare.py results/zeroshot_ps1_*.json results/fewshot_ps1_*.json \\
                      results/cot_ps1_*.json results/combined_ps1_*.json

    # 比较所有分区（批量）
    python compare.py --dir results/ --partition ps1

    # 生成 CSV（导入 Excel 制图）
    python compare.py --dir results/ --csv output.csv
"""

import argparse
import glob
import json
import os
import sys
from collections import defaultdict


GREEN  = "\033[92m"
YELLOW = "\033[93m"
RED    = "\033[91m"
BOLD   = "\033[1m"
CYAN   = "\033[96m"
RESET  = "\033[0m"

EXPERIMENTS = ["zeroshot", "fewshot", "cot", "combined"]
PARTITIONS  = ["ps1", "ps2", "ps3", "ps4", "ps5"]

EXP_NAMES = {
    "zeroshot": "零样本",
    "fewshot":  "少样本",
    "cot":      "思维链",
    "combined": "组合",
}


def load_results(paths: list) -> list:
    results = []
    for p in paths:
        with open(p, encoding="utf-8") as f:
            results.append(json.load(f))
    return results


def collect_from_dir(results_dir: str) -> dict:
    """
    从 results/ 目录收集所有结果。
    返回结构: {experiment: {partition: result_dict}}
    """
    data = defaultdict(dict)
    for exp in EXPERIMENTS:
        for part in PARTITIONS:
            pattern = os.path.join(results_dir, f"{exp}_{part}_*.json")
            files = sorted(glob.glob(pattern))
            if files:
                # 取最新的一个
                with open(files[-1], encoding="utf-8") as f:
                    data[exp][part] = json.load(f)
    return data


def print_comparison_table(data: dict, partition: str = None):
    """打印各实验在各分区的得分对比表"""
    parts = [partition] if partition else PARTITIONS

    print()
    print(f"{BOLD}{'='*72}{RESET}")
    print(f"{BOLD}  实验结果对比表{RESET}")
    print(f"{BOLD}{'='*72}{RESET}")

    # 表头
    col_w = 12
    header = f"  {'实验':8}"
    if not partition:
        for p in parts:
            header += f"  {p:^{col_w}}"
        header += f"  {'平均':^{col_w}}"
    else:
        header += f"  {'结构(30%)':^{col_w}} {'API(40%)':^{col_w}} {'语义(30%)':^{col_w}} {'综合':^{col_w}}"
    print(header)
    print(f"  {'-'*68}")

    for exp in EXPERIMENTS:
        if exp not in data:
            continue
        row = f"  {EXP_NAMES[exp]:8}({exp[:3]})"
        scores = []

        if not partition:
            # 横向：各分区综合得分
            for p in parts:
                if p in data[exp]:
                    sc = data[exp][p]["scores"]["total"]
                    scores.append(sc)
                    clr = GREEN if sc >= 80 else (YELLOW if sc >= 50 else RED)
                    row += f"  {clr}{sc:^{col_w}.1f}{RESET}"
                else:
                    row += f"  {'N/A':^{col_w}}"
            if scores:
                avg = sum(scores) / len(scores)
                clr = GREEN if avg >= 80 else (YELLOW if avg >= 50 else RED)
                row += f"  {BOLD}{clr}{avg:^{col_w}.1f}{RESET}"
        else:
            # 纵向：单分区的三维度得分
            if partition in data[exp]:
                sc = data[exp][partition]["scores"]
                for key, label in [("structural", None), ("api", None),
                                   ("semantic", None), ("total", None)]:
                    v = sc[key]
                    clr = GREEN if v >= 80 else (YELLOW if v >= 50 else RED)
                    bold = BOLD if key == "total" else ""
                    row += f"  {bold}{clr}{v:^{col_w}.1f}{RESET}"
            else:
                row += f"  {'N/A':^{col_w}}" * 4

        print(row)

    print(f"{BOLD}{'='*72}{RESET}")
    print()

    # 改进幅度分析
    if "zeroshot" in data and "combined" in data:
        print(f"{BOLD}  零样本 → 组合实验改进幅度:{RESET}")
        for p in parts:
            if p in data["zeroshot"] and p in data["combined"]:
                base  = data["zeroshot"][p]["scores"]["total"]
                final = data["combined"][p]["scores"]["total"]
                delta = final - base
                clr   = GREEN if delta > 0 else (RED if delta < 0 else YELLOW)
                print(f"    {p}: {base:.1f} → {final:.1f}  {clr}({'+' if delta>0 else ''}{delta:.1f}){RESET}")
        print()


def print_dimension_breakdown(data: dict):
    """打印各实验在三个维度的平均得分（雷达图数据）"""
    print(f"{BOLD}  三维度平均得分（可用于雷达图）:{RESET}")
    print(f"  {'实验':12} {'结构完整性':>10} {'API正确性':>10} {'语义一致性':>10}")
    print(f"  {'-'*48}")
    for exp in EXPERIMENTS:
        if exp not in data:
            continue
        scores_s, scores_a, scores_c = [], [], []
        for p in PARTITIONS:
            if p in data[exp]:
                scores_s.append(data[exp][p]["scores"]["structural"])
                scores_a.append(data[exp][p]["scores"]["api"])
                scores_c.append(data[exp][p]["scores"]["semantic"])
        if scores_s:
            avg_s = sum(scores_s) / len(scores_s)
            avg_a = sum(scores_a) / len(scores_a)
            avg_c = sum(scores_c) / len(scores_c)
            print(f"  {EXP_NAMES[exp]:12} {avg_s:>10.1f} {avg_a:>10.1f} {avg_c:>10.1f}")
    print()


def export_csv(data: dict, csv_path: str):
    """导出 CSV，每行 = 一次实验 × 一个分区的完整得分"""
    import csv
    rows = []
    for exp in EXPERIMENTS:
        if exp not in data:
            continue
        for p in PARTITIONS:
            if p not in data[exp]:
                continue
            sc = data[exp][p]["scores"]
            rows.append({
                "实验": exp,
                "实验名称": EXP_NAMES[exp],
                "分区": p,
                "结构完整性": sc["structural"],
                "API正确性": sc["api"],
                "语义一致性": sc["semantic"],
                "综合得分": sc["total"],
            })
    with open(csv_path, "w", newline="", encoding="utf-8-sig") as f:
        writer = csv.DictWriter(f, fieldnames=rows[0].keys())
        writer.writeheader()
        writer.writerows(rows)
    print(f"  CSV 已导出: {csv_path}")


def main():
    parser = argparse.ArgumentParser(description="比较多组 IMA C代码评估实验结果")
    parser.add_argument("files", nargs="*", help="直接指定 JSON 结果文件")
    parser.add_argument("--dir", "-d", default=None,
                        help="自动从 results/ 目录读取所有实验结果")
    parser.add_argument("--partition", "-p", default=None,
                        choices=PARTITIONS + [None],
                        help="只显示指定分区的详细维度比较")
    parser.add_argument("--csv", default=None,
                        help="同时导出 CSV 文件路径（如 output.csv）")
    args = parser.parse_args()

    if args.dir:
        data = collect_from_dir(args.dir)
    elif args.files:
        # 从文件名推断实验和分区
        data = defaultdict(dict)
        for f in args.files:
            r = json.load(open(f, encoding="utf-8"))
            exp  = r.get("experiment", os.path.basename(f).split("_")[0])
            part = r.get("partition",  os.path.basename(f).split("_")[1])
            data[exp][part] = r
    else:
        # 默认读取 eval/results/
        default_dir = os.path.join(os.path.dirname(__file__), "results")
        data = collect_from_dir(default_dir)

    if not data:
        print("[INFO] results/ 目录下暂无结果文件。先运行 evaluate.py --save 生成结果。")
        sys.exit(0)

    print_comparison_table(data, args.partition)
    print_dimension_breakdown(data)

    if args.csv:
        export_csv(data, args.csv)


if __name__ == "__main__":
    main()
