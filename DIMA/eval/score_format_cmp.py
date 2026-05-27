#!/usr/bin/env python3
"""
输入格式对比评分脚本

前置条件：已运行 save_response.py，三种格式的文件已写入：
    DIMA/eval/generated/format_cmp/{nl,aadl,json}/ps2/

用法：
    python3 DIMA/eval/score_format_cmp.py

输出：控制台打印对比表 + 结果保存至 results/format_cmp_results.json
"""

import json
import os
import sys

EVAL_DIR  = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, EVAL_DIR)

from evaluate import evaluate
from metrics import arinc_compliance

SPEC_JSON = os.path.join(EVAL_DIR, "specs", "ps2.json")
GEN_BASE  = os.path.join(EVAL_DIR, "generated", "format_cmp")
RESULT_F  = os.path.join(EVAL_DIR, "results", "format_cmp_results.json")
PARTITION = "ps2"
FORMATS   = ["nl", "aadl", "json"]
LABELS    = {"nl": "自然语言", "aadl": "AADL原文", "json": "结构化JSON"}


def main():
    with open(SPEC_JSON, encoding="utf-8") as f:
        spec = json.load(f)

    results = {}

    for fmt in FORMATS:
        gen_dir = os.path.join(GEN_BASE, fmt, PARTITION)
        if not os.path.isdir(gen_dir) or not os.listdir(gen_dir):
            print(f"[{fmt.upper()}] 目录为空或不存在，跳过（请先运行 save_response.py）")
            continue

        print(f"[{fmt.upper()}] 评估中 → {gen_dir}")
        ev = evaluate(gen_dir, spec)
        s  = ev["scores"]

        arinc_res  = arinc_compliance.check(gen_dir, spec)
        passed     = sum(1 for r in arinc_res if r.get("pass"))
        arinc_rate = round(passed / len(arinc_res) * 100, 1) if arinc_res else 0.0

        results[fmt] = {
            "structural":   s["structural"],
            "api":          s["api"],
            "semantic":     s["semantic"],
            "total":        s["total"],
            "arinc_rate":   arinc_rate,
            "arinc_detail": arinc_res,
        }

    if not results:
        print("\n尚无可评估结果，请先完成实验并运行 save_response.py。")
        return

    print(f"\n{'='*54}")
    print("输入格式对比结果（分区 ps2，零样本策略）\n")
    print(f"{'格式':<10} {'结构S':>6} {'API A':>6} {'语义C':>6} {'综合':>6} {'合规率':>8}")
    print("-" * 50)
    for fmt in FORMATS:
        r = results.get(fmt)
        if r:
            print(f"{LABELS[fmt]:<10} {r['structural']:>6.1f} {r['api']:>6.1f} "
                  f"{r['semantic']:>6.1f} {r['total']:>6.1f} {r['arinc_rate']:>7.1f}%")
        else:
            print(f"{LABELS[fmt]:<10} {'—':>6} {'—':>6} {'—':>6} {'—':>6} {'—':>8}")

    os.makedirs(os.path.dirname(RESULT_F), exist_ok=True)
    with open(RESULT_F, "w", encoding="utf-8") as f:
        json.dump(results, f, indent=2, ensure_ascii=False)
    print(f"\n[DONE] 结果已保存: {RESULT_F}")


if __name__ == "__main__":
    main()
