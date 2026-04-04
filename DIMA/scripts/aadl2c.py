#!/usr/bin/env python3
"""
aadl2c.py  —  AADL IMA Model to ARINC 653 C Code Generator
=============================================================
Translates an IMA (Integrated Modular Avionics) AADL text model
into skeleton ARINC 653 C source files (one directory per partition).

Supported AADL → ARINC 653 mapping
------------------------------------
  process / process implementation → partition  (appMain + resource init)
  thread  / thread  implementation → ARINC 653 PROCESS  (periodic task)
  out/in data port  (Sampling_Refresh_Period)  → SAMPLING_PORT  (inter-partition)
  out/in event data port (Queue_Size)          → QUEUING_PORT   (inter-partition)
  intra-partition data port connection         → BLACKBOARD     (latest-value)
  intra-partition event data port connection   → BUFFER         (FIFO queue)
  data access (protected_data)                 → BLACKBOARD     (protected shared data)

Usage
-----
  python3 aadl2c.py --partitions  <DIMA_partitions.aadl>
                    --threads     <DIMA_threads.aadl>
                    --outdir      <output_directory>
                    [--module     <module_name>]

Example
-------
  python3 aadl2c.py \\
      --partitions ../Sys/DIMA_partitions.aadl \\
      --threads    ../Sys/DIMA_threads.aadl \\
      --outdir     ../C_Code_Gen/generated \\
      --module     M1
"""

import argparse
import json
import os
import re
import sys
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Tuple


# ─────────────────────────────────────────────────────────────────────────────
#  Data model
# ─────────────────────────────────────────────────────────────────────────────

@dataclass
class PortDef:
    name: str
    direction: str          # 'in' | 'out' | 'inout'
    port_type: str          # 'data' | 'event_data'
    base_type: str          # e.g. 'Base_Types::Integer'
    queue_size: int = 0     # Queue_Size property (queuing ports)
    refresh_ns: int = 0     # Sampling_Refresh_Period in nanoseconds
    timeout_ns: int = 0     # ARINC653::Timeout in nanoseconds
    queuing_disc: str = 'FIFO'


@dataclass
class ThreadDef:
    name: str               # e.g. 'Tsk11'
    impl: str               # e.g. 'Tsk11.impl'
    features: Dict[str, PortDef] = field(default_factory=dict)
    # scheduling properties
    period_ns: int = 50_000_000
    deadline_ns: int = 50_000_000
    priority: int = 1
    stack_size: int = 8192
    time_capacity_ns: int = 50_000_000
    deadline_type: str = 'SOFT'
    dispatch_abs_ns: int = 0
    calls: List[str] = field(default_factory=list)  # subprogram names


@dataclass
class DataAccessDef:
    """Represents 'data access sem <-> taskXX.portname' in a process impl."""
    data_comp: str          # name of the data subcomponent (e.g. 'sem')
    task_name: str          # thread subcomponent name (e.g. 'task12')
    port_name: str          # port on thread (e.g. 'task2_printer')
    access_right: str       # 'read_only' | 'write_only' | 'read_write'


@dataclass
class ConnectionDef:
    name: str
    kind: str               # 'port' | 'data_access'
    src: str                # 'component.feature'  or just process-level feature
    dst: str


@dataclass
class PartitionDef:
    name: str               # process name, e.g. 'P1'
    impl: str               # e.g. 'P1.impl'
    features: Dict[str, PortDef] = field(default_factory=dict)
    task_subcomps: Dict[str, str] = field(default_factory=dict)  # inst_name → type_name
    data_subcomps: Dict[str, str] = field(default_factory=dict)  # inst_name → type_name
    connections: List[ConnectionDef] = field(default_factory=list)
    data_accesses: List[DataAccessDef] = field(default_factory=list)
    dal: str = 'LEVEL_A'


# ─────────────────────────────────────────────────────────────────────────────
#  Utility helpers
# ─────────────────────────────────────────────────────────────────────────────

def _strip_comments(text: str) -> str:
    """Remove AADL line comments (-- ...) and block content."""
    return re.sub(r'--[^\n]*', '', text)


def _norm(text: str) -> str:
    """Collapse whitespace for easier regex matching."""
    return re.sub(r'\s+', ' ', text).strip()


def _parse_time(value: str) -> int:
    """Convert AADL time literal to nanoseconds (int)."""
    value = value.strip()
    m = re.match(r'([0-9]+(?:\.[0-9]+)?)\s*(ns|us|ms|sec|s)', value, re.I)
    if not m:
        return 0
    num = float(m.group(1))
    unit = m.group(2).lower()
    factors = {'ns': 1, 'us': 1_000, 'ms': 1_000_000,
               'sec': 1_000_000_000, 's': 1_000_000_000}
    return int(num * factors.get(unit, 1))


def _parse_size(value: str) -> int:
    """Convert AADL memory size literal to bytes."""
    value = value.strip()
    m = re.match(r'([0-9]+)\s*(kbyte|byte|kb|b)?', value, re.I)
    if not m:
        return 8192
    num = int(m.group(1))
    unit = (m.group(2) or 'b').lower()
    if unit in ('kbyte', 'kb'):
        num *= 1024
    return num


# ─────────────────────────────────────────────────────────────────────────────
#  AADL Parser
# ─────────────────────────────────────────────────────────────────────────────

