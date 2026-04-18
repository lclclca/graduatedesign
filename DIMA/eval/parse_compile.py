"""
解析 cl.exe 编译检查结果文件，输出分类摘要表格和各分区错误报告。

用法：
    python DIMA/eval/parse_compile.py
    python DIMA/eval/parse_compile.py --input my_results.txt
    python DIMA/eval/parse_compile.py --verbose   # 显示每条错误/警告详情
    python DIMA/eval/parse_compile.py --reports   # 生成各分区独立报告文件
"""

import re
import csv
import sys
import argparse
from collections import defaultdict
from pathlib import Path

DEFAULT_LOG = Path(__file__).parent.parent.parent / "compile_check_results.txt"
CSV_OUT     = Path(__file__).parent / "results" / "compile_check.csv"
REPORT_DIR  = Path(__file__).parent / "results" / "compile_reports"

# 忽略的警告码：C4819 = 代码页（中文注释引发），/wd4819 已在 bat 里抑制，双保险
IGNORED_CODES = {"C4819"}

# ── 错误/警告分类 ──────────────────────────────────────────────────────────────
# 格式：类别名 -> 判断函数(code: str) -> bool
# code 形如 "C2065"

def _n(code):
    return int(code[1:])

CATEGORIES = [
    ("语法错误",      lambda c: _n(c) in {2059, 2143, 2018, 2061, 2062, 2063, 2064, 2017, 2003}),
    ("未声明标识符",   lambda c: _n(c) in {2065, 2087, 4013}),
    ("类型/表达式错误", lambda c: _n(c) in {2440, 2664, 2100, 2106, 2198, 2166, 2036,
                                            2015, 2011, 2040, 2397}
                                 or 2200 <= _n(c) <= 2299),
    ("重定义",        lambda c: _n(c) in {2086, 2374, 2375, 2084, 2082}),
    ("未初始化变量",   lambda c: _n(c) == 4700),
    ("未使用变量",     lambda c: _n(c) in {4101, 4189}),
    ("函数声明缺失",   lambda c: _n(c) in {4013, 2085}),
]
CATEGORY_NAMES = [name for name, _ in CATEGORIES] + ["其他"]


def categorize(code: str) -> str:
    for name, fn in CATEGORIES:
        try:
            if fn(code):
                return name
        except Exception:
            pass
    return "其他"


# ── 解析 ───────────────────────────────────────────────────────────────────────

def parse_results(log_path: Path):
    if not log_path.exists():
        print(f"[错误] 找不到结果文件: {log_path}")
        print("请先在 Windows 上运行 check_all.bat 生成结果文件。")
        sys.exit(1)

    # check_all.bat 开头有 chcp 65001，生成的文件是 UTF-8
    # 旧文件（GBK）作为回退
    for enc in ("utf-8-sig", "utf-8", "gbk"):
        try:
            text = log_path.read_text(encoding=enc)
            break
        except UnicodeDecodeError:
            continue
    else:
        text = log_path.read_text(encoding="gbk", errors="replace")
    sections = re.split(r"\[([a-z]+/[a-z0-9]+)\]", text)

    results = []
    for i in range(1, len(sections), 2):
        label   = sections[i]
        content = sections[i + 1] if i + 1 < len(sections) else ""
        strategy, partition = label.split("/")

        issues = []   # list of (kind, code, category, full_line)

        for line in content.splitlines():
            line = line.strip()
            if not line:
                continue
            m = re.search(r":\s+(error|warning)\s+(C\d+):\s*(.*)", line, re.IGNORECASE)
            if not m:
                continue
            kind  = m.group(1).lower()
            code  = m.group(2).upper()
            if code in IGNORED_CODES:
                continue
            cat = categorize(code)
            issues.append((kind, code, cat, line))

        errors   = [(k, c, cat, l) for k, c, cat, l in issues if k == "error"]
        warnings = [(k, c, cat, l) for k, c, cat, l in issues if k == "warning"]

        # 按分类统计
        cat_counts = defaultdict(int)
        for _, _, cat, _ in issues:
            cat_counts[cat] += 1

        results.append({
            "strategy":   strategy,
            "partition":  partition,
            "errors":     len(errors),
            "warnings":   len(warnings),
            "cat_counts": dict(cat_counts),
            "error_items":   errors,
            "warning_items": warnings,
            "passed":     len(errors) == 0,
        })

    return results


# ── 输出：摘要表 ───────────────────────────────────────────────────────────────

