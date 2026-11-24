#!/usr/bin/env python3
"""Run clang-tidy in parallel on the VTX sources listed in a compile_commands.json, without compiling them."""

import argparse
import json
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-p", "--build-dir", required=True, help="Directory containing compile_commands.json")
    parser.add_argument("--clang-tidy", default="clang-tidy", help="clang-tidy executable")
    parser.add_argument("--root", default=os.getcwd(), help="Repository root; regexes match paths relative to it")
    parser.add_argument("--include", default=r"^(lib|tool|app)/", help="Regex of files to analyze (relative path)")
    # Test targets are configured with NO_LINTER: keep them out of the analysis.
    parser.add_argument(
        "--exclude", default=r"(^|/)(vendor|build|test)/", help="Regex of files to skip (relative path)"
    )
    parser.add_argument("-j", "--jobs", type=int, default=os.cpu_count() or 1)
    args = parser.parse_args()

    with open(os.path.join(args.build_dir, "compile_commands.json"), encoding="utf-8") as f:
        database = json.load(f)

    root = os.path.abspath(args.root)
    include = re.compile(args.include)
    exclude = re.compile(args.exclude)

    def selected(path):
        if not path.endswith((".cpp", ".cc", ".cxx")):
            return False
        try:
            relative = os.path.relpath(path, root)
        except ValueError:  # Different drive on Windows.
            return False
        relative = relative.replace(os.sep, "/")
        if relative.startswith("../"):
            return False
        return include.search(relative) is not None and exclude.search(relative) is None

    files = sorted(
        {
            path
            for path in (os.path.abspath(os.path.join(entry["directory"], entry["file"])) for entry in database)
            if selected(path)
        }
    )
    print(f"clang-tidy: {len(files)} files, {args.jobs} jobs", flush=True)
    if not files:
        print("clang-tidy: no file selected, check --root / --include / --exclude", flush=True)
        return 1

    def run(path):
        result = subprocess.run(
            [args.clang_tidy, "-p", args.build_dir, "--quiet", path],
            capture_output=True,
            text=True,
            errors="replace",
        )
        return path, result.returncode, result.stdout + result.stderr

    failures = 0
    with ThreadPoolExecutor(max_workers=args.jobs) as executor:
        for index, (path, returncode, output) in enumerate(executor.map(run, files), start=1):
            print(f"[{index}/{len(files)}] {path}", flush=True)
            if output.strip():
                print(output.rstrip(), flush=True)
            if returncode != 0:
                failures += 1

    if failures:
        print(f"clang-tidy failed on {failures} file(s)", flush=True)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
