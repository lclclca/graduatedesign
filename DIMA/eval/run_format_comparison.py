#!/usr/bin/env python3
"""
输入格式对比实验
================
针对同一分区 ps2，使用相同零样本提示骨架，对比三种输入格式：
  nl   — 自然语言描述
  aadl — AADL 原文（partitions + threads 相关节段）
  json — 结构化 JSON 规格

用法：
    export ANTHROPIC_API_KEY=sk-ant-...
    python DIMA/eval/run_format_comparison.py

输出：
    eval/generated/format_cmp/{nl,aadl,json}/ps2/  — 生成的11个文件
    eval/results/format_cmp_results.json            — 三维得分 + 合规率汇总
"""

import anthropic
import json
import os
import re
import sys
import time

sys.path.insert(0, os.path.dirname(__file__))
from metrics import structural, api_check, semantic
from metrics import arinc_compliance

# ── 路径配置 ──────────────────────────────────────────────────────────────────
EVAL_DIR   = os.path.dirname(os.path.abspath(__file__))
SPEC_JSON  = os.path.join(EVAL_DIR, "specs", "ps2.json")
AADL_PARTS = os.path.join(EVAL_DIR, "../../Sys/DIMA_partitions.aadl")
AADL_THRDS = os.path.join(EVAL_DIR, "../../Sys/DIMA_threads.aadl")
OUT_BASE   = os.path.join(EVAL_DIR, "generated", "format_cmp")
RESULT_F   = os.path.join(EVAL_DIR, "results", "format_cmp_results.json")

PARTITION  = "ps2"
MODEL      = "claude-3-5-sonnet-20241022"
MAX_TOKENS = 8192

WEIGHTS = {"structural": 0.30, "api": 0.40, "semantic": 0.30}


# ════════════════════════════════════════════════════════════════════════════
#  三种输入格式规格
# ════════════════════════════════════════════════════════════════════════════

NL_SPEC = """\
分区名称：ps2（所属模块：M1）

任务配置（共3个周期性任务）：
- task21：周期 50 ms，优先级 2
- task22：周期 50 ms，优先级 3
- task23：周期 100 ms，优先级 4

分区间通信端口：
- 采样输入端口：pr2samplingin（从其他分区接收整数数据，只读操作）
- 队列输出端口：pr2queueingout（向其他分区发送整数数据，队列容量 30，写操作）

分区内共享资源：
- 黑板（BLACKBOARD）3 个：bb_acc3, bb_t2t3, bb_t3t2
- 缓冲区（BUFFER）3 个：buf_order, buf_t2tot3, buf_t3tot2

辅助子程序：commandboard_receiveinput_spg, commandboard_printinfos_spg

部署参数：
- NB_THREADS = 3，NB_SAMPLING_PORTS = 1，NB_QUEUING_PORTS = 1
- NB_BLACKBOARDS = 3，NB_BUFFERS = 3，STACKS_SIZE = 24576
"""


def load_aadl_spec():
    """提取 DIMA_partitions.aadl 中 P2 相关节段 + DIMA_threads.aadl 中 Tsk21/22/23"""
    def extract_block(text, keyword):
        """提取从 keyword 开始到下一个 process/thread 定义之前的内容"""
        pattern = rf'((?:process|thread)[^;]*{keyword}.*?)(?=\n\s*(?:process|thread)\s|\Z)'
        m = re.search(pattern, text, re.DOTALL)
        return m.group(1).strip() if m else ""

    with open(AADL_PARTS, encoding='utf-8') as f:
        parts_text = f.read()
    with open(AADL_THRDS, encoding='utf-8') as f:
        thrds_text = f.read()

    # 提取 P2 定义（process P2 到 end P2.impl）
    p2_match = re.search(r'process P2.*?end P2\.impl;', parts_text, re.DOTALL)
    p2_block = p2_match.group(0).strip() if p2_match else parts_text

    # 提取 Tsk21 / Tsk22 / Tsk23
    tsk_blocks = []
    for tsk in ["Tsk21", "Tsk22", "Tsk23"]:
        m = re.search(rf'thread {tsk}.*?end {tsk}\.impl;', thrds_text, re.DOTALL)
        if m:
            tsk_blocks.append(m.group(0).strip())

    return f"""-- ===== 分区定义（DIMA_partitions.aadl 节选）=====
package DIMA_partitions
public
  with DIMA_threads; with ARINC653; with Base_Types;

{p2_block}

end DIMA_partitions;

-- ===== 线程定义（DIMA_threads.aadl 节选）=====
package DIMA_threads
public
  with ARINC653; with Base_Types;

{chr(10).join(tsk_blocks)}

end DIMA_threads;
"""


def load_json_spec():
    with open(SPEC_JSON, encoding='utf-8') as f:
        return json.dumps(json.load(f), indent=2, ensure_ascii=False)


# ════════════════════════════════════════════════════════════════════════════
#  提示词构造（三种格式共用同一骨架，只换规格部分）
# ════════════════════════════════════════════════════════════════════════════

SYSTEM_PROMPT = """\
你是一名专业的嵌入式软件工程师，精通 ACoreOS653 实时操作系统和 ARINC 653 标准。
你的任务是为 IMA 分区生成完整的 ARINC 653 C 源代码。"""

TASK_HEADER = """\
请根据以下分区规格，生成该分区的全部 11 个 C/H 源文件：

  activity.c / activity.h   — 任务函数与 HM 回调实现
  deployment.c / deployment.h — 部署常量宏定义
  globals.c / globals.h     — 全局变量声明与定义
  gtypes.c / gtypes.h       — 类型定义
  main.c                    — appMain 入口（资源创建与任务启动）
  subprograms.c / subprograms.h — 辅助子程序桩

每个文件输出格式：
```filename.c
/* 文件内容 */
```

严格遵守 ARINC 653 APEX API，所有 CREATE/READ/WRITE 操作必须双端配对，
HM 回调函数必须实现，CHECK_CODE 宏必须用于 API 返回值检查。

"""