class AadlParser:
    """
    Lightweight regex-based parser for AADL text models.
    Handles process, process implementation, thread, thread implementation.
    """

    def __init__(self):
        self.threads: Dict[str, ThreadDef] = {}    # name.impl → ThreadDef
        self.partitions: Dict[str, PartitionDef] = {}  # name.impl → PartitionDef

    # ── public entry point ──────────────────────────────────────────────────

    def parse_file(self, path: str):
        with open(path, encoding='utf-8', errors='replace') as f:
            raw = f.read()
        text = _strip_comments(raw)
        self._parse_text(text)

    # ── top-level dispatcher ────────────────────────────────────────────────

    def _parse_text(self, text: str):
        """Extract all top-level component declarations."""
        # Match: thread Xxx ... end Xxx;
        # Match: thread implementation Xxx.impl ... end Xxx.impl;
        # Match: process Xxx ... end Xxx;
        # Match: process implementation Xxx.impl ... end Xxx.impl;

        pattern = re.compile(
            r'\b(thread|process)\s+(implementation\s+)?'
            r'([\w]+(?:\.[\w]+)?)'   # name (possibly name.impl)
            r'(.*?)'
            r'\bend\s+\3\s*;',
            re.S | re.I
        )

        for m in pattern.finditer(text):
            comp_kind  = m.group(1).lower()      # 'thread' | 'process'
            is_impl    = bool(m.group(2))
            comp_name  = m.group(3).strip()
            body       = m.group(4)

            if comp_kind == 'thread':
                if is_impl:
                    self._parse_thread_impl(comp_name, body)
                else:
                    self._parse_thread_decl(comp_name, body)
            elif comp_kind == 'process':
                if is_impl:
                    self._parse_process_impl(comp_name, body)
                else:
                    self._parse_process_decl(comp_name, body)

    # ── thread declaration ──────────────────────────────────────────────────

    def _parse_thread_decl(self, name: str, body: str):
        base = name.split('.')[0]
        td = self.threads.setdefault(base, ThreadDef(name=base, impl=base + '.impl'))
        features = self._parse_features(body)
        td.features.update(features)

    # ── thread implementation ───────────────────────────────────────────────

    def _parse_thread_impl(self, name: str, body: str):
        base = name.split('.')[0]
        td = self.threads.setdefault(base, ThreadDef(name=base, impl=name))
        td.impl = name

        # Properties section
        props_m = re.search(r'\bproperties\b(.*?)(?=\bcalls\b|\bconnections\b|\bend\b|$)', body, re.S | re.I)
        if props_m:
            self._apply_thread_props(td, props_m.group(1))

        # Calls section (subprogram calls)
        calls_m = re.search(r'\bcalls\b.*?\{(.*?)\}', body, re.S | re.I)
        if calls_m:
            for spg_m in re.finditer(r':\s*subprogram\s+([\w:]+)', calls_m.group(1)):
                spg_name = spg_m.group(1).split('::')[-1]
                if spg_name not in td.calls:
                    td.calls.append(spg_name)

    def _apply_thread_props(self, td: ThreadDef, props_text: str):
        """Parse scheduling properties from thread implementation."""
        def _get(pattern):
            m = re.search(pattern, props_text, re.I)
            return m.group(1).strip() if m else None

        if (v := _get(r'\bPeriod\s*=>\s*([^;]+)')):
            td.period_ns = _parse_time(v)
        if (v := _get(r'\bDeadline\s*=>\s*([^;]+)')):
            td.deadline_ns = _parse_time(v)
        if (v := _get(r'\bPriority\s*=>\s*(\d+)')):
            td.priority = int(v)
        if (v := _get(r'\bStack_Size\s*=>\s*([^;]+)')):
            td.stack_size = _parse_size(v)
        if (v := _get(r'Time_Capacity\s*=>\s*([^;]+)')):
            td.time_capacity_ns = _parse_time(v)
        if (v := _get(r'Deadline_Type\s*=>\s*(\w+)')):
            td.deadline_type = v.upper()
        if (v := _get(r'Dispatch_Absolute_Time\s*=>\s*([^;]+)')):
            td.dispatch_abs_ns = _parse_time(v)

    # ── process declaration ─────────────────────────────────────────────────

    def _parse_process_decl(self, name: str, body: str):
        base = name.split('.')[0]
        pd = self.partitions.setdefault(base, PartitionDef(name=base, impl=base + '.impl'))
        features = self._parse_features(body)
        pd.features.update(features)

    # ── process implementation ──────────────────────────────────────────────

    def _parse_process_impl(self, name: str, body: str):
        base = name.split('.')[0]
        pd = self.partitions.setdefault(base, PartitionDef(name=base, impl=name))
        pd.impl = name

        # Subcomponents
        sub_m = re.search(r'\bsubcomponents\b(.*?)(?=\bconnections\b|\bproperties\b|\bend\b)', body, re.S | re.I)
        if sub_m:
            for s in re.finditer(r'(\w+)\s*:\s*(thread|data)\s+([\w:]+)', sub_m.group(1), re.I):
                inst, kind, typ = s.group(1), s.group(2).lower(), s.group(3)
                type_base = typ.split('::')[-1].split('.')[0]
                if kind == 'thread':
                    pd.task_subcomps[inst] = type_base
                else:
                    pd.data_subcomps[inst] = type_base

        # Connections
        conn_m = re.search(r'\bconnections\b(.*?)(?=\bproperties\b|\bend\b)', body, re.S | re.I)
        if conn_m:
            self._parse_connections(pd, conn_m.group(1))

        # Properties
        prop_m = re.search(r'\bproperties\b(.*?)(?=\bend\b)', body, re.S | re.I)
        if prop_m:
            dal_m = re.search(r'ARINC653::DAL\s*=>\s*(LEVEL_\w+)', prop_m.group(1), re.I)
            if dal_m:
                pd.dal = dal_m.group(1)

    def _parse_connections(self, pd: PartitionDef, text: str):
        """Parse port and data access connections inside a process implementation."""
        for m in re.finditer(
            r'(\w+)\s*:\s*port\s+([\w.]+)\s*->\s*([\w.]+)\s*;', text, re.I
        ):
            pd.connections.append(ConnectionDef(
                name=m.group(1), kind='port',
                src=m.group(2).strip(), dst=m.group(3).strip()
            ))

        # data access: sem <-> task12.task2_printer
        for m in re.finditer(
            r'(\w+)\s*:\s*data\s+access\s+(\w+)\s*<->\s*([\w.]+)\s*;', text, re.I
        ):
            comp_inst, data_comp, accessor = m.group(1), m.group(2), m.group(3)
            parts = accessor.split('.')
            if len(parts) == 2:
                task_inst, port_name = parts
            else:
                task_inst, port_name = accessor, ''
            pd.data_accesses.append(DataAccessDef(
                data_comp=data_comp,
                task_name=task_inst,
                port_name=port_name,
                access_right='read_write'  # refined later from thread feature props
            ))

    # ── feature parser (shared by process and thread) ───────────────────────

    def _parse_features(self, body: str) -> Dict[str, PortDef]:
        """Extract port features from a component body."""
        features: Dict[str, PortDef] = {}
        feat_m = re.search(r'\bfeatures\b(.*?)(?=\bend\b|\bproperties\b|\bsubcomponents\b|\bconnections\b|\bcalls\b|$)', body, re.S | re.I)
        if not feat_m:
            return features

        feat_text = feat_m.group(1)

        # Pattern: name : [in|out|inout] [event] data port Type [{props}]
        pat = re.compile(
            r'(\w+)\s*:\s*(in|out|inout)\s+(event\s+)?data\s+port\s+([\w:]+)'
            r'(?:\s*\{([^}]*)\})?',
            re.S | re.I
        )
        for m in pat.finditer(feat_text):
            fname   = m.group(1)
            dirn    = m.group(2).lower()
            is_evt  = bool(m.group(3))
            btype   = m.group(4).strip()
            props   = m.group(5) or ''

            pdef = PortDef(
                name=fname,
                direction=dirn,
                port_type='event_data' if is_evt else 'data',
                base_type=btype
            )

            # Parse inline properties
            qs = re.search(r'Queue_Size\s*=>\s*(\d+)', props, re.I)
            if qs:
                pdef.queue_size = int(qs.group(1))
            rp = re.search(r'Sampling_Refresh_Period\s*=>\s*([^;,}]+)', props, re.I)
            if rp:
                pdef.refresh_ns = _parse_time(rp.group(1))
            to = re.search(r'ARINC653::Timeout\s*=>\s*([^;,}]+)', props, re.I)
            if to:
                pdef.timeout_ns = _parse_time(to.group(1))
            qd = re.search(r'Queueing_Discipline\s*=>\s*(\w+)', props, re.I)
            if qd:
                pdef.queuing_disc = qd.group(1).upper()

            features[fname] = pdef

        return features


