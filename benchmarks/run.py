#!/usr/bin/env python3

import argparse
import concurrent.futures
import datetime
import os
import re
import shlex
import signal
import subprocess
import sys
import threading
import time
from pathlib import Path

from gen import LIBRARIES, TEST_CASES, generate_sources


MAGIC_ENUM_VERSION = "v0.9.7"
ENCHANTUM_VERSION = "0.5.0"
RSS_PATTERN = re.compile(r"BENCHMARK_MAX_RSS_KB=(\d+)")
MEMORY_LIMIT_PERCENT = 95
GCC_TOTAL_PATTERN = re.compile(r"^\s*TOTAL\s*:\s*(.*)$", re.MULTILINE)
GCC_TIME_PATTERN = re.compile(
    r"([0-9]+(?:\.[0-9]+)?)(?:\s*\(\s*[0-9]+%\s*\))?"
)
_ACTIVE_PROCESSES = set()
_PROCESS_LOCK = threading.Lock()
_CANCEL_REQUESTED = threading.Event()


def _signal_process(process, signal_number):
    if process.poll() is not None:
        return
    try:
        if os.name == "posix":
            os.killpg(process.pid, signal_number)
        elif signal_number == signal.SIGTERM:
            process.terminate()
        else:
            process.kill()
    except ProcessLookupError:
        pass


def _kill_process(process):
    if process.poll() is None:
        if os.name == "posix":
            _signal_process(process, signal.SIGKILL)
        else:
            process.kill()


def terminate_active_processes(grace_seconds=5):
    with _PROCESS_LOCK:
        processes = list(_ACTIVE_PROCESSES)
    for process in processes:
        _signal_process(process, signal.SIGTERM)

    deadline = time.monotonic() + grace_seconds
    while time.monotonic() < deadline and any(
        process.poll() is None for process in processes
    ):
        time.sleep(0.05)

    for process in processes:
        _kill_process(process)
    for process in processes:
        process.wait()


def find_gnu_time():
    for candidate in ("/usr/bin/time", "/bin/time"):
        if not Path(candidate).is_file():
            continue
        result = subprocess.run(
            [candidate, "--version"], capture_output=True, text=True, check=False
        )
        if "GNU Time" in result.stdout or "GNU Time" in result.stderr:
            return candidate
    return None


def available_memory_bytes():
    if sys.platform == "linux":
        system_available = None
        for line in Path("/proc/meminfo").read_text(encoding="utf-8").splitlines():
            if line.startswith("MemAvailable:"):
                system_available = int(line.split()[1]) * 1024
                break
        if system_available is not None:
            cgroup_available = None
            cgroup_v2_max = Path("/sys/fs/cgroup/memory.max")
            cgroup_v2_current = Path("/sys/fs/cgroup/memory.current")
            if cgroup_v2_max.is_file() and cgroup_v2_current.is_file():
                maximum = cgroup_v2_max.read_text(encoding="utf-8").strip()
                if maximum != "max":
                    cgroup_available = max(
                        0,
                        int(maximum)
                        - int(cgroup_v2_current.read_text(encoding="utf-8").strip()),
                    )
            else:
                cgroup_v1_limit = Path(
                    "/sys/fs/cgroup/memory/memory.limit_in_bytes"
                )
                cgroup_v1_usage = Path(
                    "/sys/fs/cgroup/memory/memory.usage_in_bytes"
                )
                if cgroup_v1_limit.is_file() and cgroup_v1_usage.is_file():
                    limit = int(cgroup_v1_limit.read_text(encoding="utf-8").strip())
                    usage = int(cgroup_v1_usage.read_text(encoding="utf-8").strip())
                    if limit < (1 << 60):
                        cgroup_available = max(0, limit - usage)
            return (
                min(system_available, cgroup_available)
                if cgroup_available is not None
                else system_available
            )
    if os.name == "nt":
        import ctypes
        from ctypes import wintypes

        class MemoryStatusEx(ctypes.Structure):
            _fields_ = [
                ("dwLength", wintypes.DWORD),
                ("dwMemoryLoad", wintypes.DWORD),
                ("ullTotalPhys", ctypes.c_ulonglong),
                ("ullAvailPhys", ctypes.c_ulonglong),
                ("ullTotalPageFile", ctypes.c_ulonglong),
                ("ullAvailPageFile", ctypes.c_ulonglong),
                ("ullTotalVirtual", ctypes.c_ulonglong),
                ("ullAvailVirtual", ctypes.c_ulonglong),
                ("ullAvailExtendedVirtual", ctypes.c_ulonglong),
            ]

        status = MemoryStatusEx()
        status.dwLength = ctypes.sizeof(status)
        kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
        kernel32.GlobalMemoryStatusEx.argtypes = [ctypes.POINTER(MemoryStatusEx)]
        kernel32.GlobalMemoryStatusEx.restype = wintypes.BOOL
        if kernel32.GlobalMemoryStatusEx(ctypes.byref(status)):
            return status.ullAvailPhys
    try:
        pages = os.sysconf("SC_AVPHYS_PAGES")
        page_size = os.sysconf("SC_PAGE_SIZE")
    except (AttributeError, OSError, ValueError):
        pages = page_size = -1
    if pages > 0 and page_size > 0:
        return pages * page_size
    raise OSError("Could not determine available system memory.")


