#!/usr/bin/env python3

import argparse
from pathlib import Path


TEST_CASES = (
    {
        "name": "big_enums",
        "value_count": 200,
        "enum_count": 32,
        "min_range": -256,
        "max_range": 256,
    },
    {
        "name": "small_enums",
        "value_count": 16,
        "enum_count": 200,
        "min_range": -128,
        "max_range": 128,
    },
    {
        "name": "small_enums_large_range",
        "value_count": 600,
        "enum_count": 200,
        "min_range": -1024,
        "max_range": 1024,
    },
    {
        "name": "ideal_range",
        "value_count": 50,
        "enum_count": 100,
        "min_range": 0,
        "max_range": 50,
    },
)

LIBRARIES = {
    "magic_enum": {
        "include_path": "magic_enum/magic_enum.hpp",
        "to_string": "magic_enum::enum_name",
        "min_macro": "MAGIC_ENUM_RANGE_MIN",
        "max_macro": "MAGIC_ENUM_RANGE_MAX",
    },
    "enchantum": {
        "include_path": "enchantum/enchantum.hpp",
        "to_string": "enchantum::to_string",
        "min_macro": "ENCHANTUM_MIN_RANGE",
        "max_macro": "ENCHANTUM_MAX_RANGE",
    },
    "enum_name": {
        "include_path": "mgutility/reflection/enum_name.hpp",
        "to_string": "mgutility::enum_name",
        "min_macro": "MGUTILITY_ENUM_RANGE_MIN",
        "max_macro": "MGUTILITY_ENUM_RANGE_MAX",
    },
}


def create_source(library, config, case, enum_count, filename):
    with filename.open("w", encoding="utf-8") as source:
        source.write(f"#define {config['min_macro']} ({case['min_range']})\n")
        source.write(f"#define {config['max_macro']} ({case['max_range']})\n")
        source.write(f"#include <{config['include_path']}>\n\n")

        for enum_index in range(enum_count):
            enum_name = f"A_{enum_index}"
            source.write(f"enum class {enum_name} : int {{\n")
            for value in range(case["value_count"]):
                source.write(f"    {enum_name}_{value} = {value},\n")
            source.write("};\n\n")

        source.write(
            "template <typename T>\n"
            "volatile T var = T{};\n\n"
            "const char* volatile pointer = nullptr;\n"
            "int main() {\n"
        )
        for enum_index in range(enum_count):
            source.write(
                f"    pointer = {config['to_string']}(var<A_{enum_index}>).data();\n"
            )
        source.write("    return 0;\n}\n")


def generate_sources(output_dir, selected_cases=None, enum_count=None):
    output_dir.mkdir(parents=True, exist_ok=True)
    selected = set(selected_cases) if selected_cases else None
    sources = []
    for case in TEST_CASES:
        if selected is not None and case["name"] not in selected:
            continue
        count = case["enum_count"] if enum_count is None else enum_count
        if not 1 <= count <= 1000:
            raise ValueError("enum count must be from 1 to 1000")
        for library, config in LIBRARIES.items():
            filename = output_dir / f"{library}_{case['name']}.cpp"
            create_source(library, config, case, count, filename)
            sources.append(
                {
                    "library": library,
                    "case": case,
                    "enum_count": count,
                    "source": filename,
                }
            )
    return sources


def main():
    parser = argparse.ArgumentParser(
        description="Generate Enchantum-style enum compile-time benchmark sources."
    )
    parser.add_argument("--output-dir", type=Path, default=Path("generated"))
    parser.add_argument(
        "--cases",
        nargs="+",
        choices=[case["name"] for case in TEST_CASES],
        help="Generate selected test cases (default: all).",
    )
    parser.add_argument(
        "--enum-count",
        type=int,
        help="Override each test case's enum count (1-1000).",
    )
    args = parser.parse_args()
    if args.enum_count is not None and not 1 <= args.enum_count <= 1000:
        parser.error("--enum-count must be from 1 to 1000")

    sources = generate_sources(args.output_dir, args.cases, args.enum_count)
    print(f"Generated {len(sources)} benchmark translation units in {args.output_dir}")


if __name__ == "__main__":
    main()
