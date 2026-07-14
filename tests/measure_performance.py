#!/usr/bin/env python3

import argparse
import json
import pathlib
import statistics
import subprocess
import tempfile
import time


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Measure cppseed generation time")
    parser.add_argument("binary", type=pathlib.Path)
    parser.add_argument("--warmups", type=int, default=3)
    parser.add_argument("--runs", type=int, default=30)
    parser.add_argument("--threshold-seconds", type=float, default=1.0)
    parser.add_argument("--output", type=pathlib.Path, default=pathlib.Path("performance.json"))
    return parser.parse_args()


def percentile_95(values: list[float]) -> float:
    ordered = sorted(values)
    index = max(0, (95 * len(ordered) + 99) // 100 - 1)
    return ordered[index]


def generate(binary: pathlib.Path, root: pathlib.Path, name: str) -> float:
    start = time.perf_counter()
    result = subprocess.run(
        [str(binary), "new", name],
        cwd=root,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
        text=True,
        timeout=5,
        check=False,
    )
    elapsed = time.perf_counter() - start
    if result.returncode != 0:
        raise RuntimeError(f"cppseed failed with {result.returncode}: {result.stderr}")
    return elapsed


def main() -> int:
    arguments = parse_arguments()
    binary = arguments.binary.resolve()
    if arguments.warmups < 0 or arguments.runs < 1:
        raise ValueError("warmups must be non-negative and runs must be positive")

    with tempfile.TemporaryDirectory(prefix="cppseed-performance-") as temporary:
        root = pathlib.Path(temporary)
        for index in range(arguments.warmups):
            generate(binary, root, f"warmup-{index}")

        durations = [
            generate(binary, root, f"measurement-{index}")
            for index in range(arguments.runs)
        ]

    result = {
        "runs": arguments.runs,
        "warmups": arguments.warmups,
        "median_seconds": statistics.median(durations),
        "p95_seconds": percentile_95(durations),
        "maximum_seconds": max(durations),
        "threshold_seconds": arguments.threshold_seconds,
    }
    arguments.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(result, indent=2))
    return 0 if result["p95_seconds"] <= arguments.threshold_seconds else 1


if __name__ == "__main__":
    raise SystemExit(main())
