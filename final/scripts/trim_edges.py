#!/usr/bin/env python3
"""
Trim a SNAP-like directed edge list to the first N edges.

Input format:
  - one edge per line: "src dst" (whitespace separated)
  - comment lines starting with '#' or '%' are ignored

This is a pragmatic helper for weak-scaling experiments when you don't
have a native graph generator: for rank-count r, create an input with
edges_per_rank * r edges so work scales roughly linearly.
"""

import argparse
import sys
from pathlib import Path


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("input_edges")
    ap.add_argument("output_edges")
    ap.add_argument("--edges", type=int, required=True, help="number of edges to keep")
    args = ap.parse_args()

    inp = Path(args.input_edges)
    out = Path(args.output_edges)
    keep = args.edges

    n_written = 0
    out.parent.mkdir(parents=True, exist_ok=True)

    with inp.open("r") as fin, out.open("w") as fout:
        for line in fin:
            line = line.strip()
            if not line or line[0] in "#%":
                continue
            # Expect: src dst
            parts = line.split()
            if len(parts) < 2:
                continue
            fout.write(f"{parts[0]} {parts[1]}\n")
            n_written += 1
            if n_written >= keep:
                break

    print(f"[trim_edges] wrote {n_written} edges -> {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

