#!/usr/bin/env python3
"""Plot parallel efficiency from efficiency_*.csv"""

import argparse
import pandas as pd
import matplotlib.pyplot as plt


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--csv", required=True)
    parser.add_argument("--title", default="Parallel Efficiency")
    parser.add_argument("--out", default="efficiency.png")
    args = parser.parse_args()

    df = pd.read_csv(args.csv).sort_values("nprocs")
    plt.figure(figsize=(8, 5))
    plt.plot(df["nprocs"], df["efficiency"], marker="o", label="Efficiency")
    plt.axhline(1.0, linestyle="--", color="gray", label="Ideal (1.0)")
    plt.xlabel("MPI processes")
    plt.ylabel("Efficiency")
    plt.title(args.title)
    plt.grid(True, linestyle=":", alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(args.out, dpi=200)
    print(f"Saved: {args.out}")


if __name__ == "__main__":
    main()
