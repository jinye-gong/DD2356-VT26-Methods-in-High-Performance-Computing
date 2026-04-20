#!/usr/bin/env python3
"""
Parse simple_roofline output and plot roofline curves.

Input format: lines like
  32, 16.250, 47.48, 367.49, 22.61, 8.125, 51.63, 168.96, 20.80, ...

Columns used:
  SP: Flops/byte (col 2), GFLOPS (col 4)
  DP: Flops/byte (col 6), GFLOPS (col 8)
  INT: Iops/byte (col 10), GIOPS (col 12)
"""

from __future__ import annotations

import argparse
import re
from dataclasses import dataclass
from pathlib import Path

import matplotlib.pyplot as plt


@dataclass
class RooflineData:
    x_sp: list[float]
    y_sp: list[float]
    x_dp: list[float]
    y_dp: list[float]
    x_int: list[float]
    y_int: list[float]


ROW_RE = re.compile(r"^\s*\d+\s*,")


def parse_roofline_output(path: Path) -> RooflineData:
    x_sp, y_sp = [], []
    x_dp, y_dp = [], []
    x_int, y_int = [], []

    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        if not ROW_RE.match(line):
            continue
        cols = [c.strip() for c in line.split(",")]
        if len(cols) < 13:
            continue

        # SP
        x_sp.append(float(cols[1]))
        y_sp.append(float(cols[3]))
        # DP
        x_dp.append(float(cols[5]))
        y_dp.append(float(cols[7]))
        # INT
        x_int.append(float(cols[9]))
        y_int.append(float(cols[11]))

    if not x_sp:
        raise ValueError(f"No data rows found in {path}")

    return RooflineData(x_sp, y_sp, x_dp, y_dp, x_int, y_int)


def sort_xy(x: list[float], y: list[float]) -> tuple[list[float], list[float]]:
    pairs = sorted(zip(x, y), key=lambda p: p[0])
    xs, ys = zip(*pairs)
    return list(xs), list(ys)


def plot_single_system(data: RooflineData, title: str, out_path: Path) -> None:
    x_sp, y_sp = sort_xy(data.x_sp, data.y_sp)
    x_dp, y_dp = sort_xy(data.x_dp, data.y_dp)
    x_int, y_int = sort_xy(data.x_int, data.y_int)

    plt.figure(figsize=(7.2, 5.2))
    plt.plot(x_sp, y_sp, "o-", label="Single precision (GFLOPS)", linewidth=2)
    plt.plot(x_dp, y_dp, "s-", label="Double precision (GFLOPS)", linewidth=2)
    plt.plot(x_int, y_int, "^-", label="Integer (GIOPS)", linewidth=2)

    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Operational Intensity (Flops/byte or Iops/byte)")
    plt.ylabel("Throughput (GFLOPS / GIOPS)")
    plt.title(title)
    plt.grid(True, which="both", linestyle="--", alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(out_path, dpi=160)
    plt.close()


def plot_compare_two(
    d1: RooflineData, d2: RooflineData, name1: str, name2: str, out_path: Path
) -> None:
    fig, axes = plt.subplots(1, 3, figsize=(14, 4.2), constrained_layout=True)

    for ax, x1, y1, x2, y2, title in [
        (axes[0], d1.x_sp, d1.y_sp, d2.x_sp, d2.y_sp, "Single precision"),
        (axes[1], d1.x_dp, d1.y_dp, d2.x_dp, d2.y_dp, "Double precision"),
        (axes[2], d1.x_int, d1.y_int, d2.x_int, d2.y_int, "Integer"),
    ]:
        sx1, sy1 = sort_xy(x1, y1)
        sx2, sy2 = sort_xy(x2, y2)
        ax.plot(sx1, sy1, "o-", linewidth=2, label=name1)
        ax.plot(sx2, sy2, "s--", linewidth=2, label=name2)
        ax.set_xscale("log")
        ax.set_yscale("log")
        ax.set_title(title)
        ax.set_xlabel("Intensity")
        ax.grid(True, which="both", linestyle="--", alpha=0.3)

    axes[0].set_ylabel("GFLOPS / GIOPS")
    axes[0].legend()
    plt.savefig(out_path, dpi=160)
    plt.close()


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", required=True, help="Input roofline text output")
    parser.add_argument("--title", required=True, help="Figure title")
    parser.add_argument("--output", required=True, help="Output PNG path")
    parser.add_argument("--compare-input", help="Second system output for comparison")
    parser.add_argument("--compare-name", default="School cluster")
    parser.add_argument("--base-name", default="Dardel")
    parser.add_argument("--compare-output", help="Comparison output PNG")
    args = parser.parse_args()

    data = parse_roofline_output(Path(args.input))
    plot_single_system(data, args.title, Path(args.output))
    print(f"Wrote {args.output}")

    if args.compare_input and args.compare_output:
        data2 = parse_roofline_output(Path(args.compare_input))
        plot_compare_two(
            data, data2, args.base_name, args.compare_name, Path(args.compare_output)
        )
        print(f"Wrote {args.compare_output}")


if __name__ == "__main__":
    main()

