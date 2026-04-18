"""
解析 cl.exe 编译检查结果文件，输出每组实验的摘要表格。

用法：
    python DIMA/eval/parse_compile.py
    python DIMA/eval/parse_compile.py --input my_results.txt
    python DIMA/eval/parse_compile.py --verbose   # 显示每条错误/警告详情
"""

import re
import csv
import sys
import argparse
from pathlib import Path

# 结果文件默认路径（相对于项目根目录）
DEFAULT_LOG = Path(__file__).parent.parent.parent / "compile_check_results.txt"
CSV_OUT = Path(__file__).parent / "results" / "compile_check.csv"

IGNORED_WARNINGS = {"C4819"}  # 代码页警告，已在 bat 里用 /wd4819 抑制，双保险


def parse_results(log_path: Path):
    """
    解析 cl 输出文件，返回每组实验的结果列表。
    每项为 dict: {strategy, partition, errors, warnings, error_lines, warn_lines}
    """
    if not log_path.exists():
        print(f"[错误] 找不到结果文件: {log_path}")
        print("请先在 Windows 上运行 check_all.bat 生成结果文件。")
        sys.exit(1)

    text = log_path.read_text(encoding="utf-8", errors="replace")
    sections = re.split(r"\[([a-z]+/[a-z0-9]+)\]", text)

    results = []
    # sections[0] 是文件头，之后每两个元素是 (标签, 内容)
    for i in range(1, len(sections), 2):
        label = sections[i]          # e.g. "zeroshot/ps1"
        content = sections[i + 1] if i + 1 < len(sections) else ""

        strategy, partition = label.split("/")

        error_lines = []
        warn_lines = []

        for line in content.splitlines():
            line = line.strip()
            if not line:
                continue
            # cl 错误/警告格式：filename.c(行号): error/warning C####: 描述
            m = re.search(r":\s+(error|warning)\s+(C\d+):", line, re.IGNORECASE)
            if m:
                kind = m.group(1).lower()
                code = m.group(2).upper()
                if code in IGNORED_WARNINGS:
                    continue
                if kind == "error":
                    error_lines.append(line)
                else:
                    warn_lines.append(line)

        results.append({
            "strategy":    strategy,
            "partition":   partition,
            "errors":      len(error_lines),
            "warnings":    len(warn_lines),
            "error_lines": error_lines,
            "warn_lines":  warn_lines,
            "passed":      len(error_lines) == 0,
        })

    return results


def print_table(results):
    col = "{:<10} {:<10} {:>6} {:>8} {}"
    header = col.format("策略", "分区", "错误", "警告", "状态")
    sep = "-" * 50
    print(sep)
    print(header)
    print(sep)
    for r in results:
        status = "✅ 通过" if r["passed"] else "❌ 失败"
        print(col.format(r["strategy"], r["partition"],
                         r["errors"], r["warnings"], status))
    print(sep)

    total = len(results)
    passed = sum(1 for r in results if r["passed"])
    print(f"\n总计：{total} 组实验，{passed} 通过，{total - passed} 失败")


def print_verbose(results):
    for r in results:
        if r["error_lines"] or r["warn_lines"]:
            print(f"\n── {r['strategy']}/{r['partition']} ──")
            for line in r["error_lines"]:
                print(f"  ERROR   {line}")
            for line in r["warn_lines"]:
                print(f"  WARNING {line}")


def save_csv(results, out_path: Path):
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with open(out_path, "w", newline="", encoding="utf-8-sig") as f:
        writer = csv.writer(f)
        writer.writerow(["策略", "分区", "错误数", "警告数", "通过"])
        for r in results:
            writer.writerow([
                r["strategy"], r["partition"],
                r["errors"], r["warnings"],
                "是" if r["passed"] else "否",
            ])
    print(f"\nCSV 已保存：{out_path}")


def main():
    parser = argparse.ArgumentParser(description="解析 cl.exe 编译检查结果")
    parser.add_argument("--input", default=str(DEFAULT_LOG),
                        help="结果文件路径（默认：项目根目录/compile_check_results.txt）")
    parser.add_argument("--verbose", action="store_true",
                        help="显示每条错误/警告的详细内容")
    args = parser.parse_args()

    results = parse_results(Path(args.input))

    print_table(results)

    if args.verbose:
        print_verbose(results)

    save_csv(results, CSV_OUT)


if __name__ == "__main__":
    main()
