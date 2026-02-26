#!/usr/bin/env python3
"""
DIMA IMA C代码评估脚本 — 单分区评估

用法:
    python evaluate.py --generated <生成代码目录> --partition <psX>
    python evaluate.py --generated ./my_gen/ps1  --partition ps1
    python evaluate.py --generated ./my_gen/ps1  --partition ps1 --save

输出：控制台打印详细评分报告；--save 时同时写入 results/<实验名>_<分区>.json
"""

import argparse
import json
import os
import sys
from datetime import datetime

# 确保可以 import metrics 包（无论从哪里调用此脚本）
sys.path.insert(0, os.path.dirname(__file__))

from metrics import structural, api_check, semantic

# 三个维度的权重（总和 = 1.0）
WEIGHTS = {
    "structural": 0.30,   # 结构完整性
    "api":        0.40,   # API 正确性
    "semantic":   0.30,   # 语义一致性
}

DIMENSION_NAMES = {
    "structural": "维度1 结构完整性",
    "api":        "维度2 API正确性",
    "semantic":   "维度3 语义一致性",
}

# ANSI 颜色（终端输出用）
GREEN  = "\033[92m"
YELLOW = "\033[93m"
RED    = "\033[91m"
BOLD   = "\033[1m"
RESET  = "\033[0m"


def color_score(score: float) -> str:
    if score >= 80:
        return f"{GREEN}{score:.1f}{RESET}"
    elif score >= 50:
        return f"{YELLOW}{score:.1f}{RESET}"
    else:
        return f"{RED}{score:.1f}{RESET}"


def load_spec(partition: str) -> dict:
    spec_path = os.path.join(os.path.dirname(__file__), "specs", f"{partition}.json")
    if not os.path.isfile(spec_path):
        print(f"[ERROR] 找不到规格文件: {spec_path}", file=sys.stderr)
        sys.exit(1)
    with open(spec_path) as f:
        return json.load(f)


def evaluate(gen_dir: str, spec: dict) -> dict:
    """运行三个维度的评估，返回完整结果字典"""
    if not os.path.isdir(gen_dir):
        print(f"[ERROR] 目录不存在: {gen_dir}", file=sys.stderr)
        sys.exit(1)

    r_struct = structural.check(gen_dir, spec)
    r_api    = api_check.check(gen_dir, spec)
    r_sem    = semantic.check(gen_dir, spec)

    # 加权总分
    total = (r_struct["score"] * WEIGHTS["structural"] +
             r_api["score"]    * WEIGHTS["api"] +
             r_sem["score"]    * WEIGHTS["semantic"])

    return {
        "partition":  spec["partition"],
        "gen_dir":    gen_dir,
        "timestamp":  datetime.now().isoformat(timespec="seconds"),
        "scores": {
            "structural": r_struct["score"],
            "api":        r_api["score"],
            "semantic":   r_sem["score"],
            "total":      round(total, 1),
        },
        "details": {
            "structural": r_struct["details"],
            "api":        r_api["details"],
            "semantic":   r_sem["details"],
        }
    }


def print_report(result: dict):
    """向终端打印格式化报告"""
    p = result["partition"]
    s = result["scores"]

    print()
    print(f"{BOLD}{'='*60}{RESET}")
    print(f"{BOLD}  IMA C代码评估报告 — 分区 {p.upper()}{RESET}")
    print(f"  生成路径: {result['gen_dir']}")
    print(f"  评估时间: {result['timestamp']}")
    print(f"{BOLD}{'='*60}{RESET}")

    # 维度汇总
    dim_map = [("structural", s["structural"]),
               ("api",        s["api"]),
               ("semantic",   s["semantic"])]
    for key, score in dim_map:
        name  = DIMENSION_NAMES[key]
        w_pct = int(WEIGHTS[key] * 100)
        bar_filled = int(score / 5)
        bar = "█" * bar_filled + "░" * (20 - bar_filled)
        print(f"  {name:<22} (权重{w_pct:2d}%)  [{bar}] {color_score(score)}")

    total_bar = int(s["total"] / 5)
    bar = "█" * total_bar + "░" * (20 - total_bar)
    print(f"  {'综合得分':<22} {'':5}  [{bar}] {BOLD}{color_score(s['total'])}{RESET}")
    print()

    # 各维度明细
    for key in ("structural", "api", "semantic"):
        print(f"{BOLD}  {DIMENSION_NAMES[key]} 明细:{RESET}")
        for d in result["details"][key]:
            icon = "✓" if d["passed"] else ("△" if d["partial"] > 0.3 else "✗")
            clr  = GREEN if d["passed"] else (YELLOW if d["partial"] > 0.3 else RED)
            pct  = f"{d['partial']*100:5.1f}%"
            print(f"    {clr}{icon}{RESET} [{d['id']}] {d['desc']}")
            if d["note"]:
                print(f"         → {d['note']}")
        print()

    print(f"{BOLD}{'='*60}{RESET}")
    total_score = s["total"]
    if total_score >= 80:
        grade = f"{GREEN}优秀（达到参考代码标准）{RESET}"
    elif total_score >= 60:
        grade = f"{YELLOW}良好（主要结构正确，存在细节偏差）{RESET}"
    elif total_score >= 40:
        grade = f"{YELLOW}一般（基本框架正确，API/语义有较多缺失）{RESET}"
    else:
        grade = f"{RED}较差（结构性错误或大量关键项缺失）{RESET}"
    print(f"  综合评级: {grade}")
    print(f"{BOLD}{'='*60}{RESET}")
    print()


def main():
    parser = argparse.ArgumentParser(
        description="评估 AI 生成的 IMA ARINC 653 C 代码质量")
    parser.add_argument("--generated", "-g", required=True,
                        help="生成代码所在目录（应包含 11 个文件）")
    parser.add_argument("--partition", "-p", required=True,
                        choices=["ps1", "ps2", "ps3", "ps4", "ps5"],
                        help="要评估的分区名称")
    parser.add_argument("--save", "-s", action="store_true",
                        help="将结果保存到 results/ 目录（JSON格式）")
    parser.add_argument("--label", "-l", default=None,
                        help="实验标签（如 zeroshot/fewshot/cot/combined），影响保存文件名")
    args = parser.parse_args()

    spec   = load_spec(args.partition)
    result = evaluate(args.generated, spec)

    # 加上实验标签
    if args.label:
        result["experiment"] = args.label

    print_report(result)

    if args.save:
        label = args.label or "unnamed"
        out_dir = os.path.join(os.path.dirname(__file__), "results")
        os.makedirs(out_dir, exist_ok=True)
        ts = datetime.now().strftime("%Y%m%d_%H%M%S")
        fname = f"{label}_{args.partition}_{ts}.json"
        out_path = os.path.join(out_dir, fname)
        with open(out_path, "w", encoding="utf-8") as f:
            json.dump(result, f, ensure_ascii=False, indent=2)
        print(f"  结果已保存: {out_path}")

    # 返回码：总分 < 40 时以非零退出（方便 CI 集成）
    sys.exit(0 if result["scores"]["total"] >= 40 else 1)


if __name__ == "__main__":
    main()
