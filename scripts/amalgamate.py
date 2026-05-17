#!/usr/bin/env python3
"""
Script to amalgamate the enum_name library into a single header file.

Usage:
    python3 scripts/amalgamate.py \
        --main include/mgutility/reflection/enum_name.hpp \
        --source-dir include \
        --source-dir mgutility/include \
        --output single_include/mgutility_enum_name.hpp
"""

import argparse
import os
import re
import subprocess
import sys
from typing import List, Optional, Set, Tuple


# Regex to match #include directives
RE_INCLUDE_LOCAL = re.compile(r'#\s*include\s+"([^"]+)"')
RE_INCLUDE_SYSTEM = re.compile(r'#\s*include\s+<([^>]+)>')

# Regex to match include guards
RE_IFNDEF = re.compile(r'#\s*ifndef\s+(\w+)')
RE_DEFINE = re.compile(r'#\s*define\s+(\w+)')
RE_PRAGMA_ONCE = re.compile(r'#\s*pragma\s+once')


def find_file(include_path: str, source_dirs: List[str],
              current_dir: Optional[str] = None) -> Optional[str]:
    """Try to find an included file.

    First tries relative to the including file's directory (current_dir),
    then tries in each source directory.
    """
    # Try relative to the including file's directory first
    if current_dir:
        candidate = os.path.normpath(os.path.join(current_dir, include_path))
        if os.path.isfile(candidate):
            return candidate

    # Then try in source directories
    for src_dir in source_dirs:
        full_path = os.path.normpath(os.path.join(src_dir, include_path))
        if os.path.isfile(full_path):
            return full_path
    return None


def read_file_content(filepath: str) -> Tuple[List[str], Optional[str]]:
    """
    Read a file and return (lines, guard_macro_or_none).
    Also detects #pragma once and returns it as a pseudo-guard.
    """
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    lines = content.splitlines(keepends=True)

    # Try to find a header guard: #ifndef X / #define X at the top
    guard = None
    for i in range(min(len(lines), 20)):
        stripped = lines[i].strip()
        if RE_PRAGMA_ONCE.match(stripped):
            guard = f"__PRAGMA_ONCE_{os.path.basename(filepath)}__"
            break
        if RE_IFNDEF.match(stripped):
            m = RE_IFNDEF.match(stripped)
            guard_candidate = m.group(1)
            # Check next non-empty, non-comment line for #define
            for j in range(i + 1, min(len(lines), i + 5)):
                m2 = RE_DEFINE.match(lines[j].strip())
                if m2 and m2.group(1) == guard_candidate:
                    guard = guard_candidate
                    break
            break

    return lines, guard


def inline_file(
    filepath: str,
    source_dirs: List[str],
    seen_guards: Set[str],
    processed_files: Set[str],
) -> List[str]:
    """
    Inline the content of a project header file, resolving its local includes.
    Returns a list of strings (lines).
    """
    abs_path = os.path.abspath(filepath)

    lines, guard = read_file_content(abs_path)

    if guard and guard in seen_guards:
        return []
    if abs_path in processed_files:
        return []

    if guard:
        seen_guards.add(guard)
    processed_files.add(abs_path)

    result: List[str] = []

    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        # Check for local include
        m_local = RE_INCLUDE_LOCAL.match(stripped)
        if m_local:
            inc_path = m_local.group(1)
            resolved = find_file(inc_path, source_dirs,
                                 os.path.dirname(abs_path))

            if resolved and os.path.isfile(resolved):
                sub_lines = inline_file(resolved, source_dirs, seen_guards,
                                        processed_files)
                # Add a separator blank line before the inlined content
                if sub_lines:
                    result.append('\n')
                result.extend(sub_lines)
                i += 1
                continue

            # Not found in project dirs - keep as-is
            result.append(line)
            i += 1
            continue

        # Everything else
        result.append(line)
        i += 1

    return result


def main():
    parser = argparse.ArgumentParser(
        description="Amalgamate enum_name headers into a single include file."
    )
    parser.add_argument(
        '--main',
        required=True,
        help='Main entry point header file '
             '(e.g. include/mgutility/reflection/enum_name.hpp)'
    )
    parser.add_argument(
        '--source-dir',
        required=True,
        action='append',
        dest='source_dirs',
        help='Include root directories to resolve local includes '
             '(can be specified multiple times)'
    )
    parser.add_argument(
        '--output',
        required=True,
        help='Output file path for the amalgamated header'
    )

    args = parser.parse_args()

    if not os.path.isfile(args.main):
        print(f"Error: Main file '{args.main}' not found.", file=sys.stderr)
        sys.exit(1)

    for sd in args.source_dirs:
        if not os.path.isdir(sd):
            print(f"Error: Source directory '{sd}' not found.", file=sys.stderr)
            sys.exit(1)

    seen_guards: Set[str] = set()
    processed_files: Set[str] = set()

    abs_main = os.path.abspath(args.main)
    lines, guard = read_file_content(abs_main)

    # Register the main file's guard
    if guard:
        seen_guards.add(guard)
    processed_files.add(abs_main)

    output_lines: List[str] = []

    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        # Check for local include: #include "..."
        m_local = RE_INCLUDE_LOCAL.match(stripped)
        if m_local:
            inc_path = m_local.group(1)
            resolved = find_file(inc_path, args.source_dirs,
                                 os.path.dirname(abs_main))

            if resolved and os.path.isfile(resolved):
                # Pre-check: if the included file has a guard already seen
                _, inc_guard = read_file_content(resolved)

                if inc_guard and inc_guard in seen_guards:
                    i += 1
                    continue

                # Mark guard as seen now so recursive processing skips it
                if inc_guard:
                    seen_guards.add(inc_guard)

                # Inline the content
                sub_lines = inline_file(resolved, args.source_dirs,
                                        seen_guards, processed_files)
                if sub_lines:
                    output_lines.append('\n')
                output_lines.extend(sub_lines)

                i += 1
                continue

            # Not found in project dirs - keep the include line as-is
            output_lines.append(line)
            i += 1
            continue

        # Check for system include: #include <...>
        m_sys = RE_INCLUDE_SYSTEM.match(stripped)
        if m_sys:
            output_lines.append(line)
            i += 1
            continue

        # Everything else: comments, code, blank lines, etc.
        output_lines.append(line)
        i += 1

    # Write output
    os.makedirs(os.path.dirname(args.output), exist_ok=True)
    with open(args.output, 'w', encoding='utf-8') as f:
        f.writelines(output_lines)

    # Run clang-format on the output file (if available)
    try:
        subprocess.run(
            ['clang-format', '-i', '-style=file', args.output],
            check=True, capture_output=True, timeout=30
        )
        print(f"Formatted: {args.output}")
    except FileNotFoundError:
        print("Warning: clang-format not found, skipping formatting",
              file=sys.stderr)
    except subprocess.CalledProcessError as e:
        print(f"Warning: clang-format failed: {e.stderr.decode().strip()}",
              file=sys.stderr)

    print(f"Amalgamated header written to: {args.output}")


if __name__ == '__main__':
    main()