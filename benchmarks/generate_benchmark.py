#!/usr/bin/env python3

import argparse
from pathlib import Path


RANGE_CHOICES = (32, 64, 128, 256, 512, 1024, 2048, 4096)
DEFAULT_ENUM_COUNT = 200
VALUES_PER_LINE = 8
CHECK_CHUNK_SIZE = 16


def generate(value_count: int, enum_count: int) -> str:
    lines = []
    for enum_index in range(enum_count):
        lines.append(f"enum class Enum{enum_index:02d} {{")
        for start in range(0, value_count, VALUES_PER_LINE):
            end = min(start + VALUES_PER_LINE, value_count)
            values = ", ".join(f"V{index}" for index in range(start, end))
            if end < value_count:
                values += ","
            lines.append(f"  {values}")
        lines.append("};")
        for start in range(0, value_count, CHECK_CHUNK_SIZE):
            count = min(CHECK_CHUNK_SIZE, value_count - start)
            lines.append(
                f"static_assert(reflect_enum_values<Enum{enum_index:02d}, "
                f"{start}, {count}>());"
            )
    return "\n".join(lines) + "\n"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--range", type=int, choices=RANGE_CHOICES, required=True)
    parser.add_argument("--value-count", type=int)
    parser.add_argument("--enum-count", type=int, default=DEFAULT_ENUM_COUNT)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    value_count = args.range if args.value_count is None else args.value_count
    if not 1 <= value_count <= args.range:
        parser.error("--value-count must be from 1 to --range")
    if not 1 <= args.enum_count <= 1000:
        parser.error("--enum-count must be an integer from 1 to 1000")

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(
        generate(value_count, args.enum_count), encoding="utf-8"
    )


if __name__ == "__main__":
    main()
