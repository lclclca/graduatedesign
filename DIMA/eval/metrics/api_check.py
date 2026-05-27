"""
维度2：API正确性（API Correctness）权重 40%

检查项：
  A1  main.c 中 CREATE_SAMPLING_PORT 调用次数与规格一致
  A2  main.c 中 CREATE_QUEUING_PORT  调用次数与规格一致
  A3  main.c 中 CREATE_BLACKBOARD    调用次数与规格一致
  A4  main.c 中 CREATE_BUFFER        调用次数与规格一致
  A5  main.c 中 CREATE_PROCESS       调用次数 == 任务数
  A6  main.c 中 START                调用次数 == 任务数
  A7  main.c 中调用了 SET_PARTITION_MODE(NORMAL)
  A8  每个任务函数体内都有 PERIODIC_WAIT
  A9  main.c 中 CHECK_CODE 调用次数合理（>= API调用总数）
  A10 采样端口方向与API匹配（SOURCE→WRITE, DEST→READ）
  A11 队列端口方向与API匹配（SOURCE→SEND, DEST→RECEIVE）
  A12 main.c 中不存在 GET_QUEUING_PORT_ID（应在 appMain 创建端口，不在任务循环里）
"""

import re
from typing import Dict


def _read(path: str) -> str:
    try:
        with open(path, encoding="utf-8", errors="replace") as f:
            return f.read()
    except FileNotFoundError:
        return ""


def _strip_comments_and_strings(code: str) -> str:
    """Remove C comments and string literals so API names inside them aren't counted."""
    # Remove block comments
    code = re.sub(r'/\*.*?\*/', ' ', code, flags=re.DOTALL)
    # Remove line comments
    code = re.sub(r'//[^\n]*', ' ', code)
    # Remove string literals (replace content with empty quotes)
    code = re.sub(r'"[^"\\]*(?:\\.[^"\\]*)*"', '""', code)
    return code


def _count(pattern: str, text: str) -> int:
    return len(re.findall(pattern, text))