def process_group_rss_bytes(process_group_id):
    if sys.platform == "linux":
        total_bytes = 0
        for process_dir in Path("/proc").iterdir():
            if not process_dir.name.isdigit():
                continue
            try:
                stat = (process_dir / "stat").read_text(encoding="utf-8")
                fields = stat[stat.rfind(")") + 2 :].split()
                if int(fields[2]) != process_group_id:
                    continue
                for line in (process_dir / "status").read_text(
                    encoding="utf-8"
                ).splitlines():
                    if line.startswith("VmRSS:"):
                        total_bytes += int(line.split()[1]) * 1024
                        break
            except (FileNotFoundError, ProcessLookupError, PermissionError, ValueError):
                continue
        return total_bytes
    if os.name == "nt":
        import ctypes
        from ctypes import wintypes

        class ProcessMemoryCountersEx(ctypes.Structure):
            _fields_ = [
                ("cb", wintypes.DWORD),
                ("PageFaultCount", wintypes.DWORD),
                ("PeakWorkingSetSize", ctypes.c_size_t),
                ("WorkingSetSize", ctypes.c_size_t),
                ("QuotaPeakPagedPoolUsage", ctypes.c_size_t),
                ("QuotaPagedPoolUsage", ctypes.c_size_t),
                ("QuotaPeakNonPagedPoolUsage", ctypes.c_size_t),
                ("QuotaNonPagedPoolUsage", ctypes.c_size_t),
                ("PagefileUsage", ctypes.c_size_t),
                ("PeakPagefileUsage", ctypes.c_size_t),
                ("PrivateUsage", ctypes.c_size_t),
            ]

        kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
        kernel32.OpenProcess.argtypes = [
            wintypes.DWORD,
            wintypes.BOOL,
            wintypes.DWORD,
        ]
        kernel32.OpenProcess.restype = wintypes.HANDLE
        kernel32.CloseHandle.argtypes = [wintypes.HANDLE]
        psapi = ctypes.WinDLL("psapi", use_last_error=True)
        psapi.GetProcessMemoryInfo.argtypes = [
            wintypes.HANDLE,
            ctypes.POINTER(ProcessMemoryCountersEx),
            wintypes.DWORD,
        ]
        psapi.GetProcessMemoryInfo.restype = wintypes.BOOL
        handle = kernel32.OpenProcess(0x1000 | 0x0400, False, process_group_id)
        if not handle:
            return 0
        try:
            counters = ProcessMemoryCountersEx()
            counters.cb = ctypes.sizeof(counters)
            if not psapi.GetProcessMemoryInfo(
                handle, ctypes.byref(counters), counters.cb
            ):
                return 0
            return counters.WorkingSetSize
        finally:
            kernel32.CloseHandle(handle)
    raise OSError("Process memory monitoring is supported on Linux and Windows.")


