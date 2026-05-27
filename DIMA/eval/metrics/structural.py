"""
维度1：结构完整性（Structural Completeness）权重 30%

检查项：
  S1  必需文件是否全部存在（11个文件）
  S2  main.c 中是否定义了 appMain()
  S3  activity.h 中是否声明了所有 taskXX_job
  S4  activity.c 中是否实现了所有 taskXX_job
  S5  所有 .h 文件是否有头文件保护（#ifndef ... #define ... #endif）
  S6  所有 .c 文件是否包含必要的 #include（apexLib.h）
  S7  globals.h 中是否定义了 CHECK_CODE 宏
  S8  gtypes.h 中是否定义了 integer 类型
"""

import os
import re
from typing import Dict, List


def _read(path: str) -> str:
    """读取文件内容，文件不存在返回空字符串"""
    try:
        with open(path, encoding="utf-8", errors="replace") as f:
            return f.read()
    except FileNotFoundError:
        return ""


def check(gen_dir: str, spec: dict) -> Dict:
    """
    参数:
        gen_dir : 待评估的生成代码目录
        spec    : 对应分区的规格字典（来自 specs/psX.json）
    返回:
        {
          "score": 0~100,
          "details": [{"id": "S1", "desc": ..., "passed": bool, "note": ...}, ...]
        }
    """
    details = []
    task_names = [t["name"] for t in spec["tasks"]]

    # ── S1：11个必需文件全部存在 ─────────────────────────────────────────
    missing = []
    for fname in spec["required_files"]:
        if not os.path.isfile(os.path.join(gen_dir, fname)):
            missing.append(fname)
    s1_pass = len(missing) == 0
    # 部分分（每缺一个文件扣分）
    s1_score = max(0, len(spec["required_files"]) - len(missing)) / len(spec["required_files"])
    details.append({
        "id": "S1", "weight": 2.0,
        "desc": f"必需文件完整性（{len(spec['required_files'])}个文件）",
        "passed": s1_pass,
        "partial": s1_score,
        "note": f"缺失: {missing}" if missing else "全部存在"
    })

    # ── S2：main.c 定义了 appMain ────────────────────────────────────────
    main_c = _read(os.path.join(gen_dir, "main.c"))
    s2_pass = bool(re.search(r"\bvoid\s+appMain\s*\(", main_c))
    details.append({
        "id": "S2", "weight": 1.5,
        "desc": "main.c 中定义了 appMain(void)",
        "passed": s2_pass, "partial": float(s2_pass),
        "note": "" if s2_pass else "未找到 appMain 函数定义"
    })

    # ── S3：activity.h 声明了所有任务函数 ───────────────────────────────
    act_h = _read(os.path.join(gen_dir, "activity.h"))
    missing_decl = [t for t in task_names
                    if not re.search(rf"\bvoid\s*\*\s*{t}_job\s*\(", act_h)]
    s3_score = max(0, len(task_names) - len(missing_decl)) / len(task_names)
    details.append({
        "id": "S3", "weight": 1.0,
        "desc": f"activity.h 声明了所有 {len(task_names)} 个任务函数",
        "passed": len(missing_decl) == 0,
        "partial": s3_score,
        "note": f"缺失声明: {missing_decl}" if missing_decl else "全部声明"
    })

    # ── S4：activity.c 实现了所有任务函数 ───────────────────────────────
    act_c = _read(os.path.join(gen_dir, "activity.c"))
    missing_impl = [t for t in task_names
                    if not re.search(rf"\bvoid\s*\*\s*{t}_job\s*\(", act_c)]
    s4_score = max(0, len(task_names) - len(missing_impl)) / len(task_names)
    details.append({
        "id": "S4", "weight": 1.5,
        "desc": f"activity.c 实现了所有 {len(task_names)} 个任务函数",
        "passed": len(missing_impl) == 0,
        "partial": s4_score,
        "note": f"缺失实现: {missing_impl}" if missing_impl else "全部实现"
    })

    # ── S5：所有 .h 文件有头文件保护 ────────────────────────────────────
    h_files = [f for f in spec["required_files"] if f.endswith(".h")]
    no_guard = []
    for hf in h_files:
        content = _read(os.path.join(gen_dir, hf))
        if not (re.search(r"#ifndef\s+\w+", content) and
                re.search(r"#define\s+\w+", content) and
                "#endif" in content):
            no_guard.append(hf)
    s5_score = max(0, len(h_files) - len(no_guard)) / len(h_files) if h_files else 1.0
    details.append({
        "id": "S5", "weight": 0.5,
        "desc": f"所有 .h 文件有头文件保护（#ifndef/#define/#endif）",
        "passed": len(no_guard) == 0,
        "partial": s5_score,
        "note": f"缺少头文件保护: {no_guard}" if no_guard else "全部正确"
    })

    # ── S6：.c 文件包含 apexLib.h ────────────────────────────────────────
    c_files_with_apex = ["activity.c", "main.c"]
    no_apex = []
    for cf in c_files_with_apex:
        content = _read(os.path.join(gen_dir, cf))
        if "apexLib.h" not in content and content:  # 文件存在但无此包含
            no_apex.append(cf)
    s6_score = max(0, len(c_files_with_apex) - len(no_apex)) / len(c_files_with_apex)
    details.append({
        "id": "S6", "weight": 0.5,
        "desc": "activity.c 和 main.c 包含 apexLib.h",
        "passed": len(no_apex) == 0,
        "partial": s6_score,
        "note": f"缺少 apexLib.h: {no_apex}" if no_apex else "全部包含"
    })

    # ── S7：globals.h 定义了 CHECK_CODE 宏 ──────────────────────────────
    globals_h = _read(os.path.join(gen_dir, "globals.h"))
    s7_pass = "#define CHECK_CODE" in globals_h
    # 区分：定义了但逻辑是否正确（else 分支是否打印错误而非 SUCCESSFUL）
    s7_correct = s7_pass and "SUCCESSFUL" not in globals_h.split("CHECK_CODE")[1][:200] \
        if s7_pass and "CHECK_CODE" in globals_h else False
    details.append({
        "id": "S7", "weight": 1.0,
        "desc": "globals.h 定义了 CHECK_CODE 宏（且 else 分支打印真实错误）",
        "passed": s7_correct,
        "partial": 0.5 if (s7_pass and not s7_correct) else float(s7_correct),
        "note": ("正确" if s7_correct else
                 "定义了但 else 分支逻辑错误（两分支均打印SUCCESSFUL）" if s7_pass
                 else "未定义 CHECK_CODE")
    })

    # ── S8：gtypes.h 定义了 integer 类型 ────────────────────────────────
    gtypes_h = _read(os.path.join(gen_dir, "gtypes.h"))
    s8_pass = bool(re.search(r"typedef\s+int\s+integer\s*;", gtypes_h))
    details.append({
        "id": "S8", "weight": 1.0,
        "desc": "gtypes.h 中 typedef int integer",
        "passed": s8_pass, "partial": float(s8_pass),
        "note": "" if s8_pass else "未找到 typedef int integer"
    })

    # ── 加权得分 ─────────────────────────────────────────────────────────
    total_weight = sum(d["weight"] for d in details)
    weighted_sum = sum(d["partial"] * d["weight"] for d in details)
    score = round(weighted_sum / total_weight * 100, 1)

    return {"score": score, "details": details}
