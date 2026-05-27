"""
ARINC 653 资源合规检查（Compliance Check）

本模块检查生成代码是否符合 ARINC 653 的资源使用规范，
重点验证"资源被创建后是否在任务函数中实际读写"，
弥补现有三维评估框架中 C6/C7 仅检查名称出现位置的盲点。

规则列表：
  R1  黑板名称出现在 activity.c（任务函数实际使用，非仅在 main.c CREATE）
  R2  缓冲区名称出现在 activity.c
  R3  采样 SOURCE 端口：WRITE_SAMPLING_MESSAGE 出现在 activity.c
  R4  采样 DEST 端口：READ_SAMPLING_MESSAGE 出现在 activity.c
  R5  队列 SOURCE 端口：SEND_QUEUING_MESSAGE 出现在 activity.c
  R6  队列 DEST 端口：RECEIVE_QUEUING_MESSAGE 出现在 activity.c
  R7  HM 回调：module_HM_callback + partition_HM_callback 定义在 main.c
  R8  activity.c 中 while(1) 内无 CREATE_ 调用（禁止运行时动态初始化）

判定逻辑：
  - R1/R2/R3/R4/R5/R6 检查特定 API 或名称是否出现在 activity.c
  - R7 检查 main.c 中函数定义
  - R8 检查 activity.c 中第一个 while 之后是否含有 CREATE_

每条规则返回：
  {
    "rule":  "R1",
    "item":  "bb_data_link",
    "pass":  True/False,
    "msg":   "黑板 bb_data_link 出现在 activity.c"
  }

注意：本模块不修改现有 evaluate.py 的分数，仅作为附加合规报告。
"""

import os
import re


def _read(gen_dir: str, fname: str) -> str:
    """读取生成目录中的指定文件，文件不存在返回空字符串"""
    try:
        with open(os.path.join(gen_dir, fname), encoding="utf-8", errors="replace") as f:
            return f.read()
    except FileNotFoundError:
        return ""


def _name(x) -> str:
    """兼容字符串和字典两种格式的资源名提取"""
    return x["name"] if isinstance(x, dict) else x


def _strip_comments(code: str) -> str:
    """去除 C 块注释和行注释，避免注释中的关键词干扰检测"""
    code = re.sub(r'/\*.*?\*/', ' ', code, flags=re.DOTALL)
    code = re.sub(r'//[^\n]*', ' ', code)
    return code


