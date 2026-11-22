#!/usr/bin/env python3

import argparse
import datetime
import json
import os
import re
import signal
import subprocess
import sys
from pathlib import Path


RANGE_CHOICES = (32, 64, 128, 256, 512, 1024, 2048, 4096)
LIBRARIES = (
    ("magic_enum", "magic_enum_benchmark", 20),
    ("enchantum", "enchantum_benchmark", 20),
    ("enum_name", "enum_name_benchmark", 20),
)
RSS_PATTERN = re.compile(r"BENCHMARK_MAX_RSS_KB=(\d+)")
ERROR_PATTERN = re.compile(r"(error:|fatal error:|CMake Error|Error [0-9])", re.IGNORECASE)
GCC_TOTAL_PATTERN = re.compile(r"^\s*TOTAL\s*:\s*(.*)$", re.MULTILINE)
GCC_TIME_PATTERN = re.compile(
    r"([0-9]+(?:\.[0-9]+)?)(?:\s*\(\s*[0-9]+%\s*\))?"
)


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


def run_logged(command, log_path, cwd, memory_tool=None, timeout_seconds=None):
    log_path.parent.mkdir(parents=True, exist_ok=True)
    timed_command = (
        [memory_tool, "-f", "BENCHMARK_MAX_RSS_KB=%M", *command]
        if memory_tool
        else command
    )
    with log_path.open("w", encoding="utf-8") as log:
        process = subprocess.Popen(
            timed_command,
            cwd=cwd,
            stdout=log,
            stderr=subprocess.STDOUT,
            text=True,
            start_new_session=(os.name == "posix"),
        )
        timed_out = False
        try:
            return_code = process.wait(timeout=timeout_seconds)
        except subprocess.TimeoutExpired:
            timed_out = True
            if os.name == "posix":
                try:
                    os.killpg(process.pid, signal.SIGTERM)
                except ProcessLookupError:
                    pass
                try:
                    process.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    try:
                        os.killpg(process.pid, signal.SIGKILL)
                    except ProcessLookupError:
                        pass
                    process.wait()
            else:
                process.kill()
                process.wait()
            return_code = 124
    output = log_path.read_text(encoding="utf-8", errors="replace")
    rss_match = RSS_PATTERN.search(output)
    peak_rss_mib = int(rss_match.group(1)) / 1024 if rss_match else None
    return return_code, peak_rss_mib, output, timed_out


def clang_trace_metrics(trace_path):
    trace = json.loads(trace_path.read_text(encoding="utf-8"))
    events = [
        event
        for event in trace.get("traceEvents", [])
        if event.get("ph") == "X" and isinstance(event.get("dur"), (int, float))
    ]

    def duration(name):
        matching = [event["dur"] for event in events if event.get("name") == name]
        return max(matching) / 1000 if matching else None

    excluded = {
        "Total ExecuteCompiler",
        "Total Frontend",
        "Total Backend",
        "Total Source",
    }
    totals = {}
    for event in events:
        name = event.get("name", "")
        if name.startswith("Total ") and name not in excluded:
            category = name[len("Total ") :]
            totals[category] = max(totals.get(category, 0), event["dur"] / 1000)
    hotspots = sorted(totals.items(), key=lambda item: item[1], reverse=True)[:3]
    compiler_duration = duration("Total ExecuteCompiler")
    if compiler_duration is None:
        compiler_duration = duration("ExecuteCompiler")
    if compiler_duration is None:
        raise ValueError("Clang compiler timing event was not found.")
    frontend_duration = duration("Total Frontend")
    if frontend_duration is None:
        frontend_duration = duration("Frontend")

    return {
        "compiler_seconds": compiler_duration / 1000,
        "frontend_seconds": (
            frontend_duration / 1000 if frontend_duration is not None else None
        ),
        "hotspots": [(name, duration / 1000) for name, duration in hotspots],
    }