# ─────────────────────────────────────────────────────────────────────────────
#  Resource analyser  (AADL model → ARINC 653 resource list)
# ─────────────────────────────────────────────────────────────────────────────

@dataclass
class SamplingRes:
    var_name: str; port_name: str; direction: str; refresh_ns: int

@dataclass
class QueuingRes:
    var_name: str; port_name: str; direction: str
    max_msgs: int; timeout_ns: int; disc: str

@dataclass
class BlackboardRes:
    var_name: str; bb_name: str; writer_task: str; reader_task: str

@dataclass
class BufferRes:
    var_name: str; buf_name: str; writer_task: str; reader_task: str
    max_msgs: int = 10; timeout_ns: int = 5_000_000

@dataclass
class TaskRes:
    inst_name: str          # e.g. 'task11'
    type_name: str          # e.g. 'Tsk11'
    thread: ThreadDef
    index: int              # position in arinc_threads[]


@dataclass
class PartitionResources:
    partition: PartitionDef
    tasks: List[TaskRes] = field(default_factory=list)
    sampling: List[SamplingRes] = field(default_factory=list)
    queuing: List[QueuingRes] = field(default_factory=list)
    blackboards: List[BlackboardRes] = field(default_factory=list)
    buffers: List[BufferRes] = field(default_factory=list)
    prefix: str = ''        # e.g. 'ps1'
    # Per-task inter-partition port routing (from partition-level connections)
    # Maps task-instance-name → set of partition-level port names
    task_interpart_reads: Dict[str, set] = field(default_factory=dict)
    task_interpart_writes: Dict[str, set] = field(default_factory=dict)


def analyse_partition(pd: PartitionDef, threads: Dict[str, ThreadDef],
                      part_prefix: str) -> PartitionResources:
    """
    Analyse one partition definition and produce the full resource list.
    """
    pr = PartitionResources(partition=pd, prefix=part_prefix)

    # ── 1. Tasks (sorted by priority for deterministic output) ──────────────
    task_list: List[Tuple[str, str, ThreadDef]] = []  # (inst, type_base, td)
    for inst, type_base in pd.task_subcomps.items():
        td = threads.get(type_base)
        if td is None:
            print(f"[WARN] Thread type '{type_base}' not found for instance '{inst}'",
                  file=sys.stderr)
            td = ThreadDef(name=type_base, impl=type_base + '.impl')
        task_list.append((inst, type_base, td))

    task_list.sort(key=lambda t: t[2].priority)

    for idx, (inst, type_base, td) in enumerate(task_list):
        pr.tasks.append(TaskRes(inst_name=inst, type_name=type_base,
                                thread=td, index=idx))

    # ── 2. Inter-partition ports (from process features) ─────────────────────
    for fname, pdef in pd.features.items():
        vname = f'{part_prefix}_{fname}_id'
        if pdef.port_type == 'data':
            # Sampling port
            arinc_dir = 'SOURCE' if pdef.direction == 'out' else 'DESTINATION'
            pr.sampling.append(SamplingRes(
                var_name=vname, port_name=fname,
                direction=arinc_dir,
                refresh_ns=pdef.refresh_ns if pdef.refresh_ns else 100
            ))
        elif pdef.port_type == 'event_data':
            # Queuing port
            arinc_dir = 'SOURCE' if pdef.direction == 'out' else 'DESTINATION'
            pr.queuing.append(QueuingRes(
                var_name=vname, port_name=fname,
                direction=arinc_dir,
                max_msgs=pdef.queue_size if pdef.queue_size else 10,
                timeout_ns=pdef.timeout_ns if pdef.timeout_ns else 5_000_000,
                disc=pdef.queuing_disc
            ))

    # ── 3. Intra-partition connections → BLACKBOARD or BUFFER ────────────────
    # Build a map from process-level feature names to know which are inter-partition
    inter_features = set(pd.features.keys())

    # Map: task-instance → set of its port names (from thread features)
    task_ports: Dict[str, Dict[str, PortDef]] = {}
    for inst, type_base, td in task_list:
        task_ports[inst] = td.features

    bb_set: set = set()   # avoid duplicates (bidirectional BB connections)
    buf_set: set = set()

    # ── 3a. Build per-task inter-partition port routing maps ─────────────────
    # Parse connections that route partition-level ports to/from tasks
    for conn in pd.connections:
        src_parts = conn.src.split('.')
        dst_parts = conn.dst.split('.')

        # Only process connections involving partition-level features
        if len(src_parts) == 1 and len(dst_parts) == 2:
            # partition_port → task.thread_port  (task reads from partition port)
            part_port = src_parts[0]
            task_inst = dst_parts[0]
            if part_port in inter_features:
                pr.task_interpart_reads.setdefault(task_inst, set()).add(part_port)
        elif len(src_parts) == 2 and len(dst_parts) == 1:
            # task.thread_port → partition_port  (task writes to partition port)
            task_inst = src_parts[0]
            part_port = dst_parts[0]
            if part_port in inter_features:
                pr.task_interpart_writes.setdefault(task_inst, set()).add(part_port)

    for conn in pd.connections:
        src_parts = conn.src.split('.')
        dst_parts = conn.dst.split('.')

        # Skip connections that link to inter-partition process features
        is_inter = (len(src_parts) == 1 or len(dst_parts) == 1)
        if is_inter:
            continue

        src_task, src_port = src_parts[0], src_parts[1]
        dst_task, dst_port = dst_parts[0], dst_parts[1]

        # Determine port type from thread feature
        src_pdef = task_ports.get(src_task, {}).get(src_port)
        dst_pdef = task_ports.get(dst_task, {}).get(dst_port)
        pdef = src_pdef or dst_pdef

        if pdef is None:
            continue

        res_name = conn.name          # use connection name as resource name
        pair_key = tuple(sorted([src_task, dst_task, res_name]))

        if pdef.port_type == 'data':
            # BLACKBOARD  (latest-value semantics)
            if pair_key in bb_set:
                continue
            bb_set.add(pair_key)
            vname = f'{part_prefix}_bb_{res_name}_id'
            pr.blackboards.append(BlackboardRes(
                var_name=vname, bb_name=f'bb_{res_name}',
                writer_task=src_task, reader_task=dst_task
            ))
        else:
            # BUFFER  (FIFO queue semantics)
            if pair_key in buf_set:
                continue
            buf_set.add(pair_key)
            vname = f'{part_prefix}_buf_{res_name}_id'
            max_m = pdef.queue_size if pdef.queue_size else 10
            tmo   = pdef.timeout_ns if pdef.timeout_ns else 5_000_000
            pr.buffers.append(BufferRes(
                var_name=vname, buf_name=f'buf_{res_name}',
                writer_task=src_task, reader_task=dst_task,
                max_msgs=max_m, timeout_ns=tmo
            ))

    # ── 4. Data access connections → BLACKBOARD (protected shared data) ──────
    # Group by data_comp name so each protected_data component → one BB
    da_groups: Dict[str, List[DataAccessDef]] = {}
    for da in pd.data_accesses:
        da_groups.setdefault(da.data_comp, []).append(da)

    for data_comp, das in da_groups.items():
        writer = next((d.task_name for d in das if 'write' in d.access_right or
                       d.port_name.endswith('printer') or d.port_name.endswith('write')),
                      das[0].task_name)
        reader = next((d.task_name for d in das if 'read'  in d.access_right or
                       d.port_name.endswith('receiver') or d.port_name.endswith('read')),
                      das[-1].task_name)
        vname = f'{part_prefix}_bb_{data_comp}_id'
        pr.blackboards.append(BlackboardRes(
            var_name=vname, bb_name=f'bb_{data_comp}',
            writer_task=writer, reader_task=reader
        ))

    return pr