def check(gen_dir: str, spec: dict) -> Dict:
    """
    返回:
        {"score": 0~100, "details": [...]}
    """
    import os
    details = []
    main_c  = _strip_comments_and_strings(_read(os.path.join(gen_dir, "main.c")))
    act_c   = _strip_comments_and_strings(_read(os.path.join(gen_dir, "activity.c")))
    task_names = [t["name"] for t in spec["tasks"]]

    nb_tasks    = len(task_names)
    nb_sampling = len(spec["sampling_ports"])
    nb_queuing  = len(spec["queuing_ports"])
    nb_bb       = len(spec["blackboards"])
    nb_buf      = len(spec["buffers"])

    # ── A1：CREATE_SAMPLING_PORT 调用次数 ───────────────────────────────
    got = _count(r"\bCREATE_SAMPLING_PORT\b", main_c)
    a1_score = 1.0 if got == nb_sampling else (0.5 if abs(got - nb_sampling) == 1 else 0.0)
    details.append({
        "id": "A1", "weight": 1.5,
        "desc": f"CREATE_SAMPLING_PORT 次数（期望 {nb_sampling}，实际 {got}）",
        "passed": got == nb_sampling, "partial": a1_score,
        "note": f"期望{nb_sampling}次，实际{got}次"
    })

    # ── A2：CREATE_QUEUING_PORT 调用次数 ────────────────────────────────
    got = _count(r"\bCREATE_QUEUING_PORT\b", main_c)
    a2_score = 1.0 if got == nb_queuing else (0.5 if abs(got - nb_queuing) == 1 else 0.0)
    details.append({
        "id": "A2", "weight": 1.5,
        "desc": f"CREATE_QUEUING_PORT 次数（期望 {nb_queuing}，实际 {got}）",
        "passed": got == nb_queuing, "partial": a2_score,
        "note": f"期望{nb_queuing}次，实际{got}次"
    })

    # ── A3：CREATE_BLACKBOARD 调用次数 ──────────────────────────────────
    got = _count(r"\bCREATE_BLACKBOARD\b", main_c)
    a3_score = 1.0 if got == nb_bb else (0.5 if abs(got - nb_bb) == 1 else 0.0)
    details.append({
        "id": "A3", "weight": 1.5,
        "desc": f"CREATE_BLACKBOARD 次数（期望 {nb_bb}，实际 {got}）",
        "passed": got == nb_bb, "partial": a3_score,
        "note": f"期望{nb_bb}次，实际{got}次"
    })

    # ── A4：CREATE_BUFFER 调用次数 ──────────────────────────────────────
    got = _count(r"\bCREATE_BUFFER\b", main_c)
    a4_score = 1.0 if got == nb_buf else (0.5 if abs(got - nb_buf) == 1 else 0.0)
    details.append({
        "id": "A4", "weight": 1.5,
        "desc": f"CREATE_BUFFER 次数（期望 {nb_buf}，实际 {got}）",
        "passed": got == nb_buf, "partial": a4_score,
        "note": f"期望{nb_buf}次，实际{got}次"
    })

    # ── A5：CREATE_PROCESS 调用次数 == 任务数 ───────────────────────────
    got = _count(r"\bCREATE_PROCESS\b", main_c)
    a5_score = 1.0 if got == nb_tasks else (0.5 if abs(got - nb_tasks) <= 1 else 0.0)
    details.append({
        "id": "A5", "weight": 2.0,
        "desc": f"CREATE_PROCESS 次数（期望 {nb_tasks}，实际 {got}）",
        "passed": got == nb_tasks, "partial": a5_score,
        "note": f"期望{nb_tasks}次，实际{got}次"
    })

    # ── A6：START 调用次数 == 任务数 ────────────────────────────────────
    got = _count(r"\bSTART\s*\(", main_c)
    a6_score = 1.0 if got == nb_tasks else (0.5 if abs(got - nb_tasks) <= 1 else 0.0)
    details.append({
        "id": "A6", "weight": 1.5,
        "desc": f"START 次数（期望 {nb_tasks}，实际 {got}）",
        "passed": got == nb_tasks, "partial": a6_score,
        "note": f"期望{nb_tasks}次，实际{got}次"
    })

    # ── A7：SET_PARTITION_MODE(NORMAL) ──────────────────────────────────
    a7_pass = bool(re.search(r"\bSET_PARTITION_MODE\s*\(\s*NORMAL\b", main_c))
    details.append({
        "id": "A7", "weight": 2.0,
        "desc": "main.c 末尾调用 SET_PARTITION_MODE(NORMAL)",
        "passed": a7_pass, "partial": float(a7_pass),
        "note": "" if a7_pass else "未找到 SET_PARTITION_MODE(NORMAL)"
    })

    # ── A8：每个任务函数体都有 PERIODIC_WAIT ────────────────────────────
    missing_pw = []
    for task in task_names:
        # 提取任务函数体（粗略：找到函数定义后到下一个顶层函数）
        m = re.search(
            rf"void\s*\*\s*{task}_job\s*\([^)]*\)\s*\{{(.+?)(?=\nvoid\s*\*|\Z)",
            act_c, re.DOTALL
        )
        body = m.group(1) if m else ""
        if "PERIODIC_WAIT" not in body:
            missing_pw.append(task)
    a8_score = max(0, nb_tasks - len(missing_pw)) / nb_tasks if nb_tasks else 1.0
    details.append({
        "id": "A8", "weight": 2.0,
        "desc": f"每个任务函数体包含 PERIODIC_WAIT",
        "passed": len(missing_pw) == 0, "partial": a8_score,
        "note": f"缺少PERIODIC_WAIT: {missing_pw}" if missing_pw else "全部包含"
    })

    # ── A9：CHECK_CODE 调用次数合理（≥ CREATE+START+PERIODIC 总数）──────
    # 最少：每个 CREATE_xxx 一次 + 每个 START 一次 + SET_PARTITION_MODE 一次
    min_expected = (nb_sampling + nb_queuing + nb_bb + nb_buf +
                    nb_tasks * 2 + 1)  # *2 = CREATE_PROCESS + START
    got_cc = _count(r"\bCHECK_CODE\b", main_c + act_c)
    a9_score = 1.0 if got_cc >= min_expected else (
               0.7 if got_cc >= min_expected * 0.7 else
               0.4 if got_cc >= min_expected * 0.4 else 0.0)
    details.append({
        "id": "A9", "weight": 1.0,
        "desc": f"CHECK_CODE 调用次数合理（期望≥{min_expected}，实际{got_cc}）",
        "passed": got_cc >= min_expected, "partial": a9_score,
        "note": f"实际{got_cc}次，最少期望{min_expected}次"
    })

    # ── A10：采样端口 API 方向正确 ──────────────────────────────────────
    sampling_ok = []
    for sp in spec["sampling_ports"]:
        if sp["direction"] == "SOURCE":
            ok = "WRITE_SAMPLING_MESSAGE" in act_c
        else:
            ok = "READ_SAMPLING_MESSAGE" in act_c
        sampling_ok.append(ok)
    a10_score = (sum(sampling_ok) / len(sampling_ok)) if sampling_ok else 1.0
    details.append({
        "id": "A10", "weight": 1.5,
        "desc": "采样端口方向与 API 匹配（SOURCE→WRITE, DEST→READ）",
        "passed": all(sampling_ok), "partial": a10_score,
        "note": ("无采样端口" if not spec["sampling_ports"] else
                 "全部正确" if all(sampling_ok) else "部分端口方向/API不匹配")
    })

    # ── A11：队列端口 API 方向正确 ──────────────────────────────────────
    queuing_ok = []
    for qp in spec["queuing_ports"]:
        if qp["direction"] == "SOURCE":
            ok = "SEND_QUEUING_MESSAGE" in act_c
        else:
            ok = "RECEIVE_QUEUING_MESSAGE" in act_c
        queuing_ok.append(ok)
    a11_score = (sum(queuing_ok) / len(queuing_ok)) if queuing_ok else 1.0
    details.append({
        "id": "A11", "weight": 1.5,
        "desc": "队列端口方向与 API 匹配（SOURCE→SEND, DEST→RECEIVE）",
        "passed": all(queuing_ok), "partial": a11_score,
        "note": ("无队列端口" if not spec["queuing_ports"] else
                 "全部正确" if all(queuing_ok) else "部分端口方向/API不匹配")
    })

    # ── A12：任务循环体内不应有 GET_QUEUING_PORT_ID ──────────────────────
    # 端口ID应在 appMain 中创建，不应在任务循环里重复查询
    a12_pass = "GET_QUEUING_PORT_ID" not in act_c
    details.append({
        "id": "A12", "weight": 1.5,
        "desc": "activity.c 中不含 GET_QUEUING_PORT_ID（端口ID应在appMain创建）",
        "passed": a12_pass, "partial": float(a12_pass),
        "note": ("正确" if a12_pass else
                 "在任务循环中调用了GET_QUEUING_PORT_ID（错误：应在appMain中CREATE_QUEUING_PORT）")
    })

    # ── 加权得分 ─────────────────────────────────────────────────────────
    total_weight = sum(d["weight"] for d in details)
    weighted_sum = sum(d["partial"] * d["weight"] for d in details)
    score = round(weighted_sum / total_weight * 100, 1)

    return {"score": score, "details": details}