def run_logged(
    command,
    log_path,
    cwd,
    memory_tool=None,
    timeout_seconds=None,
    memory_limit_bytes=None,
):
    log_path.parent.mkdir(parents=True, exist_ok=True)
    timed_command = (
        [memory_tool, "-f", "BENCHMARK_MAX_RSS_KB=%M", *command]
        if memory_tool
        else command
    )
    with log_path.open("w", encoding="utf-8") as log:
        log.write(f"$ {shlex.join(command)}\n\n")
        log.flush()
        with _PROCESS_LOCK:
            if _CANCEL_REQUESTED.is_set():
                return 130, None, "", False
            process = subprocess.Popen(
                timed_command,
                cwd=cwd,
                stdout=log,
                stderr=subprocess.STDOUT,
                text=True,
                start_new_session=(os.name == "posix"),
            )
            _ACTIVE_PROCESSES.add(process)
        timed_out = False
        memory_exceeded = False
        peak_group_rss_bytes = 0
        started = time.monotonic()
        try:
            while True:
                try:
                    return_code = process.wait(timeout=0.1)
                    break
                except subprocess.TimeoutExpired:
                    if memory_limit_bytes is not None:
                        current_rss_bytes = process_group_rss_bytes(process.pid)
                        peak_group_rss_bytes = max(
                            peak_group_rss_bytes, current_rss_bytes
                        )
                        if current_rss_bytes > memory_limit_bytes:
                            memory_exceeded = True
                            _signal_process(process, signal.SIGTERM)
                            try:
                                process.wait(timeout=5)
                            except subprocess.TimeoutExpired:
                                _kill_process(process)
                                process.wait()
                            return_code = 125
                            break
                    if (
                        timeout_seconds is not None
                        and time.monotonic() - started >= timeout_seconds
                    ):
                        timed_out = True
                        _signal_process(process, signal.SIGTERM)
                        try:
                            process.wait(timeout=5)
                        except subprocess.TimeoutExpired:
                            _kill_process(process)
                            process.wait()
                        return_code = 124
                        break
        finally:
            with _PROCESS_LOCK:
                _ACTIVE_PROCESSES.discard(process)

    output = log_path.read_text(encoding="utf-8", errors="replace")
    rss_match = RSS_PATTERN.search(output)
    peak_rss_bytes = peak_group_rss_bytes
    if rss_match:
        peak_rss_bytes = max(peak_rss_bytes, int(rss_match.group(1)) * 1024)
    peak_rss_mib = peak_rss_bytes / (1024 * 1024) if peak_rss_bytes else None
    return return_code, peak_rss_mib, output, timed_out, memory_exceeded


def ensure_dependency(name, version, url, dependency_dir):
    checkout = dependency_dir / f"{name}-{version}"
    expected_header = (
        checkout / "include" / "magic_enum" / "magic_enum.hpp"
        if name == "magic_enum"
        else checkout / "include" / "enchantum" / "enchantum.hpp"
    )
    if expected_header.is_file():
        return checkout / "include"

    checkout.parent.mkdir(parents=True, exist_ok=True)
    command = [
        "git",
        "clone",
        "--depth",
        "1",
        "--branch",
        version,
        url,
        str(checkout),
    ]
    result = subprocess.run(command, capture_output=True, text=True, check=False)
    if result.returncode:
        raise RuntimeError(
            f"Could not fetch {name} {version}:\n"
            f"{result.stdout}{result.stderr}"
        )
    if not expected_header.is_file():
        raise RuntimeError(f"Expected {name} header is missing: {expected_header}")
    return checkout / "include"


CLANG_TIME_VALUE_PATTERN = re.compile(r"([0-9]+(?:\.[0-9]+)?)\s+\([^)]+\)")
CLANG_TIME_REPORT_PATTERN = re.compile(
    r"^\s*Clang(?: front-end)? time report\s*$", re.MULTILINE
)