def gcc_time_metrics(output):
    header = re.search(r"^\s*Time variable\s+(.*)$", output, re.MULTILINE)
    if not header:
        raise ValueError("GCC -ftime-report header was not found.")
    columns = header.group(1).split()
    wall_index = columns.index("wall") if "wall" in columns else -1

    total_match = GCC_TOTAL_PATTERN.search(output)
    if not total_match:
        raise ValueError("GCC -ftime-report TOTAL row was not found.")
    total_times = GCC_TIME_PATTERN.findall(total_match.group(1))
    if not total_times or wall_index >= len(total_times):
        raise ValueError("GCC -ftime-report TOTAL time was not recognized.")
    compiler_seconds = float(total_times[wall_index])

    hotspots = []
    for line in output.splitlines():
        phase = re.match(r"^\s*([^:]+?)\s*:\s*(.*)$", line)
        if not phase or phase.group(1).strip() == "TOTAL":
            continue
        phase_times = GCC_TIME_PATTERN.findall(phase.group(2))
        if not phase_times:
            continue
        index = wall_index if wall_index >= 0 else -1
        if -len(phase_times) <= index < len(phase_times):
            hotspots.append((phase.group(1).strip(), float(phase_times[index])))
    hotspots.sort(key=lambda item: item[1], reverse=True)

    return {
        "compiler_seconds": compiler_seconds,
        "frontend_seconds": None,
        "hotspots": hotspots[:3],
    }


def format_number(value, suffix="", precision=2):
    return "n/a" if value is None else f"{value:.{precision}f}{suffix}"


def short_log(log_path, root):
    if not log_path or not log_path.exists():
        return "No build log was created."
    lines = log_path.read_text(encoding="utf-8", errors="replace").splitlines()
    errors = [line for line in lines if ERROR_PATTERN.search(line)]
    excerpt = "\n".join((errors or lines)[-12:])
    relative_log = os.path.relpath(log_path, root)
    return f"Log: `{relative_log}`\n\n```text\n{excerpt}\n```"


def write_report(report_path, metadata, results, root):
    report_path.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "# Compile-time benchmark comparison",
        "",
        f"- Generated: {metadata['generated']}",
        f"- Compiler: `{metadata['compiler']}` ({metadata['compiler_version']})",
        f"- C++ standard: C++{metadata['standard']}",
        f"- Build type: `{metadata['build_type']}`",
        f"- Timing source: `{metadata['timing_source']}`",
        f"- Build jobs: {metadata['jobs']}",
        f"- Enum types per library: {metadata['enum_count']}",
        f"- Requested ranges: {', '.join(str(value) for value in metadata['ranges'])}",
        "",
        "Compile time comes from compiler output: Clang `-ftime-trace` or GCC",
        "`-ftime-report`. No external wall-clock build timings are reported.",
        "Peak RSS is collected from GNU time when available; otherwise it is `n/a`.",
        "",
        "| Range | Scan bounds | Enums | Values/enum | Library | Status | Compiler time (s) | Frontend time (s) | Peak RSS (MiB) | Compiler hotspots (s) |",
        "| ---: | ---: | ---: | ---: | --- | --- | ---: | ---: | ---: | --- |",
    ]

    for result in results:
        hotspots = ", ".join(
            f"{name}: {duration:.3f}".replace("|", r"\|")
            for name, duration in result["hotspots"]
        )
        lines.append(
            "| {range} | {bounds} | {enum_count} | {value_count} | {library} | "
            "{status} | {compiler_time} | {frontend_time} | {rss} | {hotspots} |".format(
                range=result["range"],
                bounds=f"{result['range_min']}..{result['range_max']}",
                enum_count=result["enum_count"],
                value_count=result["value_count"],
                library=result["library"],
                status=result["status"],
                compiler_time=format_number(result.get("compiler_seconds"), precision=3),
                frontend_time=format_number(
                    result.get("frontend_seconds"), precision=3
                ),
                rss=format_number(result.get("peak_rss_mib")),
                hotspots=hotspots or "n/a",
            )
        )

    failures = [
        result
        for result in results
        if result["status"] not in ("ok", "unsupported (C++20+)")
    ]
    if failures:
        lines.extend(["", "## Failed targets", ""])
        for result in failures:
            lines.extend(
                [
                    f"### Range {result['range']} "
                    f"({result['range_min']}..{result['range_max']}) — "
                    f"{result['library']}",
                    "",
                    result.get("error", "Build failed."),
                    "",
                    short_log(result.get("log_path"), root),
                    "",
                ]
            )

    report_path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def compiler_version(compiler):
    result = subprocess.run(
        [compiler, "--version"], capture_output=True, text=True, check=False
    )
    output = result.stdout or result.stderr
    return output.splitlines()[0] if output else "unknown"


def compiler_family(version):
    normalized = version.lower()
    if "clang" in normalized:
        return "clang"
    if "gcc" in normalized or "g++" in normalized:
        return "gcc"
    raise SystemExit("Compile-time benchmarks support GCC and Clang only.")