FORMAT_LABELS = {
    "nl":   "【分区规格（自然语言描述）】",
    "aadl": "【分区规格（AADL 模型原文）】",
    "json": "【分区规格（结构化 JSON）】",
}


def build_prompt(fmt: str, spec_text: str) -> str:
    return TASK_HEADER + FORMAT_LABELS[fmt] + "\n\n" + spec_text


# ════════════════════════════════════════════════════════════════════════════
#  LLM 调用与文件解析
# ════════════════════════════════════════════════════════════════════════════

def call_llm(system: str, user: str) -> str:
    client = anthropic.Anthropic()
    msg = client.messages.create(
        model=MODEL,
        max_tokens=MAX_TOKENS,
        system=system,
        messages=[{"role": "user", "content": user}],
    )
    return msg.content[0].text


def parse_and_save(response: str, out_dir: str):
    """从 LLM 回复中提取各文件内容并写入 out_dir"""
    os.makedirs(out_dir, exist_ok=True)
    # 匹配 ```filename.ext\n...内容...```
    pattern = r'```(\w[\w./\-]*\.[ch])\n(.*?)```'
    files_written = []
    for m in re.finditer(pattern, response, re.DOTALL):
        fname = os.path.basename(m.group(1))
        content = m.group(2)
        fpath = os.path.join(out_dir, fname)
        with open(fpath, 'w', encoding='utf-8') as f:
            f.write(content)
        files_written.append(fname)
    print(f"  → 写入 {len(files_written)} 个文件: {', '.join(sorted(files_written))}")
    return files_written


# ════════════════════════════════════════════════════════════════════════════
#  评估
# ════════════════════════════════════════════════════════════════════════════

def evaluate(gen_dir: str, spec: dict) -> dict:
    s_res = structural.check(gen_dir, spec)
    a_res = api_check.check(gen_dir, spec)
    c_res = semantic.check(gen_dir, spec)

    s_score = sum(r["partial"] * r["weight"] for r in s_res) / sum(r["weight"] for r in s_res) * 100
    a_score = sum(r["partial"] * r["weight"] for r in a_res) / sum(r["weight"] for r in a_res) * 100
    c_score = sum(r["partial"] * r["weight"] for r in c_res) / sum(r["weight"] for r in c_res) * 100
    total   = s_score * WEIGHTS["structural"] + a_score * WEIGHTS["api"] + c_score * WEIGHTS["semantic"]

    arinc_res = arinc_compliance.check(gen_dir, spec)
    passed = sum(1 for r in arinc_res if r.get("passed"))
    arinc_rate = passed / len(arinc_res) * 100 if arinc_res else 0.0

    return {
        "structural": round(s_score, 1),
        "api":        round(a_score, 1),
        "semantic":   round(c_score, 1),
        "total":      round(total, 1),
        "arinc_rate": round(arinc_rate, 1),
        "arinc_detail": arinc_res,
    }


# ════════════════════════════════════════════════════════════════════════════
#  主流程
# ════════════════════════════════════════════════════════════════════════════

def main():
    if not os.environ.get("ANTHROPIC_API_KEY"):
        print("[ERROR] 请先设置环境变量 ANTHROPIC_API_KEY")
        sys.exit(1)

    with open(SPEC_JSON, encoding='utf-8') as f:
        spec = json.load(f)

    formats = {
        "nl":   NL_SPEC,
        "aadl": load_aadl_spec(),
        "json": load_json_spec(),
    }

    results = {}

    for fmt, spec_text in formats.items():
        print(f"\n{'='*60}")
        print(f"[格式: {fmt.upper()}] 正在调用 LLM...")
        prompt = build_prompt(fmt, spec_text)
        out_dir = os.path.join(OUT_BASE, fmt, PARTITION)

        try:
            response = call_llm(SYSTEM_PROMPT, prompt)
        except Exception as e:
            print(f"  [ERROR] API 调用失败: {e}")
            continue

        parse_and_save(response, out_dir)

        print(f"  评估中...")
        scores = evaluate(out_dir, spec)
        results[fmt] = scores

        print(f"  结构:{scores['structural']}  API:{scores['api']}  "
              f"语义:{scores['semantic']}  综合:{scores['total']}  "
              f"合规率:{scores['arinc_rate']}%")

        time.sleep(2)  # 避免 API 限速

    # ── 汇总输出 ───────────────────────────────────────────────────────────
    print(f"\n{'='*60}")
    print("输入格式对比结果（分区 ps2，零样本策略）\n")
    print(f"{'格式':<8} {'结构S':>6} {'API A':>6} {'语义C':>6} {'综合':>6} {'合规率':>8}")
    print("-" * 46)
    labels = {"nl": "自然语言", "aadl": "AADL原文", "json": "结构化JSON"}
    for fmt in ["nl", "aadl", "json"]:
        r = results.get(fmt, {})
        print(f"{labels[fmt]:<8} {r.get('structural','-'):>6} {r.get('api','-'):>6} "
              f"{r.get('semantic','-'):>6} {r.get('total','-'):>6} {r.get('arinc_rate','-'):>7}%")

    os.makedirs(os.path.dirname(RESULT_F), exist_ok=True)
    with open(RESULT_F, 'w', encoding='utf-8') as f:
        json.dump(results, f, indent=2, ensure_ascii=False)
    print(f"\n[DONE] 结果已保存: {RESULT_F}")


if __name__ == "__main__":
    main()