# ─────────────────────────────────────────────────────────────────────────────
#  C Code Generator
# ─────────────────────────────────────────────────────────────────────────────

BANNER = """/*****************************************************/
/*  This file was automatically generated by aadl2c */
/*  Partition: {prefix}  AADL: {impl}              */
/*  DO NOT hand-modify: changes will be overwritten  */
/*****************************************************/
"""

def _banner(pr: PartitionResources) -> str:
    return BANNER.format(prefix=pr.prefix, impl=pr.partition.impl)


def gen_deployment_h(pr: PartitionResources) -> str:
    p = pr.prefix.upper()
    nb_bb  = len(pr.blackboards)
    nb_buf = len(pr.buffers)
    nb_sam = len(pr.sampling)
    nb_que = len(pr.queuing)
    nb_tsk = len(pr.tasks)
    stack  = nb_tsk * 8192

    lines = [
        f'#ifndef __{p}_GENERATED_DEPLOYMENT_H_',
        f'#define __{p}_GENERATED_DEPLOYMENT_H_',
        '',
        _banner(pr),
        '#define IMA2C_RUNTIME_ACoreOS653 1',
        '#define ACoreOS653_GENERATED_CODE 1',
        '',
        f'#define ACoreOS653_CONFIG_NB_THREADS     {nb_tsk}',
        f'#define ACoreOS653_CONFIG_NB_SAMPLINGS   {nb_sam}',
        f'#define ACoreOS653_CONFIG_NB_QUEUEINGS   {nb_que}',
        f'#define ACoreOS653_CONFIG_NB_BLACKBOARDS {nb_bb}',
        f'#define ACoreOS653_CONFIG_NB_BUFFERS     {nb_buf}',
        '',
        '#define ACoreOS653_NEEDS_ARINC653_PARTITION  1',
        '#define ACoreOS653_NEEDS_ARINC653_PROCESS    1',
        f'#define ACoreOS653_NEEDS_ARINC653_SAMPLING   {1 if nb_sam else 0}',
        f'#define ACoreOS653_NEEDS_ARINC653_QUEUEING   {1 if nb_que else 0}',
        f'#define ACoreOS653_NEEDS_ARINC653_BLACKBOARD {1 if nb_bb  else 0}',
        f'#define ACoreOS653_NEEDS_ARINC653_BUFFER     {1 if nb_buf else 0}',
        '#define ACoreOS653_NEEDS_ARINC653_SEMAPHORE  0',
        '#define ACoreOS653_NEEDS_ARINC653_EVENT      0',
        '#define ACoreOS653_NEEDS_MIDDLEWARE          1',
        '#define ACoreOS653_NEEDS_ARINC653_TIME       1',
        '',
        f'#define ACoreOS653_CONFIG_STACKS_SIZE {stack}',
        '',
        '#endif',
    ]
    return '\n'.join(lines) + '\n'