def parse_args():
    parser = argparse.ArgumentParser(
        description="Build enum reflection benchmarks and write a Markdown report."
    )
    parser.add_argument("--compiler", default=os.environ.get("CXX", "clang++"))
    parser.add_argument("--standard", type=int, default=20)
    parser.add_argument("--build-type", default="Release")
    parser.add_argument(
        "--jobs", type=int, default=8, help="CMake build parallelism (default: 8)"
    )
    parser.add_argument(
        "--timeout",
        type=int,
        default=1200,
        help="Per-target timeout in seconds (default: 1200)",
    )
    parser.add_argument("--enum-count", type=int, default=10)
    parser.add_argument(
        "--value-count",
        type=int,
        help="Declared members per enum (defaults to the selected range)",
    )
    parser.add_argument(
        "--range-min",
        type=int,
        help="Inclusive minimum value scanned by each library",
    )
    parser.add_argument(
        "--range-max",
        type=int,
        help="Inclusive maximum value scanned by each library",
    )
    parser.add_argument(
        "--ranges", type=int, nargs="+", choices=RANGE_CHOICES, default=(128,)
    )
    parser.add_argument(
        "--build-dir",
        type=Path,
        help="Build and log directory (default: build/compile-time-benchmarks)",
    )
    parser.add_argument(
        "--report",
        type=Path,
        help="Markdown output path (default: <build-dir>/report.md)",
    )
    return parser.parse_args()