def print_table(results):
    # 宽表：基础列 + 各分类列
    base  = "{:<10} {:<10} {:>5} {:>6}"
    extra = " {:>8}" * len(CATEGORY_NAMES)
    fmt   = base + extra

    header = fmt.format("策略", "分区", "错误", "警告", *CATEGORY_NAMES)
    sep    = "-" * (len(header) + 4)

    print(sep)
    print(header)
    print(sep)
    for r in results:
        cats = [r["cat_counts"].get(n, 0) for n in CATEGORY_NAMES]
        status_prefix = "  " if r["passed"] else "* "
        print(status_prefix + fmt.format(
            r["strategy"], r["partition"],
            r["errors"], r["warnings"], *cats
        ))
    print(sep)
    print("* 表示有错误的分区\n")

    total  = len(results)
    passed = sum(1 for r in results if r["passed"])
    print(f"总计：{total} 组实验，{passed} 通过，{total - passed} 失败")


# ── 输出：详细内容 ─────────────────────────────────────────────────────────────

def print_verbose(results):
    for r in results:
        all_items = r["error_items"] + r["warning_items"]
        if not all_items:
            continue
        print(f"\n{'-'*60}")
        print(f"  {r['strategy']}/{r['partition']}")
        print(f"{'-'*60}")
        # 按分类分组显示
        by_cat = defaultdict(list)
        for kind, code, cat, line in all_items:
            by_cat[cat].append((kind.upper(), code, line))
        for cat in CATEGORY_NAMES:
            if cat not in by_cat:
                continue
            print(f"\n  [{cat}]")
            for kind, code, line in by_cat[cat]:
                print(f"    {kind:<7} {code}  {line}")


# ── 输出：各分区独立报告文件 ───────────────────────────────────────────────────

def save_reports(results, report_dir: Path):
    report_dir.mkdir(parents=True, exist_ok=True)
    for r in results:
        fname = report_dir / f"{r['strategy']}_{r['partition']}.txt"
        lines = []
        lines.append(f"编译检查报告：{r['strategy']}/{r['partition']}")
        lines.append("=" * 50)
        lines.append(f"错误数：{r['errors']}  警告数：{r['warnings']}  "
                     f"{'通过' if r['passed'] else '未通过'}")
        lines.append("")

        if r["cat_counts"]:
            lines.append("分类统计：")
            for cat, cnt in r["cat_counts"].items():
                lines.append(f"  {cat:<12} {cnt}")
            lines.append("")

        all_items = r["error_items"] + r["warning_items"]
        if all_items:
            lines.append("详细问题：")
            by_cat = defaultdict(list)
            for kind, code, cat, line in all_items:
                by_cat[cat].append((kind.upper(), code, line))
            for cat in CATEGORY_NAMES:
                if cat not in by_cat:
                    continue
                lines.append(f"\n  [{cat}]")
                for kind, code, line in by_cat[cat]:
                    lines.append(f"    {kind:<7} {code}  {line}")
        else:
            lines.append("无错误/警告。")

        fname.write_text("\n".join(lines), encoding="utf-8-sig")

    print(f"\n各分区报告已保存至：{report_dir}/")


# ── 输出：CSV ─────────────────────────────────────────────────────────────────

def save_csv(results, out_path: Path):
    out_path.parent.mkdir(parents=True, exist_ok=True)
    try:
        with open(out_path, "w", newline="", encoding="utf-8-sig") as f:
            writer = csv.writer(f)
            writer.writerow(["策略", "分区", "错误数", "警告数", "通过"] + CATEGORY_NAMES)
            for r in results:
                cats = [r["cat_counts"].get(n, 0) for n in CATEGORY_NAMES]
                writer.writerow([
                    r["strategy"], r["partition"],
                    r["errors"], r["warnings"],
                    "是" if r["passed"] else "否",
                    *cats,
                ])
        print(f"CSV 已保存：{out_path}")
    except PermissionError:
        print(f"[跳过 CSV] 文件被占用（可能在 Excel 中打开）：{out_path}")


# ── 入口 ──────────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description="解析 cl.exe 编译检查结果")
    parser.add_argument("--input",   default=str(DEFAULT_LOG),
                        help="结果文件路径（默认：项目根目录/compile_check_results.txt）")
    parser.add_argument("--verbose", action="store_true",
                        help="在终端显示每条错误/警告的分类详情")
    parser.add_argument("--reports", action="store_true",
                        help="为每个分区生成独立的 .txt 报告文件")
    args = parser.parse_args()

    sys.stdout.reconfigure(encoding="utf-8", errors="replace")

    results = parse_results(Path(args.input))
    print_table(results)

    if args.verbose:
        print_verbose(results)

    save_csv(results, CSV_OUT)

    if args.reports:
        save_reports(results, REPORT_DIR)


if __name__ == "__main__":
    main()
