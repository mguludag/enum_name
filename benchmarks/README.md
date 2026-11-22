# Compile-time benchmarks

These opt-in benchmarks compile equivalent translation units separately against
`enum_name`, `magic_enum`, and `enchantum`. A Python generator creates a
configurable number of distinct enums with contiguous declared members, and
each target calls its library's name-conversion function for every declared
member at compile time. The checks are evaluated in chunks of 16 to manage
compiler constexpr work.

Direct CMake builds default to range `2048`, 200 enums, and 2048 declared
members per enum, with inclusive scan bounds `[0, 2047]`. The Python driver
defaults to range `128` and 10 enums. Both default to C++20 so all three
libraries are included. The supported `--ranges` values are `32`, `64`, `128`,
`256`, `512`, `1024`, `2048`, and `4096`. `--value-count` can
override the number of declared members independently of the scan range;
`--range-min` and `--range-max` set inclusive scan bounds. For example, the
default CMake workload can be run through the driver with:

```sh
python3 benchmarks/run_benchmarks.py \
  --ranges 2048 \
  --enum-count 200
```

This dense workload deliberately differs from Enchantum's published Large
Range case, which declares only 16 members in each of its 200 enums. Here every
library's name-conversion function is checked for all 2048 declared members.
Consequently, the published timings are not directly comparable.

Python 3 is required when the benchmark option is enabled. Choose the enum
count with `--enum-count` in the driver or
`ENUM_NAME_BENCHMARK_ENUM_COUNT` in CMake; it ranges from `1` to `1000`
(default: `200`).

`enum_name` and `magic_enum` can be benchmarked with C++17 and later.
`enchantum` is included only with C++20 and later; C++17 reports it as
unsupported.

Use the driver script to generate each workload, build the benchmark targets,
collect compiler-reported timing and peak memory where GNU `time` is available,
and write a Markdown comparison report. Clang timings use `-ftime-trace`; GCC
timings use `-ftime-report`. No external wall-clock measurements are reported.
CMake 3.22 or later and Python 3 are required.

```sh
python3 benchmarks/run_benchmarks.py --standard 17
```

The GitHub Actions workflow runs GCC and Clang at ranges `256`, `1024`, and
`2048`, with 200 enums. Each job appends the Markdown report to the workflow
run summary and uploads the report and logs as an artifact.

By default, the script tests range 128 and writes
`build/compile-time-benchmarks/report.md`, with build logs and trace JSON files
under the same directory. Larger ranges and enum counts can take substantially
longer; for example, one enum at range 4096 took about 285 seconds for
`enum_name` in a local run. It uses 8 CMake build jobs by default; override
this with `--jobs`. Each library target is built with CMake's `--clean-first`,
so prior object files are not reused. To run selected ranges or choose another standard:

```sh
python3 benchmarks/run_benchmarks.py \
  --standard 20 \
  --enum-count 100 \
  --ranges 256 1024 4096 \
  --build-dir build/enum-bench-cpp20
```

Pass all seven supported values to compare every range in one run.

The report includes declared values per enum, inclusive scan bounds, compiler
time, Clang frontend time when available, compiler-reported hotspots, peak RSS
(when GNU `time` is available), and failures.
Individual CMake targets are `enum_name_benchmark`, `magic_enum_benchmark`,
and `enchantum_benchmark`. Configuration and each target build have a
20-minute timeout by default; override it with `--timeout` (seconds). Timed-out
targets are recorded in the report and the other libraries continue.

The `enum_name` benchmark sizes its fixed compile-time string buffer to the
generated member names (with an 8192-byte minimum), rather than reserving
65536 bytes for every enum. Clang and GCC use compiler-specific constexpr
operation limits. With the pinned `enchantum` version and Clang 22, the dense
4096-member target currently fails in Enchantum's Clang reflection
implementation. Build selected targets individually to get their measurements:

```sh
cmake --build build-bench --target enum_name_benchmark magic_enum_benchmark
```

Clang JSON traces can be opened in a time-trace viewer (for example, Chrome's
`chrome://tracing`). GCC phase timings are recorded in the build logs and
summarized in the report. Compare timings only when using the same compiler
version, standard, build configuration, and machine.