def main():
    args = parse_args()
    if args.standard < 17:
        raise SystemExit("--standard must be 17 or later.")
    if args.jobs < 1:
        raise SystemExit("--jobs must be at least 1.")
    if args.timeout < 1:
        raise SystemExit("--timeout must be at least 1 second.")
    if not 1 <= args.enum_count <= 1000:
        raise SystemExit("--enum-count must be from 1 to 1000.")
    if args.value_count is not None and args.value_count < 1:
        raise SystemExit("--value-count must be positive.")
    if (args.range_min is None) != (args.range_max is None):
        raise SystemExit("--range-min and --range-max must be specified together.")

    root = Path(__file__).resolve().parents[1]
    build_root = (args.build_dir or root / "build" / "compile-time-benchmarks").resolve()
    report_path = (args.report or build_root / "report.md").resolve()
    memory_tool = find_gnu_time()
    version = compiler_version(args.compiler)
    family = compiler_family(version)
    metadata = {
        "generated": datetime.datetime.now(datetime.timezone.utc).isoformat(),
        "compiler": args.compiler,
        "compiler_version": version,
        "timing_source": (
            "Clang -ftime-trace" if family == "clang" else "GCC -ftime-report"
        ),
        "standard": args.standard,
        "build_type": args.build_type,
        "jobs": args.jobs,
        "enum_count": args.enum_count,
        "value_count": args.value_count,
        "ranges": args.ranges,
    }
    results = []
    supported_libraries = [
        library for library in LIBRARIES if args.standard >= library[2]
    ]
    unsupported_libraries = [
        library for library in LIBRARIES if args.standard < library[2]
    ]

    print(f"Writing benchmark results to {report_path}")
    if not memory_tool:
        print("GNU time not found; peak RSS metrics will be n/a.")

    for enum_range in args.ranges:
        value_count = (
            enum_range if args.value_count is None else args.value_count
        )
        if args.range_min is None and args.range_max is None:
            range_min, range_max = 0, enum_range - 1
        else:
            range_min = args.range_min
            range_max = args.range_max
        if value_count > enum_range:
            raise SystemExit(
                f"--value-count ({value_count}) cannot exceed range {enum_range}."
            )
        if range_min > 0 or range_max < value_count - 1:
            raise SystemExit(
                "The scan bounds must include all generated values 0.."
                f"{value_count - 1}."
            )
        if range_max <= range_min:
            raise SystemExit("--range-max must be greater than --range-min.")
        build_dir = (
            build_root
            / f"cpp{args.standard}"
            / f"enums-{args.enum_count}"
            / f"values-{value_count}"
            / f"range-{enum_range}"
            / f"bounds-{range_min}-{range_max}"
        )
        log_dir = (
            build_root
            / "logs"
            / f"cpp{args.standard}"
            / f"enums-{args.enum_count}"
            / f"values-{value_count}"
            / f"range-{enum_range}"
            / f"bounds-{range_min}-{range_max}"
        )
        configure_log = log_dir / "configure.log"
        configure_command = [
            "cmake",
            "-S",
            str(root),
            "-B",
            str(build_dir),
            f"-DCMAKE_CXX_COMPILER={args.compiler}",
            f"-DCMAKE_CXX_STANDARD={args.standard}",
            f"-DCMAKE_BUILD_TYPE={args.build_type}",
            f"-DENUM_NAME_BENCHMARK_RANGE={enum_range}",
            f"-DENUM_NAME_BENCHMARK_VALUE_COUNT={value_count}",
            f"-DENUM_NAME_BENCHMARK_ENUM_COUNT={args.enum_count}",
            f"-DENUM_NAME_BENCHMARK_MIN_RANGE={range_min}",
            f"-DENUM_NAME_BENCHMARK_MAX_RANGE={range_max}",
            "-DENUM_NAME_BUILD_TESTS=OFF",
            "-DENUM_NAME_BUILD_EXAMPLE=OFF",
            "-DENUM_NAME_BUILD_BENCHMARKS=ON",
            f"-DFETCHCONTENT_BASE_DIR={build_root / 'fetchcontent'}",
        ]
        print(f"Configuring C++{args.standard}, range {enum_range}...")
        configure_code, _, configure_output, configure_timed_out = run_logged(
            configure_command, configure_log, root, timeout_seconds=args.timeout
        )
        if configure_timed_out or configure_code != 0:
            for library, _, _ in supported_libraries:
                results.append(
                    {
                        "range": enum_range,
                        "range_min": range_min,
                        "range_max": range_max,
                        "enum_count": args.enum_count,
                        "value_count": value_count,
                        "library": library,
                        "status": "configure failed",
                        "log_path": configure_log,
                        "error": (
                            f"CMake configuration timed out after {args.timeout}s."
                            if configure_timed_out
                            else "CMake configuration failed."
                        ),
                        "hotspots": [],
                    }
                )
            for library, _, _ in unsupported_libraries:
                results.append(
                    {
                        "range": enum_range,
                        "range_min": range_min,
                        "range_max": range_max,
                        "enum_count": args.enum_count,
                        "value_count": value_count,
                        "library": library,
                        "status": "unsupported (C++20+)",
                        "hotspots": [],
                        "error": "This library requires C++20 or later.",
                    }
                )
            write_report(report_path, metadata, results, root)
            print(configure_output[-2000:], file=sys.stderr)
            continue

        for library, target, minimum_standard in LIBRARIES:
            if args.standard < minimum_standard:
                results.append(
                    {
                        "range": enum_range,
                        "range_min": range_min,
                        "range_max": range_max,
                        "enum_count": args.enum_count,
                        "value_count": value_count,
                        "library": library,
                        "status": "unsupported (C++20+)",
                        "hotspots": [],
                        "error": "This library requires C++20 or later.",
                    }
                )
                write_report(report_path, metadata, results, root)
                continue

            log_path = log_dir / f"{library}.log"
            trace_path = (
                build_dir
                / "benchmarks"
                / "CMakeFiles"
                / f"{target}.dir"
                / f"{target}.cpp.json"
            )
            if trace_path.exists():
                trace_path.unlink()
            build_command = [
                "cmake",
                "--build",
                str(build_dir),
                "--target",
                target,
                "--clean-first",
                "--parallel",
                str(args.jobs),
            ]
            print(f"  Building {library}...")
            code, peak_rss_mib, build_output, timed_out = run_logged(
                build_command,
                log_path,
                root,
                memory_tool,
                timeout_seconds=args.timeout,
            )
            result = {
                "range": enum_range,
                "range_min": range_min,
                "range_max": range_max,
                "enum_count": args.enum_count,
                "value_count": value_count,
                "library": library,
                "status": "ok",
                "peak_rss_mib": peak_rss_mib,
                "log_path": log_path,
                "hotspots": [],
            }
            if timed_out:
                result["status"] = "timed out"
                result["error"] = (
                    f"Build exceeded the {args.timeout}-second timeout."
                )
            elif code != 0:
                result["status"] = "build failed"
                result["error"] = "CMake failed to build the benchmark target."
            else:
                try:
                    if family == "clang":
                        if not trace_path.exists():
                            raise ValueError(
                                "Clang did not produce the expected time trace."
                            )
                        result.update(clang_trace_metrics(trace_path))
                    else:
                        result.update(gcc_time_metrics(build_output))
                except (OSError, ValueError, KeyError) as error:
                    result["status"] = "compiler timing missing"
                    result["error"] = f"Could not parse compiler timing: {error}"
            results.append(result)
            write_report(report_path, metadata, results, root)

    failed = sum(
        result["status"] not in ("ok", "unsupported (C++20+)")
        for result in results
    )
    print(f"Report: {report_path}")
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
