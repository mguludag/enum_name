import unittest

from run import clang_time_report, github_annotation


class ClangTimeReportTests(unittest.TestCase):
    def test_parses_frontend_report_from_current_clang(self):
        output = """\
0.0010 (100.0%)   0.0010 (100.0%)   0.0010 (100.0%)   0.0010 (100.0%)  Total

===-------------------------------------------------------------------------===
                          Clang front-end time report
===-------------------------------------------------------------------------===
  Total Execution Time: 0.8123 seconds (0.9456 wall clock)

   ---User Time---   --System Time--   --User+System--   ---Wall Time---  --- Name ---
   0.7000 (100.0%)   0.1123 (100.0%)   0.8123 (100.0%)   0.9456 (100.0%)  Clang front-end timer
   0.7000 (100.0%)   0.1123 (100.0%)   0.8123 (100.0%)   0.9456 (100.0%)  Total
"""
        metrics = clang_time_report(output)

        self.assertEqual(metrics["compiler_seconds"], 0.9456)
        self.assertEqual(metrics["frontend_seconds"], 0.9456)

    def test_parses_legacy_clang_report(self):
        output = """\
Clang time report
   ---User Time---   --System Time--   --User+System--   ---Wall Time---  --- Name ---
   0.4000 (100.0%)   0.1000 (100.0%)   0.5000 (100.0%)   0.6000 (100.0%)  Front end
   0.4000 (100.0%)   0.1000 (100.0%)   0.5000 (100.0%)   0.6000 (100.0%)  Total
"""
        metrics = clang_time_report(output)

        self.assertEqual(metrics["compiler_seconds"], 0.6)
        self.assertEqual(metrics["frontend_seconds"], 0.6)


class GithubAnnotationTests(unittest.TestCase):
    def test_formats_result_as_notice_and_escapes_workflow_data(self):
        annotation = github_annotation(
            {
                "case": "small_enums\n",
                "library": "enum_name",
                "status": "ok",
                "compiler_seconds": 1.25,
                "frontend_seconds": 1.0,
                "peak_rss_mib": 25.0,
            }
        )

        self.assertEqual(
            annotation,
            "::notice title=Compile-time benchmark::"
            "small_enums%0A / enum_name: ok; compiler=1.250s; "
            "frontend=1.000s; peak RSS=25.000 MiB",
        )

    def test_marks_skips_as_warnings_and_failures_as_errors(self):
        result = {
            "case": "small_enums",
            "library": "enum_name",
            "compiler_seconds": None,
            "frontend_seconds": None,
        }

        result["status"] = "skipped (memory limit)"
        self.assertTrue(github_annotation(result).startswith("::warning "))

        result["status"] = "compiler timing missing"
        self.assertTrue(github_annotation(result).startswith("::error "))


if __name__ == "__main__":
    unittest.main()