def clang_time_report(output):
    headings = list(CLANG_TIME_REPORT_PATTERN.finditer(output))
    if not headings:
        raise ValueError("Clang -ftime-report summary was not found.")

    frontend_headings = [
        heading
        for heading in headings
        if "front-end" in heading.group(0).lower()
    ]
    heading = (frontend_headings or headings)[-1]
    section = output[heading.start() :]
    table_header = re.search(
        r"^\s*---User Time---.*--- Name ---\s*$", section, re.MULTILINE
    )
    if not table_header:
        raise ValueError("Clang -ftime-report table header was not found.")
    wall_index = 3 if "--System Time--" in table_header.group(0) else 2

    report_lines = section[: table_header.start()].rstrip().splitlines()
    report_lines.extend(section[table_header.start() :].splitlines()[:1])
    rows = []
    for line in section[table_header.end() :].splitlines():
        values = CLANG_TIME_VALUE_PATTERN.finditer(line)
        matches = list(values)
        if not matches:
            if rows:
                break
            continue
        if len(matches) <= wall_index:
            continue
        name = line[matches[-1].end() :].strip()
        if not name:
            continue
        row = {
            "wall": float(matches[wall_index].group(1)),
            "name": name,
        }
        rows.append(row)
        report_lines.append(line)
        if row["name"] == "Total":
            break

    total = next((row for row in rows if row["name"] == "Total"), None)
    if total is None:
        raise ValueError("Clang -ftime-report total row was not recognized.")
    frontend = next(
        (
            row
            for row in rows
            if "front end" in row["name"].lower().replace("-", " ")
        ),
        None,
    )
    hotspots = sorted(
        (row for row in rows if row["name"] != "Total"),
        key=lambda row: row["wall"],
        reverse=True,
    )
    return {
        "compiler_seconds": total["wall"],
        "frontend_seconds": frontend["wall"] if frontend else None,
        "hotspots": [(row["name"], row["wall"]) for row in hotspots[:3]],
        "raw_compiler_data": "\n".join(report_lines).strip(),
        "raw_compiler_format": "text",
    }


def gcc_time_metrics(output):
    header = re.search(r"^\s*Time variable\s+(.*)$", output, re.MULTILINE)
    if not header:
        raise ValueError("GCC -ftime-report header was not found.")
    columns = header.group(1).split()
    if "wall" not in columns:
        raise ValueError("GCC -ftime-report has no wall-time column.")
    wall_index = columns.index("wall")

    total_match = GCC_TOTAL_PATTERN.search(output)
    if not total_match:
        raise ValueError("GCC -ftime-report TOTAL row was not found.")
    total_times = GCC_TIME_PATTERN.findall(total_match.group(1))
    if wall_index >= len(total_times):
        raise ValueError("GCC -ftime-report TOTAL time was not recognized.")

    hotspots = []
    for line in output.splitlines():
        phase = re.match(r"^\s*([^:]+?)\s*:\s*(.*)$", line)
        if not phase or phase.group(1).strip() == "TOTAL":
            continue
        times = GCC_TIME_PATTERN.findall(phase.group(2))
        if wall_index < len(times):
            hotspots.append((phase.group(1).strip(), float(times[wall_index])))
    hotspots.sort(key=lambda item: item[1], reverse=True)

    return {
        "compiler_seconds": float(total_times[wall_index]),
        "frontend_seconds": None,
        "hotspots": hotspots[:3],
    }


def gcc_raw_report(output):
    header = re.search(r"^\s*Time variable\s+.*$", output, re.MULTILINE)
    if not header:
        raise ValueError("GCC -ftime-report header was not found.")
    total = GCC_TOTAL_PATTERN.search(output, header.end())
    if not total:
        raise ValueError("GCC -ftime-report TOTAL row was not found.")
    return output[header.start() : total.end()].strip()


def compiler_metrics(family, output):
    if family == "clang":
        return clang_time_report(output)
    metrics = gcc_time_metrics(output)
    metrics["raw_compiler_data"] = gcc_raw_report(output)
    metrics["raw_compiler_format"] = "text"
    return metrics