def gen_globals_h(pr: PartitionResources) -> str:
    return f"""\
#ifndef __{pr.prefix.upper()}_GENERATED_GLOBALS_H_
#define __{pr.prefix.upper()}_GENERATED_GLOBALS_H_

{_banner(pr)}
#include <stdio.h>
#include <os/pos/apex/apexLib.h>

#define CHECK_CODE(msg, code) \\
    if ((code) == NO_ERROR) {{ \\
        printf("%s : NO_ERROR\\n", (msg)); \\
    }} else {{ \\
        printf("%s : ERROR(%s)\\n", (msg), codeToStr(code)); \\
    }}

#define CHECK_VALIDCODE(msg, validCode, ret) \\
    if ((validCode) == VALID && (ret) == NO_ERROR) {{ \\
        printf("%s : VALID/NO_ERROR\\n", (msg)); \\
    }} else {{ \\
        printf("%s : INVALID or ERROR(%s)\\n", (msg), codeToStr(ret)); \\
    }}

static char *codeToStr(RETURN_CODE_TYPE retCode)
{{
    switch (retCode)
    {{
        case NO_ERROR:       return "NO_ERROR";
        case NO_ACTION:      return "NO_ACTION";
        case NOT_AVAILABLE:  return "NOT_AVAILABLE";
        case INVALID_PARAM:  return "INVALID_PARAM";
        case INVALID_CONFIG: return "INVALID_CONFIG";
        case INVALID_MODE:   return "INVALID_MODE";
        case TIMED_OUT:      return "TIMED_OUT";
        default:             break;
    }}
    return "UNKNOWN";
}}

#endif
"""


def gen_gtypes_h(pr: PartitionResources) -> str:
    return f"""\
#ifndef __{pr.prefix.upper()}_GENERATED_GTYPES_H_
#define __{pr.prefix.upper()}_GENERATED_GTYPES_H_

{_banner(pr)}
#include <os/pos/apex/apexLib.h>

typedef int integer;

#endif
"""


def gen_activity_h(pr: PartitionResources) -> str:
    lines = [
        f'#ifndef __{pr.prefix.upper()}_GENERATED_ACTIVITY_H_',
        f'#define __{pr.prefix.upper()}_GENERATED_ACTIVITY_H_',
        '',
        _banner(pr),
    ]
    for tr in pr.tasks:
        td = tr.thread
        lines.append(f'/* Periodic task: {tr.inst_name} (AADL: {tr.type_name})')
        lines.append(f' * Period={td.period_ns//1_000_000}ms  '
                     f'Priority={td.priority}  Stack={td.stack_size}B */')
        lines.append(f'void* {tr.inst_name}_job(void);')
        lines.append('')
    lines.append('#endif')
    return '\n'.join(lines) + '\n'


def gen_subprograms_h(pr: PartitionResources) -> str:
    spgs = set()
    for tr in pr.tasks:
        for spg in tr.thread.calls:
            spgs.add(spg)

    lines = [
        f'#ifndef __{pr.prefix.upper()}_GENERATED_SUBPROGRAMS_H_',
        f'#define __{pr.prefix.upper()}_GENERATED_SUBPROGRAMS_H_',
        '',
        _banner(pr),
        '#include "gtypes.h"',
        '',
    ]
    for spg in sorted(spgs):
        lines.append(f'void {spg}(integer *data_in, integer *data_out);')
    lines.append('')
    lines.append('#endif')
    return '\n'.join(lines) + '\n'


def gen_subprograms_c(pr: PartitionResources) -> str:
    spgs = set()
    for tr in pr.tasks:
        for spg in tr.thread.calls:
            spgs.add(spg)

    lines = [
        '#include "subprograms.h"',
        _banner(pr),
        '/* User shall fill in the function bodies. */',
        '',
    ]
    for spg in sorted(spgs):
        lines += [
            f'void {spg}(integer *data_in, integer *data_out)',
            '{',
            f'    /* TODO: implement {spg} */',
            '    if (data_out) *data_out = (data_in ? *data_in : 0);',
            '}',
            '',
        ]
    return '\n'.join(lines) + '\n'


def gen_main_c(pr: PartitionResources) -> str:
    pf = pr.prefix
    lines = [
        '#include <os/pos/apex/apexLib.h>',
        '#include <stdlib.h>',
        '#include <stdio.h>',
        '#include <string.h>',
        '#include "activity.h"',
        '#include "gtypes.h"',
        '#include "deployment.h"',
        '#include "globals.h"',
        '#include "subprograms.h"',
        '',
        _banner(pr),
        '',
        f'PROCESS_ID_TYPE arinc_threads[ACoreOS653_CONFIG_NB_THREADS];',
        '',
        '/* ---- Inter-partition ports ---- */',
    ]

    # Declare port IDs
    for s in pr.sampling:
        lines.append(f'SAMPLING_PORT_ID_TYPE {s.var_name};')
    for q in pr.queuing:
        lines.append(f'QUEUING_PORT_ID_TYPE  {q.var_name};')

    if pr.blackboards:
        lines.append('')
        lines.append('/* ---- Intra-partition blackboards ---- */')
        for bb in pr.blackboards:
            lines.append(f'BLACKBOARD_ID_TYPE {bb.var_name};'
                         f'  /* {bb.writer_task} -> {bb.reader_task} */')

    if pr.buffers:
        lines.append('')
        lines.append('/* ---- Intra-partition buffers ---- */')
        for buf in pr.buffers:
            lines.append(f'BUFFER_ID_TYPE {buf.var_name};'
                         f'  /* {buf.writer_task} -> {buf.reader_task} */')

    lines += ['', 'void appMain(void)', '{',
              '    PROCESS_ATTRIBUTE_TYPE tattr;',
              '    RETURN_CODE_TYPE ret = NO_ERROR;', '']

    step = 1

    # Create sampling ports
    for s in pr.sampling:
        lines += [
            f'    /* {step}. Sampling port: {s.port_name} ({s.direction}) */',
            f'    CREATE_SAMPLING_PORT(',
            f'        "{s.port_name}",',
            f'        sizeof(integer),',
            f'        {s.direction},',
            f'        {s.refresh_ns}ll,',
            f'        &({s.var_name}), &ret);',
            f'    CHECK_CODE("CREATE_SAMPLING_PORT({s.port_name})", ret);',
            '',
        ]
        step += 1

    # Create blackboards
    for bb in pr.blackboards:
        lines += [
            f'    /* {step}. Blackboard: {bb.bb_name}'
            f'  ({bb.writer_task} -> {bb.reader_task}) */',
            f'    CREATE_BLACKBOARD(',
            f'        "{bb.bb_name}",',
            f'        sizeof(integer),',
            f'        &({bb.var_name}), &ret);',
            f'    CHECK_CODE("CREATE_BLACKBOARD({bb.bb_name})", ret);',
            '',
        ]
        step += 1

    # Create buffers
    for buf in pr.buffers:
        lines += [
            f'    /* {step}. Buffer: {buf.buf_name}'
            f'  ({buf.writer_task} -> {buf.reader_task}) */',
            f'    CREATE_BUFFER(',
            f'        "{buf.buf_name}",',
            f'        sizeof(integer),',
            f'        {buf.max_msgs},',
            f'        FIFO,',
            f'        &({buf.var_name}), &ret);',
            f'    CHECK_CODE("CREATE_BUFFER({buf.buf_name})", ret);',
            '',
        ]
        step += 1

    # Create tasks (sorted by priority)
    for tr in pr.tasks:
        td = tr.thread
        dl = td.deadline_type if td.deadline_type in ('SOFT', 'HARD') else 'SOFT'
        lines += [
            f'    /* {step}. Task: {tr.inst_name} (AADL: {tr.type_name})'
            f'  Period={td.period_ns//1_000_000}ms Priority={td.priority} */',
            f'    strcpy(tattr.NAME, "{tr.inst_name}");',
            f'    tattr.ENTRY_POINT   = {tr.inst_name}_job;',
            f'    tattr.BASE_PRIORITY = {td.priority};',
            f'    tattr.PERIOD        = {td.period_ns}ll;',
            f'    tattr.STACK_SIZE    = {td.stack_size};',
            f'    tattr.TIME_CAPACITY = {td.time_capacity_ns}ll;',
            f'    tattr.DEADLINE      = {dl};',
            f'    CREATE_PROCESS(&(tattr), &(arinc_threads[{tr.index}]), &ret);',
            f'    CHECK_CODE("CREATE_PROCESS({tr.inst_name})", ret);',
            f'    START(arinc_threads[{tr.index}], &ret);',
            f'    CHECK_CODE("START({tr.inst_name})", ret);',
            '',
        ]
        step += 1

    # Create queuing ports (after tasks, following IMA2C convention)
    for q in pr.queuing:
        lines += [
            f'    /* {step}. Queuing port: {q.port_name} ({q.direction}) */',
            f'    CREATE_QUEUING_PORT(',
            f'        "{q.port_name}",',
            f'        sizeof(integer),',
            f'        {q.max_msgs},',
            f'        {q.direction},',
            f'        {q.disc},',
            f'        &({q.var_name}), &ret);',
            f'    CHECK_CODE("CREATE_QUEUING_PORT({q.port_name})", ret);',
            '',
        ]
        step += 1

    lines += [
        f'    /* {step}. Switch to NORMAL operating mode */',
        '    SET_PARTITION_MODE(NORMAL, &ret);',
        '    CHECK_CODE("SET_PARTITION_MODE(NORMAL)", ret);',
        '    return;',
        '}',
    ]
    return '\n'.join(lines) + '\n'


