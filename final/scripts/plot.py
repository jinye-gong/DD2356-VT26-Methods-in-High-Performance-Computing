#!/usr/bin/env python3
"""Plot speedup and parallel efficiency from a strong-scaling CSV.

The CSV must have a header "<unit>,seconds" where <unit> is `threads` or `ranks`.
Usage:  python plot.py results/strong_omp.csv [out.png]
"""

import csv
import sys
from pathlib import Path


def main() -> int:
    if len(sys.argv) < 2:
        print("usage: plot.py <csv> [out.png]", file=sys.stderr)
        return 1

    csv_path = Path(sys.argv[1])
    out_path = Path(sys.argv[2]) if len(sys.argv) > 2 else csv_path.with_suffix(".png")

    with csv_path.open() as f:
        rdr = csv.reader(f)
        header = next(rdr)
        unit   = header[0]
        rows   = [(int(r[0]), float(r[1])) for r in rdr if r and r[0]]
    rows.sort()
    ps   = [r[0] for r in rows]
    secs = [r[1] for r in rows]
    if not rows:
        print("empty csv", file=sys.stderr); return 1

    t1      = secs[0]
    speedup = [t1 / s for s in secs]
    eff     = [sp / p for sp, p in zip(speedup, ps)]

    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
    except ImportError:
        print("matplotlib not installed; printing tables only.")
        print(f"{unit}\tseconds\tspeedup\tefficiency")
        for p, s, sp, e in zip(ps, secs, speedup, eff):
            print(f"{p}\t{s:.4f}\t{sp:.2f}\t{e:.2f}")
        return 0

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 4))
    ax1.plot(ps, speedup, "o-", label="measured")
    ax1.plot(ps, ps,      "--", label="ideal")
    ax1.set_xlabel(unit); ax1.set_ylabel("speedup"); ax1.set_title("Strong scaling")
    ax1.legend(); ax1.grid(True, alpha=0.3)

    ax2.plot(ps, eff, "s-")
    ax2.axhline(1.0, ls="--", c="grey")
    ax2.set_ylim(0, 1.1); ax2.set_xlabel(unit); ax2.set_ylabel("efficiency")
    ax2.set_title("Parallel efficiency"); ax2.grid(True, alpha=0.3)

    fig.tight_layout(); fig.savefig(out_path, dpi=160)
    print(f"[plot] wrote {out_path}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