def compile_target(
    target,
    compiler,
    family,
    standard,
    jobs,
    include_dirs,
    output_dir,
    log_dir,
    timeout,
    memory_tool,
):
    source = target["source"]
    stem = source.stem
    object_path = output_dir / f"{stem}.o"
    object_path.unlink(missing_ok=True)
    command = [
        *compiler,
        f"-std=c++{standard}",
        "-O3",
        "-c",
        str(source),
        "-o",
        str(object_path),
    ]
    for include_dir in include_dirs:
        command.extend(["-I", str(include_dir)])
    if family == "clang":
        command.append("-fconstexpr-steps=10000000")
    command.append("-ftime-report")
    command.append("-DENCHANTUM_CHECK_OUT_OF_BOUNDS_BY")

    log_path = log_dir / f"{stem}.log"
    available_memory = available_memory_bytes()
    memory_limit = available_memory * MEMORY_LIMIT_PERCENT // (100 * jobs)
    code, peak_rss_mib, output, timed_out, memory_exceeded = run_logged(
        command,
        log_path,
        Path.cwd(),
        memory_tool,
        timeout_seconds=timeout,
        memory_limit_bytes=memory_limit,
    )
    case = target["case"]
    result = {
        "case": case["name"],
        "enum_count": target["enum_count"],
        "value_count": case["value_count"],
        "min_range": case["min_range"],
        "max_range": case["max_range"],
        "library": target["library"],
        "status": "ok",
        "peak_rss_mib": peak_rss_mib,
        "available_memory_mib": available_memory / (1024 * 1024),
        "memory_limit_mib": memory_limit / (1024 * 1024),
        "log_path": log_path,
        "hotspots": [],
    }
    if memory_exceeded:
        result["status"] = "skipped (memory limit)"
        result["error"] = (
            f"Process memory exceeded the {result['memory_limit_mib']:.1f} MiB "
            f"budget ({MEMORY_LIMIT_PERCENT}% of {result['available_memory_mib']:.1f} "
            f"MiB available divided across {jobs} parallel jobs); stopped "
            "this target and continued."
        )
    elif timed_out:
        result["status"] = "timed out"
        result["error"] = f"Compilation exceeded the {timeout}-second timeout."
    elif code:
        result["status"] = "compile failed"
        result["error"] = f"Compiler exited with status {code}."
    else:
        try:
            result.update(compiler_metrics(family, output))
        except (OSError, ValueError, KeyError) as error:
            result["status"] = "compiler timing missing"
            result["error"] = f"Could not parse compiler timing: {error}"
    return result


def format_number(value, suffix="", precision=3):
    return "n/a" if value is None else f"{value:.{precision}f}{suffix}"


def github_annotation(result):
    status = result["status"]
    command = {
        "ok": "notice",
        "skipped (memory limit)": "warning",
    }.get(status, "error")
    details = [
        f"{result['case']} / {result['library']}: {status}",
        f"compiler={format_number(result.get('compiler_seconds'), 's')}",
        f"frontend={format_number(result.get('frontend_seconds'), 's')}",
    ]
    if result.get("peak_rss_mib") is not None:
        details.append(f"peak RSS={format_number(result['peak_rss_mib'], ' MiB')}")
    message = "; ".join(details)
    message = (
        message.replace("%", "%25")
        .replace("\r", "%0D")
        .replace("\n", "%0A")
    )
    return f"::{command} title=Compile-time benchmark::{message}"


def emit_github_annotations(results):
    if os.environ.get("GITHUB_ACTIONS") != "true":
        return
    for result in results:
        print(github_annotation(result), flush=True)