def check(gen_dir: str, spec: dict) -> list:
    """
    运行所有 ARINC 653 合规检查。

    参数:
        gen_dir: 生成代码所在目录（含 activity.c, main.c 等）
        spec:    分区规格 JSON（已解析为 dict）

    返回:
        list of dict，每条结果含 rule/item/pass/msg 字段
    """
    act_c  = _strip_comments(_read(gen_dir, "activity.c"))
    main_c = _strip_comments(_read(gen_dir, "main.c"))
    results = []

    # ── R1：黑板读写 API 均出现在 activity.c ──────────────────────────
    # 仅检查名称是否出现会被 extern 声明欺骗（extern BLACKBOARD_ID_TYPE bb_xx_id
    # 使名称出现但黑板从未真正使用）。必须同时存在 DISPLAY_BLACKBOARD（写入）
    # 和 READ_BLACKBOARD（读取）调用，才能确认黑板被实际使用。
    has_display = "DISPLAY_BLACKBOARD" in act_c
    has_read_bb = "READ_BLACKBOARD"    in act_c
    blackboards = spec.get("blackboards", [])
    if not blackboards:
        results.append({
            "rule": "R1", "item": "(无黑板)", "pass": True,
            "msg": "无黑板，跳过 R1"
        })
    else:
        ok = has_display and has_read_bb
        missing = []
        if not has_display: missing.append("DISPLAY_BLACKBOARD（写入）")
        if not has_read_bb: missing.append("READ_BLACKBOARD（读取）")
        results.append({
            "rule": "R1", "item": "blackboard read/write",
            "pass": ok,
            "msg": ("DISPLAY_BLACKBOARD 与 READ_BLACKBOARD 均出现在 activity.c ✓"
                    if ok else
                    f"activity.c 缺少：{', '.join(missing)}")
        })

    # ── R2：缓冲区收发 API 均出现在 activity.c ────────────────────────
    # 同 R1，仅检查名称会被 extern 声明欺骗。必须同时存在 SEND_BUFFER
    # 和 RECEIVE_BUFFER，才能确认缓冲区双端均已实现。
    has_send_buf = "SEND_BUFFER"    in act_c
    has_recv_buf = "RECEIVE_BUFFER" in act_c
    buffers = spec.get("buffers", [])
    if not buffers:
        results.append({
            "rule": "R2", "item": "(无缓冲区)", "pass": True,
            "msg": "无缓冲区，跳过 R2"
        })
    else:
        ok = has_send_buf and has_recv_buf
        missing = []
        if not has_send_buf: missing.append("SEND_BUFFER（发送）")
        if not has_recv_buf: missing.append("RECEIVE_BUFFER（接收）")
        results.append({
            "rule": "R2", "item": "buffer send/recv",
            "pass": ok,
            "msg": ("SEND_BUFFER 与 RECEIVE_BUFFER 均出现在 activity.c ✓"
                    if ok else
                    f"activity.c 缺少：{', '.join(missing)}")
        })

    # ── R3/R4：采样端口方向与 API 匹配（在 activity.c 中）────────────
    # ARINC 653: SOURCE 端口调用 WRITE_SAMPLING_MESSAGE，
    #            DESTINATION 端口调用 READ_SAMPLING_MESSAGE。
    sampling = spec.get("sampling_ports", [])
    if not sampling:
        results.append({
            "rule": "R3/R4", "item": "(无采样端口)", "pass": True,
            "msg": "无采样端口，跳过 R3/R4"
        })
    else:
        for p in sampling:
            pname = p["name"]
            if p["direction"] == "SOURCE":
                ok = "WRITE_SAMPLING_MESSAGE" in act_c
                results.append({
                    "rule": "R3", "item": pname, "pass": ok,
                    "msg": f"采样SOURCE {pname}：activity.c 中"
                           f"{'有' if ok else '【无】'} WRITE_SAMPLING_MESSAGE"
                })
            else:
                ok = "READ_SAMPLING_MESSAGE" in act_c
                results.append({
                    "rule": "R4", "item": pname, "pass": ok,
                    "msg": f"采样DEST {pname}：activity.c 中"
                           f"{'有' if ok else '【无】'} READ_SAMPLING_MESSAGE"
                })

    # ── R5/R6：队列端口方向与 API 匹配（在 activity.c 中）────────────
    # ARINC 653: SOURCE 端口调用 SEND_QUEUING_MESSAGE，
    #            DESTINATION 端口调用 RECEIVE_QUEUING_MESSAGE。
    queuing = spec.get("queuing_ports", [])
    if not queuing:
        results.append({
            "rule": "R5/R6", "item": "(无队列端口)", "pass": True,
            "msg": "无队列端口，跳过 R5/R6"
        })
    else:
        for p in queuing:
            pname = p["name"]
            if p["direction"] == "SOURCE":
                ok = "SEND_QUEUING_MESSAGE" in act_c
                results.append({
                    "rule": "R5", "item": pname, "pass": ok,
                    "msg": f"队列SOURCE {pname}：activity.c 中"
                           f"{'有' if ok else '【无】'} SEND_QUEUING_MESSAGE"
                })
            else:
                ok = "RECEIVE_QUEUING_MESSAGE" in act_c
                results.append({
                    "rule": "R6", "item": pname, "pass": ok,
                    "msg": f"队列DEST {pname}：activity.c 中"
                           f"{'有' if ok else '【无】'} RECEIVE_QUEUING_MESSAGE"
                })

    # ── R7：HM 回调函数定义在 main.c ───────────────────────────────────
    # ARINC 653 要求每个分区提供模块级和分区级健康监控回调。
    has_module_hm    = "module_HM_callback" in main_c
    has_partition_hm = "partition_HM_callback" in main_c
    hm_ok = has_module_hm and has_partition_hm
    missing = []
    if not has_module_hm:    missing.append("module_HM_callback")
    if not has_partition_hm: missing.append("partition_HM_callback")
    results.append({
        "rule": "R7", "item": "HM callbacks", "pass": hm_ok,
        "msg": ("HM 回调函数均已定义" if hm_ok
                else f"HM 回调缺失：{', '.join(missing)}")
    })

    # ── R8：activity.c 中 while(1) 内无 CREATE_ 调用 ──────────────────
    # ARINC 653 规定：所有资源必须在 NORMAL 模式切换前（即 appMain 中）
    # 完成初始化，不能在任务运行时动态创建。
    if "while" not in act_c:
        results.append({
            "rule": "R8", "item": "no CREATE in loop", "pass": True,
            "msg": "activity.c 中无 while 循环（无法判断，视为通过）"
        })
    else:
        first_while = act_c.find("while")
        after_while = act_c[first_while:]
        has_create = bool(re.search(r'\bCREATE_\w+\s*\(', after_while))
        results.append({
            "rule": "R8", "item": "no CREATE in loop", "pass": not has_create,
            "msg": ("while(1) 后无 CREATE_ 调用 ✓" if not has_create
                    else "【警告】while(1) 后发现 CREATE_ 调用（运行时初始化违反 ARINC 653）")
        })

    return results