def gen_activity_c(pr: PartitionResources) -> str:
    """Generate activity.c with one task body per periodic task."""
    pf = pr.prefix

    # Build lookup: task-inst → set of resource variable names it accesses
    # We need to emit extern declarations per task.

    # Collect all declared IDs by var_name
    all_ids: Dict[str, str] = {}   # var_name → C type
    for s  in pr.sampling:    all_ids[s.var_name]   = 'SAMPLING_PORT_ID_TYPE'
    for q  in pr.queuing:     all_ids[q.var_name]   = 'QUEUING_PORT_ID_TYPE'
    for bb in pr.blackboards: all_ids[bb.var_name]  = 'BLACKBOARD_ID_TYPE'
    for buf in pr.buffers:    all_ids[buf.var_name] = 'BUFFER_ID_TYPE'

    lines = [
        '#include <os/pos/apex/apexLib.h>',
        '#include <stdio.h>',
        '#include <stdlib.h>',
        '#include <string.h>',
        '#include "activity.h"',
        '#include "gtypes.h"',
        '#include "deployment.h"',
        '#include "globals.h"',
        '#include "subprograms.h"',
        '',
        _banner(pr),
        '',
        '#define SZ 1024',
        '',
    ]

    for tr in pr.tasks:
        td   = tr.thread
        inst = tr.inst_name

        # Determine which resources this task accesses
        writes_bb  = [bb  for bb  in pr.blackboards if bb.writer_task == inst]
        reads_bb   = [bb  for bb  in pr.blackboards if bb.reader_task == inst]
        writes_buf = [buf for buf in pr.buffers     if buf.writer_task == inst]
        reads_buf  = [buf for buf in pr.buffers     if buf.reader_task == inst]

        # For inter-partition ports, use connection routing if available;
        # otherwise fall back to direction-based assignment (all tasks share).
        task_reads  = pr.task_interpart_reads.get(inst)   # set or None
        task_writes = pr.task_interpart_writes.get(inst)  # set or None
        has_routing = bool(pr.task_interpart_reads or pr.task_interpart_writes)

        if has_routing:
            reads_sam  = [s for s in pr.sampling
                          if s.direction == 'DESTINATION'
                          and (task_reads is None or s.port_name in task_reads)]
            writes_sam = [s for s in pr.sampling
                          if s.direction == 'SOURCE'
                          and (task_writes is None or s.port_name in task_writes)]
            reads_que  = [q for q in pr.queuing
                          if q.direction == 'DESTINATION'
                          and task_reads is not None and q.port_name in task_reads]
            writes_que = [q for q in pr.queuing
                          if q.direction == 'SOURCE'
                          and task_writes is not None and q.port_name in task_writes]
        else:
            writes_sam = [s for s in pr.sampling if s.direction == 'SOURCE']
            reads_sam  = [s for s in pr.sampling if s.direction == 'DESTINATION']
            writes_que = [q for q in pr.queuing  if q.direction == 'SOURCE']
            reads_que  = [q for q in pr.queuing  if q.direction == 'DESTINATION']

        # Collect extern declarations needed
        externs_needed = set()
        for r in writes_bb + reads_bb:   externs_needed.add(r.var_name)
        for r in writes_buf + reads_buf: externs_needed.add(r.var_name)
        for r in writes_sam + reads_sam: externs_needed.add(r.var_name)
        for r in writes_que + reads_que: externs_needed.add(r.var_name)

        lines.append(f'/* {"="*66} */')
        lines.append(f'/*  {inst}  (AADL: {td.name})')
        lines.append(f' *  Period={td.period_ns//1_000_000}ms  '
                     f'Priority={td.priority}  '
                     f'Dispatch_Abs={td.dispatch_abs_ns//1_000_000}ms')
        if writes_sam: lines.append(f' *  Inter-partition WRITES: '
                                    f'{", ".join(s.port_name for s in writes_sam)} (SAMPLING SOURCE)')
        if reads_sam:  lines.append(f' *  Inter-partition READS:  '
                                    f'{", ".join(s.port_name for s in reads_sam)} (SAMPLING DEST)')
        if writes_que: lines.append(f' *  Inter-partition WRITES: '
                                    f'{", ".join(q.port_name for q in writes_que)} (QUEUING SOURCE)')
        if reads_que:  lines.append(f' *  Inter-partition READS:  '
                                    f'{", ".join(q.port_name for q in reads_que)} (QUEUING DEST)')
        if writes_bb:  lines.append(f' *  Intra-partition WRITES (BB):  '
                                    f'{", ".join(b.bb_name for b in writes_bb)}')
        if reads_bb:   lines.append(f' *  Intra-partition READS  (BB):  '
                                    f'{", ".join(b.bb_name for b in reads_bb)}')
        if writes_buf: lines.append(f' *  Intra-partition WRITES (BUF): '
                                    f'{", ".join(b.buf_name for b in writes_buf)}')
        if reads_buf:  lines.append(f' *  Intra-partition READS  (BUF): '
                                    f'{", ".join(b.buf_name for b in reads_buf)}')
        lines.append(f' * {"="*66} */')

        # extern declarations
        for vname in sorted(externs_needed):
            ctype = all_ids.get(vname, 'UNKNOWN_TYPE')
            lines.append(f'extern {ctype} {vname};')
        lines.append('')

        # Function body
        lines.append(f'void* {inst}_job(void)')
        lines.append('{')
        lines.append('    RETURN_CODE_TYPE ret = NO_ERROR;')
        if reads_sam:
            lines.append('    VALIDITY_TYPE    validity;')
        lines.append('    MESSAGE_SIZE_TYPE msg_len;')
        lines.append('    integer val = 0;')
        lines.append('')
        lines.append('    while (1)')
        lines.append('    {')

        # READS first
        for s in reads_sam:
            lines += [
                f'        /* READ inter-partition SAMPLING: {s.port_name} */',
                f'        READ_SAMPLING_MESSAGE({s.var_name},',
                f'            (MESSAGE_ADDR_TYPE)&val, &msg_len, &validity, &ret);',
                f'        CHECK_VALIDCODE("READ_SAMPLING_MESSAGE({s.port_name})", validity, ret);',
                '',
            ]
        for q in reads_que:
            lines += [
                f'        /* RECEIVE inter-partition QUEUING: {q.port_name} */',
                f'        RECEIVE_QUEUING_MESSAGE({q.var_name},',
                f'            {q.timeout_ns}ll,',
                f'            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);',
                f'        CHECK_CODE("RECEIVE_QUEUING_MESSAGE({q.port_name})", ret);',
                '',
            ]
        for bb in reads_bb:
            lines += [
                f'        /* READ intra-partition BLACKBOARD: {bb.bb_name} */',
                f'        READ_BLACKBOARD({bb.var_name}, 0ll,',
                f'            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);',
                f'        CHECK_CODE("READ_BLACKBOARD({bb.bb_name})", ret);',
                '',
            ]
        for buf in reads_buf:
            lines += [
                f'        /* READ intra-partition BUFFER: {buf.buf_name} */',
                f'        READ_BUFFER({buf.var_name}, {buf.timeout_ns}ll,',
                f'            (MESSAGE_ADDR_TYPE)&val, &msg_len, &ret);',
                f'        CHECK_CODE("READ_BUFFER({buf.buf_name})", ret);',
                '',
            ]

        # Subprogram calls
        for spg in td.calls:
            lines.append(f'        {spg}(&val, &val);  /* user subprogram */')
        if td.calls:
            lines.append('')

        # WRITES
        for s in writes_sam:
            lines += [
                f'        /* WRITE inter-partition SAMPLING: {s.port_name} */',
                f'        WRITE_SAMPLING_MESSAGE({s.var_name},',
                f'            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), &ret);',
                f'        CHECK_CODE("WRITE_SAMPLING_MESSAGE({s.port_name})", ret);',
                '',
            ]
        for q in writes_que:
            lines += [
                f'        /* SEND inter-partition QUEUING: {q.port_name} */',
                f'        SEND_QUEUING_MESSAGE({q.var_name},',
                f'            (MESSAGE_ADDR_TYPE)&val, sizeof(integer),',
                f'            {q.timeout_ns}ll, &ret);',
                f'        CHECK_CODE("SEND_QUEUING_MESSAGE({q.port_name})", ret);',
                '',
            ]
        for bb in writes_bb:
            lines += [
                f'        /* DISPLAY intra-partition BLACKBOARD: {bb.bb_name} */',
                f'        DISPLAY_BLACKBOARD({bb.var_name},',
                f'            (MESSAGE_ADDR_TYPE)&val, sizeof(integer), &ret);',
                f'        CHECK_CODE("DISPLAY_BLACKBOARD({bb.bb_name})", ret);',
                '',
            ]
        for buf in writes_buf:
            lines += [
                f'        /* WRITE intra-partition BUFFER: {buf.buf_name} */',
                f'        WRITE_BUFFER({buf.var_name},',
                f'            (MESSAGE_ADDR_TYPE)&val, sizeof(integer),',
                f'            {buf.timeout_ns}ll, &ret);',
                f'        CHECK_CODE("WRITE_BUFFER({buf.buf_name})", ret);',
                '',
            ]

        lines += [
            '        PERIODIC_WAIT(&ret);',
            f'        CHECK_CODE("PERIODIC_WAIT({inst})", ret);',
            '    }',
            '}',
            '',
        ]

    return '\n'.join(lines) + '\n'