def write_report(report_path, metadata, results, root):
    lines = [
        "# Compile-time benchmark comparison",
        "",
        f"- Generated: {metadata['generated']}",
        f"- Compiler: `{metadata['compiler']}` ({metadata['compiler_version']})",
        f"- C++ standard: C++{metadata['standard']}",
        f"- Optimization: `-O3`",
        f"- Timing source: `{metadata['timing_source']}`",
        f"- Build jobs: {metadata['jobs']}",
        f"- Test cases: {', '.join(metadata['cases'])}",
        "",
        "Workloads follow Enchantum's benchmark generator: each generated enum",
        "has the configured number of values, and the name-conversion function",
        "is called once per enum. Compiler time is taken from Clang or GCC",
        "`-ftime-report`; no external wall-clock build",
        "times are reported. Each active compile has a memory budget equal to",
        f"{MEMORY_LIMIT_PERCENT}% of currently available RAM divided by the",
        "configured parallel job count. A compile exceeding its budget is",
        "stopped and the next candidate runs.",
        "",
        "| Test case | Enums | Values/enum | Search range | Library | Status | Compiler time (s) | Frontend time (s) | Peak RSS (MiB) | Memory budget (MiB) | Compiler hotspots (s) |",
        "| --- | ---: | ---: | ---: | --- | --- | ---: | ---: | ---: | ---: | --- |",
    ]
    for result in results:
        hotspots = ", ".join(
            f"{name}: {duration:.3f}".replace("|", r"\|")
            for name, duration in result.get("hotspots", [])
        )
        lines.append(
            "| {case} | {enum_count} | {value_count} | {value_range} | "
            "{library} | {status} | {compiler_time} | {frontend_time} | "
            "{rss} | {memory_limit} | {hotspots} |".format(
                case=result["case"],
                enum_count=result["enum_count"],
                value_count=result["value_count"],
                value_range=f"{result['min_range']}..{result['max_range']}",
                library=result["library"],
                status=result["status"],
                compiler_time=format_number(result.get("compiler_seconds")),
                frontend_time=format_number(result.get("frontend_seconds")),
                rss=format_number(result.get("peak_rss_mib"), precision=2),
                memory_limit=format_number(
                    result.get("memory_limit_mib"), precision=2
                ),
                hotspots=hotspots or "n/a",
            )
        )

    lines.extend(["", "## Raw compiler output", ""])
    for result in results:
        lines.extend(
            [
                "<details>",
                "<summary>{case} — {library} ({status})</summary>".format(
                    case=result["case"],
                    library=result["library"],
                    status=result["status"],
                ),
                "",
            ]
        )
        raw_data = result.get("raw_compiler_data")
        raw_format = result.get("raw_compiler_format", "text")
        if raw_data is None:
            log_path = result.get("log_path")
            if log_path and log_path.is_file():
                raw_data = log_path.read_text(encoding="utf-8", errors="replace")
                raw_format = "text"
                lines.append("Compiler log (raw timing data unavailable):")
            else:
                raw_data = "No compiler report or log was produced."
                raw_format = "text"
        lines.extend(
            [
                "",
                f"```{raw_format}",
                raw_data.rstrip(),
                "```",
                "",
                "</details>",
                "",
            ]
        )

    skipped = [
        result
        for result in results
        if result["status"] == "skipped (memory limit)"
    ]
    if skipped:
        lines.extend(["", "## Skipped for memory limit", ""])
        for result in skipped:
            lines.append(
                "- {case} — {library}: {error}".format(
                    case=result["case"],
                    library=result["library"],
                    error=result["error"],
                )
            )

    failures = [
        result
        for result in results
        if result["status"] not in ("ok", "skipped (memory limit)")
    ]
    if failures:
        lines.extend(["", "## Failed targets", ""])
        for result in failures:
            log_path = result.get("log_path")
            excerpt = ""
            if log_path and log_path.is_file():
                excerpt = "\n".join(
                    log_path.read_text(encoding="utf-8", errors="replace")
                    .splitlines()[-12:]
                )
            lines.extend(
                [
                    f"### {result['case']} — {result['library']}",
                    "",
                    result.get("error", "Compilation failed."),
                    "",
                    f"Log: `{os.path.relpath(log_path, root)}`" if log_path else "",
                    "",
                    "```text",
                    excerpt,
                    "```",
                    "",
                ]
            )
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def parse_args():
    parser = argparse.ArgumentParser(
        description="Generate and compile Enchantum-style enum benchmarks."
    )
    parser.add_argument("--compiler", default=os.environ.get("CXX", "clang++"))
    parser.add_argument("--standard", type=int, default=20)
    parser.add_argument("--jobs", type=int, default=8)
    parser.add_argument("--timeout", type=int, default=1200)
    parser.add_argument("--enum-count", type=int)
    parser.add_argument(
        "--cases",
        nargs="+",
        choices=[case["name"] for case in TEST_CASES],
        default=[case["name"] for case in TEST_CASES],
    )
    parser.add_argument(
        "--build-dir",
        type=Path,
        default=Path("build/compile-time-benchmarks"),
    )
    parser.add_argument("--report", type=Path)
    return parser.parse_args()


