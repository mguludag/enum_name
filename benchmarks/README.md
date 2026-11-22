# Compile-time benchmarks

This benchmark adopts the generator and test-case model from
[Enchantum's benchmark][enchantum-benchmark], with `enum_name` added alongside
`magic_enum` and `enchantum`. The adapted generator and its MIT license notice
are in this directory.

[enchantum-benchmark]: https://github.com/ZXShady/enchantum/tree/6ada6cbe086e74355c76437e3e1931985272f426/benchmarks

The four workloads match Enchantum's generator:

| Test case                 | Enums | Values per enum | Search range |
| ------------------------- | ----: | --------------: | -----------: |
| `small_enums`             |   200 |              16 |    -128..128 |
| `big_enums`               |    32 |             200 |    -256..256 |
| `small_enums_large_range` |   200 |              16 |  -1024..1024 |
| `ideal_range`             |   100 |              50 |        0..50 |

As in the upstream benchmark, each generated translation unit calls the
library's name-conversion function once for each enum. This intentionally
replaces the previous all-members workload. The generator can override the
enum count from 1 to 1000 for smoke tests or custom comparisons.

Generate sources without compiling:

```sh
python3 benchmarks/gen.py --output-dir build/enum-bench/generated
```

Run all cases with GCC or Clang:

```sh
python3 benchmarks/run.py \
  --compiler g++ \
  --standard 17 \
  --jobs 2 \
  --build-dir build/enum-bench-gcc
```

The runner fetches pinned `magic_enum` and Enchantum headers into the build
directory, compiles each generated source independently with `-O3`, and
produces `report.md` and compiler logs. Both Clang and GCC use `-ftime-report`;
the runner extracts compiler and frontend timings from Clang's text summary
and includes the raw report for each candidate in a collapsed section after
the results table. The table also includes each successful compile's
object-file size in KiB. No external wall-clock compile times are reported.
GNU `time` is used only for peak RSS when available.

Clang is given a 10,000,000-step constexpr limit for all candidates so large
search ranges can be compared without hitting the compiler's default
evaluation ceiling. Each compile has a 20-minute timeout by default. Before
each compile, the runner checks available system memory and gives that compile
a budget of 95% of available RAM divided by `--jobs`. If a process group
exceeds its budget, it is stopped, marked as skipped for memory, and the next
candidate runs.

In GitHub Actions, each candidate also emits a notice annotation with its
status and timing. Skipped targets are warnings; compile or report-parsing
failures are errors.

Use `--cases` to select cases and `--enum-count` to override their enum count:

```sh
python3 benchmarks/run.py \
  --compiler clang++ \
  --cases small_enums \
  --enum-count 2 \
  --jobs 1 \
  --build-dir build/enum-bench-smoke
```

The GitHub Actions workflow runs the full matrix with GCC and Clang and
publishes each Markdown report in the run summary and as an artifact. Runs on
`main` also update `latest.md`, `gcc.md`, and `clang.md` on the
`benchmark-reports` branch. The stable report index is available at
[`benchmark-reports/latest.md`](https://github.com/mguludag/enum_name/blob/benchmark-reports/latest.md).
Compare compiler timings only across the same compiler version and machine.
