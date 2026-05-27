#!/usr/bin/env python3
"""
将 Claude 网页回复文本解析为 11 个 C/H 文件

用法：
    python3 DIMA/eval/save_response.py             # 处理全部三种格式
    python3 DIMA/eval/save_response.py --fmt nl    # 只处理 NL 格式

输入：DIMA/eval/responses/{nl,aadl,json}_ps2.txt
输出：DIMA/eval/generated/format_cmp/{nl,aadl,json}/ps2/
"""

import argparse
import os
import re

EVAL_DIR  = os.path.dirname(os.path.abspath(__file__))
RESP_DIR  = os.path.join(EVAL_DIR, "responses")
OUT_BASE  = os.path.join(EVAL_DIR, "generated", "format_cmp")
PARTITION = "ps2"
FORMATS   = ["nl", "aadl", "json"]


def parse_and_save(fmt: str):
    resp_file = os.path.join(RESP_DIR, f"{fmt}_{PARTITION}.txt")
    if not os.path.isfile(resp_file):
        print(f"[{fmt.upper()}] 回复文件不存在，跳过: {resp_file}")
        return

    with open(resp_file, encoding="utf-8") as f:
        response = f.read()

    out_dir = os.path.join(OUT_BASE, fmt, PARTITION)
    os.makedirs(out_dir, exist_ok=True)

    pattern = r'```(\w[\w./\-]*\.[ch])\n(.*?)```'
    written = []
    for m in re.finditer(pattern, response, re.DOTALL):
        fname   = os.path.basename(m.group(1))
        content = m.group(2)
        with open(os.path.join(out_dir, fname), "w", encoding="utf-8") as f:
            f.write(content)
        written.append(fname)

    if written:
        print(f"[{fmt.upper()}] 写入 {len(written)} 个文件 → {out_dir}")
        print(f"       {', '.join(sorted(written))}")
    else:
        print(f"[{fmt.upper()}] 未找到代码块，请检查回复文件格式（需要 ```filename.c 块）")


def main():
    parser = argparse.ArgumentParser(description="解析 Claude 回复，提取 C/H 文件")
    parser.add_argument("--fmt", choices=FORMATS, help="只处理指定格式（nl/aadl/json）")
    args = parser.parse_args()

    os.makedirs(RESP_DIR, exist_ok=True)
    for fmt in ([args.fmt] if args.fmt else FORMATS):
        parse_and_save(fmt)


if __name__ == "__main__":
    main()
