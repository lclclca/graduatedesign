"""
维度3：语义一致性（Semantic Consistency）权重 30%

检查生成代码与 AADL 规格的对应关系：
  C1  任务数量正确
  C2  任务名称全部正确
  C3  任务周期正确（检查 main.c 中 tattr.PERIOD 的纳秒值）
  C4  任务优先级正确（检查 main.c 中 tattr.BASE_PRIORITY 值）
  C5  分区间端口名称正确（出现在代码中）
  C6  分区内黑板名称正确
  C7  分区内缓冲区名称正确
  C8  子程序调用名称正确
  C9  deployment.h 中 NB_THREADS 值正确
  C10 deployment.h 中 NB_QUEUEINGS / NB_SAMPLINGS 值正确
"""

import re
from typing import Dict


def _read(path: str) -> str:
    try:
        with open(path, encoding="utf-8", errors="replace") as f:
            return f.read()
    except FileNotFoundError:
        return ""


def check(gen_dir: str, spec: dict) -> Dict:
    import os
    details = []
    main_c   = _read(os.path.join(gen_dir, "main.c"))
    act_c    = _read(os.path.join(gen_dir, "activity.c"))
    act_h    = _read(os.path.join(gen_dir, "activity.h"))
    deploy_h = _read(os.path.join(gen_dir, "deployment.h"))
    sub_c    = _read(os.path.join(gen_dir, "subprograms.c"))
    sub_h    = _read(os.path.join(gen_dir, "subprograms.h"))
    all_code = main_c + act_c + act_h + sub_c + sub_h

    task_names = [t["name"] for t in spec["tasks"]]

    # ── C1：任务数量 ─────────────────────────────────────────────────────
    found_tasks = re.findall(r"\bvoid\s*\*\s*(task\w+)_job\s*\(", act_c)
    found_count = len(set(found_tasks))
    expected_count = len(task_names)
    c1_score = 1.0 if found_count == expected_count else (
               0.7 if abs(found_count - expected_count) == 1 else
               max(0.0, 1.0 - abs(found_count - expected_count) / expected_count))
    details.append({
        "id": "C1", "weight": 2.0,
        "desc": f"任务数量（期望 {expected_count}，实际 {found_count}）",
        "passed": found_count == expected_count, "partial": c1_score,
        "note": f"找到任务: {sorted(set(found_tasks))}"
    })

    # ── C2：任务名称全部正确 ─────────────────────────────────────────────
    wrong_names = [t for t in task_names if t not in found_tasks]
    extra_names  = [t for t in found_tasks if t not in task_names]
    c2_score = max(0, len(task_names) - len(wrong_names)) / len(task_names)
    details.append({
        "id": "C2", "weight": 2.0,
        "desc": f"任务名称与 AADL 一致（{task_names}）",
        "passed": len(wrong_names) == 0, "partial": c2_score,
        "note": (f"缺失: {wrong_names}；多余: {extra_names}"
                 if (wrong_names or extra_names) else "全部匹配")
    })

    # ── C3：任务周期（tattr.PERIOD = X ns）──────────────────────────────
    period_errors = []
    for t in spec["tasks"]:
        expected_ns = int(t["period_ms"] * 1_000_000)
        # 检查是否存在正确的纳秒值（允许 ll 后缀）
        pattern = rf'(?:tattr\.PERIOD|\.PERIOD)\s*=\s*{expected_ns}(?:ll|LL)?'
        if not re.search(pattern, main_c):
            # 也接受十六进制或其他形式不太可能，但允许只差1个数量级（写成ms的错误）
            period_errors.append(f"{t['name']}(期望{expected_ns}ns)")
    c3_score = max(0, len(task_names) - len(period_errors)) / len(task_names)
    details.append({
        "id": "C3", "weight": 1.5,
        "desc": "任务周期值（纳秒）与 AADL 一致",
        "passed": len(period_errors) == 0, "partial": c3_score,
        "note": f"错误: {period_errors}" if period_errors else "全部正确"
    })

    # ── C4：任务优先级（tattr.BASE_PRIORITY = N）────────────────────────
    priority_errors = []
    for t in spec["tasks"]:
        pattern = rf'(?:tattr\.BASE_PRIORITY|\.BASE_PRIORITY)\s*=\s*{t["priority"]}\b'
        if not re.search(pattern, main_c):
            priority_errors.append(f"{t['name']}(期望优先级{t['priority']})")
    c4_score = max(0, len(task_names) - len(priority_errors)) / len(task_names)
    details.append({
        "id": "C4", "weight": 1.5,
        "desc": "任务优先级与 AADL 一致",
        "passed": len(priority_errors) == 0, "partial": c4_score,
        "note": f"错误: {priority_errors}" if priority_errors else "全部正确"
    })

    # ── C5：分区间端口名称出现在代码中 ──────────────────────────────────
    all_ports = (spec["sampling_ports"] + spec["queuing_ports"])
    missing_ports = []
    for p in all_ports:
        # 端口名应出现在字符串字面量（CREATE时）或变量名中
        if p["name"] not in all_code:
            missing_ports.append(p["name"])
    c5_score = max(0, len(all_ports) - len(missing_ports)) / len(all_ports) if all_ports else 1.0
    details.append({
        "id": "C5", "weight": 2.0,
        "desc": f"分区间端口名称出现在代码中（{[p['name'] for p in all_ports]}）",
        "passed": len(missing_ports) == 0, "partial": c5_score,
        "note": (f"缺失端口名: {missing_ports}" if missing_ports else
                 "无分区间端口" if not all_ports else "全部出现")
    })

    # ── C6：黑板名称出现在代码中 ────────────────────────────────────────
    # 兼容两种格式：DIMA specs 为字符串列表，IMA2 specs 为字典列表 {"name": ...}
    bb_names = [b["name"] if isinstance(b, dict) else b for b in spec["blackboards"]]
    missing_bb = [n for n in bb_names if n not in all_code]
    c6_score = (max(0, len(bb_names) - len(missing_bb)) /
                len(bb_names)) if bb_names else 1.0
    details.append({
        "id": "C6", "weight": 1.5,
        "desc": f"黑板名称出现在代码中（{bb_names}）",
        "passed": len(missing_bb) == 0, "partial": c6_score,
        "note": (f"缺失黑板名: {missing_bb}" if missing_bb else
                 "无黑板" if not bb_names else "全部出现")
    })

    # ── C7：缓冲区名称出现在代码中 ──────────────────────────────────────
    # 兼容两种格式：DIMA specs 为字符串列表，IMA2 specs 为字典列表 {"name": ..., "max_nb": ...}
    buf_names = [b["name"] if isinstance(b, dict) else b for b in spec["buffers"]]
    missing_buf = [n for n in buf_names if n not in all_code]
    c7_score = (max(0, len(buf_names) - len(missing_buf)) /
                len(buf_names)) if buf_names else 1.0
    details.append({
        "id": "C7", "weight": 1.5,
        "desc": f"缓冲区名称出现在代码中（{buf_names}）",
        "passed": len(missing_buf) == 0, "partial": c7_score,
        "note": (f"缺失缓冲区名: {missing_buf}" if missing_buf else
                 "无缓冲区" if not buf_names else "全部出现")
    })

    # ── C8：子程序名称出现在代码中 ──────────────────────────────────────
    missing_spg = [s for s in spec["subprograms"] if s not in all_code]
    c8_score = (max(0, len(spec["subprograms"]) - len(missing_spg)) /
                len(spec["subprograms"])) if spec["subprograms"] else 1.0
    details.append({
        "id": "C8", "weight": 1.0,
        "desc": f"子程序名称出现在代码中（{spec['subprograms']}）",
        "passed": len(missing_spg) == 0, "partial": c8_score,
        "note": (f"缺失子程序: {missing_spg}" if missing_spg else
                 "无子程序" if not spec["subprograms"] else "全部出现")
    })

    # ── C9：deployment.h NB_THREADS 正确 ────────────────────────────────
    m = re.search(r"#define\s+ACoreOS653_CONFIG_NB_THREADS\s+(\d+)", deploy_h)
    got_threads = int(m.group(1)) if m else -1
    c9_pass = got_threads == spec["deployment"]["nb_threads"]
    details.append({
        "id": "C9", "weight": 1.5,
        "desc": f"deployment.h NB_THREADS（期望 {spec['deployment']['nb_threads']}）",
        "passed": c9_pass,
        "partial": 1.0 if c9_pass else (
                   0.5 if abs(got_threads - spec["deployment"]["nb_threads"]) == 1 else 0.0),
        "note": f"实际值: {got_threads}"
    })

    # ── C10：deployment.h NB_QUEUEINGS / NB_SAMPLINGS 正确 ──────────────
    mq = re.search(r"#define\s+ACoreOS653_CONFIG_NB_QUEUEINGS\s+(\d+)", deploy_h)
    ms = re.search(r"#define\s+ACoreOS653_CONFIG_NB_SAMPLINGS\s+(\d+)", deploy_h)
    got_q = int(mq.group(1)) if mq else -1
    got_s = int(ms.group(1)) if ms else -1
    exp_q = spec["deployment"]["nb_queueings"]
    exp_s = spec["deployment"]["nb_samplings"]
    c10_q_ok = got_q == exp_q
    c10_s_ok = got_s == exp_s
    c10_score = (float(c10_q_ok) + float(c10_s_ok)) / 2
    details.append({
        "id": "C10", "weight": 1.5,
        "desc": f"deployment.h NB_QUEUEINGS={exp_q}, NB_SAMPLINGS={exp_s}",
        "passed": c10_q_ok and c10_s_ok, "partial": c10_score,
        "note": f"QUEUEINGS: 期望{exp_q} 实际{got_q}; SAMPLINGS: 期望{exp_s} 实际{got_s}"
    })

    # ── 加权得分 ─────────────────────────────────────────────────────────
    total_weight = sum(d["weight"] for d in details)
    weighted_sum = sum(d["partial"] * d["weight"] for d in details)
    score = round(weighted_sum / total_weight * 100, 1)

    return {"score": score, "details": details}