def main():
    args = parse_args()
    if args.standard < 17:
        raise SystemExit("--standard must be 17 or later.")
    if args.jobs < 1:
        raise SystemExit("--jobs must be at least 1.")
    if args.timeout < 1:
        raise SystemExit("--timeout must be at least 1 second.")
    if args.enum_count is not None and not 1 <= args.enum_count <= 1000:
        raise SystemExit("--enum-count must be from 1 to 1000.")

    root = Path(__file__).resolve().parents[1]
    build_dir = args.build_dir.resolve()
    generated_dir = build_dir / "generated"
    log_dir = build_dir / "logs"
    report_path = (args.report or build_dir / "report.md").resolve()
    report_path.unlink(missing_ok=True)
    compiler = shlex.split(args.compiler)
    if not compiler:
        raise SystemExit("--compiler must name a compiler.")
    version_result = subprocess.run(
        [*compiler, "--version"], capture_output=True, text=True, check=False
    )
    if version_result.returncode:
        raise SystemExit(f"Could not run compiler: {args.compiler}")
    version = (version_result.stdout or version_result.stderr).splitlines()[0]
    normalized_version = version.lower()
    if "clang" in normalized_version:
        family = "clang"
        timing_source = "Clang -ftime-report"
    elif "gcc" in normalized_version or "g++" in normalized_version:
        family = "gcc"
        timing_source = "GCC -ftime-report"
    else:
        raise SystemExit("Compile-time benchmarks support GCC and Clang only.")

    memory_tool = find_gnu_time()
    if not memory_tool:
        print("GNU time not found; peak RSS metrics will be n/a.", file=sys.stderr)

    dependency_dir = build_dir / "dependencies"
    magic_enum_include = ensure_dependency(
        "magic_enum",
        MAGIC_ENUM_VERSION,
        "https://github.com/Neargye/magic_enum.git",
        dependency_dir,
    )
    enchantum_include = ensure_dependency(
        "enchantum",
        ENCHANTUM_VERSION,
        "https://github.com/ZXShady/enchantum.git",
        dependency_dir,
    )
    sources = generate_sources(generated_dir, args.cases, args.enum_count)
    include_dirs = [
        root / "include",
        root / "mgutility" / "include",
        magic_enum_include,
        enchantum_include,
    ]
    metadata = {
        "generated": datetime.datetime.now(datetime.timezone.utc).isoformat(),
        "compiler": args.compiler,
        "compiler_version": version,
        "timing_source": timing_source,
        "standard": args.standard,
        "jobs": args.jobs,
        "cases": args.cases,
    }
    results = []
    print(
        f"Compiling {len(sources)} translation units with {args.compiler}; "
        f"writing report to {report_path}"
    )
    try:
        with concurrent.futures.ThreadPoolExecutor(
            max_workers=args.jobs
        ) as executor:
            futures = [
                executor.submit(
                    compile_target,
                    target,
                    compiler,
                    family,
                    args.standard,
                    args.jobs,
                    include_dirs,
                    build_dir,
                    log_dir,
                    args.timeout,
                    memory_tool,
                )
                for target in sources
            ]
            try:
                for future in concurrent.futures.as_completed(futures):
                    result = future.result()
                    results.append(result)
                    results.sort(key=lambda item: (item["case"], item["library"]))
                    write_report(report_path, metadata, results, root)
                    print(
                        f"{result['case']} / {result['library']}: {result['status']}"
                    )
            except KeyboardInterrupt:
                _CANCEL_REQUESTED.set()
                for future in futures:
                    future.cancel()
                terminate_active_processes()
                raise
    except KeyboardInterrupt:
        if results:
            write_report(report_path, metadata, results, root)
        print("\nInterrupted; active compiler processes were stopped.", file=sys.stderr)
        return 130

    failed = sum(
        result["status"] not in ("ok", "skipped (memory limit)")
        for result in results
    )
    emit_github_annotations(results)
    print(f"Report: {report_path}")
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