# ─────────────────────────────────────────────────────────────────────────────
#  File writer
# ─────────────────────────────────────────────────────────────────────────────

def write_partition(pr: PartitionResources, outdir: str):
    """Write all C files for one partition into outdir/<prefix>/."""
    dest = os.path.join(outdir, pr.prefix)
    os.makedirs(dest, exist_ok=True)

    files = {
        'deployment.h':  gen_deployment_h(pr),
        'globals.h':     gen_globals_h(pr),
        'globals.c':     '#include "globals.h"\n',
        'gtypes.h':      gen_gtypes_h(pr),
        'gtypes.c':      '#include "gtypes.h"\n',
        'subprograms.h': gen_subprograms_h(pr),
        'subprograms.c': gen_subprograms_c(pr),
        'activity.h':    gen_activity_h(pr),
        'activity.c':    gen_activity_c(pr),
        'main.c':        gen_main_c(pr),
        'deployment.c':  '#include "deployment.h"\n',
    }

    for fname, content in files.items():
        fpath = os.path.join(dest, fname)
        with open(fpath, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f'  [WRITE] {fpath}')


# ─────────────────────────────────────────────────────────────────────────────
#  Spec JSON builder
# ─────────────────────────────────────────────────────────────────────────────

def build_spec_json(res, module: str, part_name: str, aadl_process: str) -> dict:
    """Serialize PartitionResources to the specs/psX.json format."""
    STACK_PER_TASK = 8192

    tasks = []
    for t in res.tasks:
        tasks.append({
            "name": t.inst_name,
            "period_ms": int(t.thread.period_ns // 1_000_000),
            "priority": t.thread.priority,
        })

    sampling_ports = []
    for s in res.sampling:
        entry = {
            "name": s.port_name,
            "direction": s.direction,
            "refresh_period_ns": s.refresh_ns,
        }
        if s.direction == "DESTINATION":
            entry["api_read"] = "READ_SAMPLING_MESSAGE"
        else:
            entry["api_write"] = "WRITE_SAMPLING_MESSAGE"
        sampling_ports.append(entry)

    queuing_ports = []
    for q in res.queuing:
        entry = {
            "name": q.port_name,
            "direction": q.direction,
            "max_nb": q.max_msgs,
        }
        if q.direction == "DESTINATION":
            entry["api_read"] = "RECEIVE_QUEUING_MESSAGE"
        else:
            entry["api_write"] = "SEND_QUEUING_MESSAGE"
        queuing_ports.append(entry)

    blackboards = [{"name": b.bb_name} for b in res.blackboards]
    buffers = [{"name": b.buf_name, "max_nb": b.max_msgs} for b in res.buffers]

    subprograms = []
    seen: set = set()
    for t in res.tasks:
        for sp in (t.thread.calls or []):
            if sp not in seen:
                subprograms.append(sp)
                seen.add(sp)

    return {
        "partition": part_name,
        "module": module,
        "aadl_process": aadl_process,
        "required_files": [
            "activity.c", "activity.h",
            "deployment.c", "deployment.h",
            "globals.c",   "globals.h",
            "gtypes.c",    "gtypes.h",
            "main.c",
            "subprograms.c", "subprograms.h",
        ],
        "tasks":          tasks,
        "sampling_ports": sampling_ports,
        "queuing_ports":  queuing_ports,
        "blackboards":    blackboards,
        "buffers":        buffers,
        "subprograms":    subprograms,
        "deployment": {
            "nb_threads":     len(tasks),
            "nb_samplings":   len(sampling_ports),
            "nb_queueings":   len(queuing_ports),
            "nb_blackboards": len(blackboards),
            "nb_buffers":     len(buffers),
            "stacks_size":    len(tasks) * STACK_PER_TASK,
        },
    }


# ─────────────────────────────────────────────────────────────────────────────
#  CLI entry point
# ─────────────────────────────────────────────────────────────────────────────

def main():
    ap = argparse.ArgumentParser(
        description='Generate ARINC 653 C skeleton code from AADL IMA model.')
    ap.add_argument('--partitions', required=True,
                    help='Path to AADL file containing process declarations (e.g. DIMA_partitions.aadl)')
    ap.add_argument('--threads',    required=True,
                    help='Path to AADL file containing thread declarations (e.g. DIMA_threads.aadl)')
    ap.add_argument('--outdir',     default='./generated',
                    help='Output root directory (default: ./generated)')
    ap.add_argument('--module',     default='',
                    help='Module name prefix for filtering (optional, e.g. M1)')
    ap.add_argument('--partlist',   nargs='*',
                    help='Generate only these partition names, e.g. --partlist P1 P2')
    ap.add_argument('--spec-json', action='store_true',
                    help='Also write a specs JSON file alongside the generated C code')
    ap.add_argument('--package', default='',
                    help='AADL package name for aadl_process field in spec JSON (e.g. DIMA2_partitions)')
    args = ap.parse_args()

    parser = AadlParser()
    print(f'[INFO] Parsing threads from {args.threads}')
    parser.parse_file(args.threads)
    print(f'[INFO] Parsing partitions from {args.partitions}')
    parser.parse_file(args.partitions)

    print(f'[INFO] Found {len(parser.threads)} thread type(s): '
          f'{list(parser.threads.keys())}')
    print(f'[INFO] Found {len(parser.partitions)} partition type(s): '
          f'{list(parser.partitions.keys())}')

    # Filter partitions if requested
    selected = set(args.partlist) if args.partlist else set(parser.partitions.keys())

    for pname, pd in parser.partitions.items():
        if pname not in selected:
            continue

        # Derive a short prefix from the partition name (e.g. P1 → ps1, PA → pa)
        nums = re.sub(r'[^0-9]', '', pname)
        prefix = ('ps' + nums) if nums else pname.lower()
        if args.module:
            prefix = prefix  # could prefix with module if needed

        print(f'\n[INFO] Generating partition {pname} → {prefix}/')
        pr = analyse_partition(pd, parser.threads, prefix)
        print(f'       Tasks:       {[t.inst_name for t in pr.tasks]}')
        print(f'       Sampling:    {[s.port_name for s in pr.sampling]}')
        print(f'       Queuing:     {[q.port_name for q in pr.queuing]}')
        print(f'       Blackboards: {[b.bb_name   for b in pr.blackboards]}')
        print(f'       Buffers:     {[b.buf_name  for b in pr.buffers]}')
        write_partition(pr, args.outdir)

        if args.spec_json:
            pkg = args.package or 'DIMA_partitions'
            aadl_proc = f"{pkg}::{pname}.impl"
            spec = build_spec_json(pr, args.module or "M?", prefix, aadl_proc)
            spec_path = os.path.join(args.outdir, prefix, f"{prefix}.json")
            with open(spec_path, 'w', encoding='utf-8') as f:
                json.dump(spec, f, indent=2, ensure_ascii=False)
            print(f'  [SPEC] {spec_path}')

    print('\n[DONE] Code generation complete.')


if __name__ == '__main__':
    main()
