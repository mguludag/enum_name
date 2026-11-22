# Compile-time benchmark comparison

- Generated: 2026-11-22T22:56:40.768673+00:00
- Compiler: `clang++` (Ubuntu clang version 18.1.3 (1ubuntu1))
- C++ standard: C++20
- Optimization: `-O3`
- Timing source: `Clang -ftime-report`
- Build jobs: 2
- Test cases: big_enums, small_enums, small_enums_large_range, ideal_range

Workloads follow Enchantum's benchmark generator: each generated enum
has the configured number of values, and the name-conversion function
is called once per enum. Compiler time is taken from Clang or GCC
`-ftime-report`; no external wall-clock build
times are reported. Each active compile has a memory budget equal to
95% of currently available RAM divided by the
configured parallel job count. A compile exceeding its budget is
stopped and the next candidate runs.

| Test case | Enums | Values/enum | Search range | Library | Status | Compiler time (s) | Frontend time (s) | Object file (KiB) | Peak RSS (MiB) | Memory budget (MiB) | Compiler hotspots (s) |
| --- | ---: | ---: | ---: | --- | --- | ---: | ---: | ---: | ---: | ---: | --- |
| big_enums | 32 | 200 | -256..256 | [enchantum](https://github.com/ZXShady/enchantum) | ok | 2.576 | 2.576 | 91.45 | 186.46 | 7023.29 | Clang front-end timer: 2.576 |
| big_enums | 32 | 200 | -256..256 | [enum_name](https://github.com/mguludag/enum_name) | ok | 2.463 | 2.463 | 313.90 | 150.53 | 6980.90 | Clang front-end timer: 2.463 |
| big_enums | 32 | 200 | -256..256 | [magic_enum](https://github.com/Neargye/magic_enum) | ok | 11.106 | 11.106 | 1705.33 | 484.27 | 7023.29 | Clang front-end timer: 11.106 |
| ideal_range | 100 | 50 | 0..50 | [enchantum](https://github.com/ZXShady/enchantum) | ok | 1.513 | 1.513 | 132.48 | 194.79 | 5564.40 | Clang front-end timer: 1.513 |
| ideal_range | 100 | 50 | 0..50 | [enum_name](https://github.com/mguludag/enum_name) | ok | 1.689 | 1.689 | 891.30 | 144.40 | 5498.43 | Clang front-end timer: 1.689 |
| ideal_range | 100 | 50 | 0..50 | [magic_enum](https://github.com/Neargye/magic_enum) | ok | 4.836 | 4.836 | 1377.43 | 293.08 | 5624.22 | Clang front-end timer: 4.836 |
| small_enums | 200 | 16 | -128..128 | [enchantum](https://github.com/ZXShady/enchantum) | ok | 3.377 | 3.377 | 196.85 | 352.25 | 6790.06 | Clang front-end timer: 3.377 |
| small_enums | 200 | 16 | -128..128 | [enum_name](https://github.com/mguludag/enum_name) | ok | 6.644 | 6.644 | 1853.03 | 236.29 | 6660.21 | Clang front-end timer: 6.644 |
| small_enums | 200 | 16 | -128..128 | [magic_enum](https://github.com/Neargye/magic_enum) | ok | 24.806 | 24.806 | 972.39 | 1086.15 | 6909.68 | Clang front-end timer: 24.806 |
| small_enums_large_range | 200 | 600 | -1024..1024 | [enchantum](https://github.com/ZXShady/enchantum) | ok | 15.946 | 15.946 | 1490.57 | 2014.21 | 6736.61 | Clang front-end timer: 15.946 |
| small_enums_large_range | 200 | 600 | -1024..1024 | [enum_name](https://github.com/mguludag/enum_name) | ok | 48.993 | 48.993 | 2556.55 | 1075.26 | 6395.66 | Clang front-end timer: 48.993 |
| small_enums_large_range | 200 | 600 | -1024..1024 | [magic_enum](https://github.com/Neargye/magic_enum) | skipped (memory limit) | n/a | n/a | n/a | 6620.64 | 6618.36 | n/a |

## Raw compiler output

<details>
<summary>big_enums — enchantum (ok)</summary>


```text
Clang front-end time report
===-------------------------------------------------------------------------===
  Total Execution Time: 1.1237 seconds (2.5765 wall clock)

   1.0306 (100.0%)   0.0931 (100.0%)   1.1237 (100.0%)   2.5765 (100.0%)  Clang front-end timer
   1.0306 (100.0%)   0.0931 (100.0%)   1.1237 (100.0%)   2.5765 (100.0%)  Total
```

</details>

<details>
<summary>big_enums — enum_name (ok)</summary>


```text
Clang front-end time report
===-------------------------------------------------------------------------===
  Total Execution Time: 2.3724 seconds (2.4634 wall clock)

   2.3193 (100.0%)   0.0531 (100.0%)   2.3724 (100.0%)   2.4634 (100.0%)  Clang front-end timer
   2.3193 (100.0%)   0.0531 (100.0%)   2.3724 (100.0%)   2.4634 (100.0%)  Total
```

</details>

<details>
<summary>big_enums — magic_enum (ok)</summary>


```text
Clang front-end time report
===-------------------------------------------------------------------------===
  Total Execution Time: 9.9345 seconds (11.1059 wall clock)

   9.6677 (100.0%)   0.2669 (100.0%)   9.9345 (100.0%)  11.1059 (100.0%)  Clang front-end timer
   9.6677 (100.0%)   0.2669 (100.0%)   9.9345 (100.0%)  11.1059 (100.0%)  Total
```

</details>

<details>
<summary>ideal_range — enchantum (ok)</summary>


```text
Clang front-end time report
===-------------------------------------------------------------------------===
  Total Execution Time: 1.5125 seconds (1.5126 wall clock)

   1.4318 (100.0%)   0.0807 (100.0%)   1.5125 (100.0%)   1.5126 (100.0%)  Clang front-end timer
   1.4318 (100.0%)   0.0807 (100.0%)   1.5125 (100.0%)   1.5126 (100.0%)  Total
```

</details>

<details>
<summary>ideal_range — enum_name (ok)</summary>


```text
Clang front-end time report
===-------------------------------------------------------------------------===
  Total Execution Time: 1.6886 seconds (1.6890 wall clock)

   1.6199 (100.0%)   0.0687 (100.0%)   1.6886 (100.0%)   1.6890 (100.0%)  Clang front-end timer
   1.6199 (100.0%)   0.0687 (100.0%)   1.6886 (100.0%)   1.6890 (100.0%)  Total
```

</details>

<details>
<summary>ideal_range — magic_enum (ok)</summary>


```text
Clang front-end time report
===-------------------------------------------------------------------------===
  Total Execution Time: 4.8338 seconds (4.8356 wall clock)

   4.6998 (100.0%)   0.1340 (100.0%)   4.8338 (100.0%)   4.8356 (100.0%)  Clang front-end timer
   4.6998 (100.0%)   0.1340 (100.0%)   4.8338 (100.0%)   4.8356 (100.0%)  Total
```

</details>

<details>
<summary>small_enums — enchantum (ok)</summary>


```text
Clang front-end time report
===-------------------------------------------------------------------------===
  Total Execution Time: 3.3768 seconds (3.3771 wall clock)

   3.1467 (100.0%)   0.2300 (100.0%)   3.3768 (100.0%)   3.3771 (100.0%)  Clang front-end timer
   3.1467 (100.0%)   0.2300 (100.0%)   3.3768 (100.0%)   3.3771 (100.0%)  Total
```

</details>

<details>
<summary>small_enums — enum_name (ok)</summary>


```text
Clang front-end time report
===-------------------------------------------------------------------------===
  Total Execution Time: 6.6429 seconds (6.6436 wall clock)

   6.4700 (100.0%)   0.1729 (100.0%)   6.6429 (100.0%)   6.6436 (100.0%)  Clang front-end timer
   6.4700 (100.0%)   0.1729 (100.0%)   6.6429 (100.0%)   6.6436 (100.0%)  Total
```

</details>

<details>
<summary>small_enums — magic_enum (ok)</summary>


```text
Clang front-end time report
===-------------------------------------------------------------------------===
  Total Execution Time: 24.7977 seconds (24.8061 wall clock)

  24.1637 (100.0%)   0.6340 (100.0%)  24.7977 (100.0%)  24.8061 (100.0%)  Clang front-end timer
  24.1637 (100.0%)   0.6340 (100.0%)  24.7977 (100.0%)  24.8061 (100.0%)  Total
```

</details>

<details>
<summary>small_enums_large_range — enchantum (ok)</summary>


```text
Clang front-end time report
===-------------------------------------------------------------------------===
  Total Execution Time: 15.9081 seconds (15.9459 wall clock)

  15.1781 (100.0%)   0.7300 (100.0%)  15.9081 (100.0%)  15.9459 (100.0%)  Clang front-end timer
  15.1781 (100.0%)   0.7300 (100.0%)  15.9081 (100.0%)  15.9459 (100.0%)  Total
```

</details>

<details>
<summary>small_enums_large_range — enum_name (ok)</summary>


```text
Clang front-end time report
===-------------------------------------------------------------------------===
  Total Execution Time: 48.9823 seconds (48.9935 wall clock)

  48.5246 (100.0%)   0.4577 (100.0%)  48.9823 (100.0%)  48.9935 (100.0%)  Clang front-end timer
  48.5246 (100.0%)   0.4577 (100.0%)  48.9823 (100.0%)  48.9935 (100.0%)  Total
```

</details>

<details>
<summary>small_enums_large_range — magic_enum (skipped (memory limit))</summary>

Compiler log (raw timing data unavailable):

```text
$ clang++ -std=c++20 -O3 -c /home/runner/work/enum_name/enum_name/build/compile-time-benchmarks-clang/generated/magic_enum_small_enums_large_range.cpp -o /home/runner/work/enum_name/enum_name/build/compile-time-benchmarks-clang/magic_enum_small_enums_large_range.o -I /home/runner/work/enum_name/enum_name/include -I /home/runner/work/enum_name/enum_name/mgutility/include -I /home/runner/work/enum_name/enum_name/build/compile-time-benchmarks-clang/dependencies/magic_enum-v0.9.7/include -I /home/runner/work/enum_name/enum_name/build/compile-time-benchmarks-clang/dependencies/enchantum-0.5.0/include -fconstexpr-steps=10000000 -ftime-report -DENCHANTUM_CHECK_OUT_OF_BOUNDS_BY
```

</details>


## Skipped for memory limit

- small_enums_large_range — magic_enum: Process memory exceeded the 6618.4 MiB budget (95% of 13933.4 MiB available divided across 2 parallel jobs); stopped this target and continued.
