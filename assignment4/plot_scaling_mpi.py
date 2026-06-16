#!/usr/bin/env python3
"""Plot MPI strong or weak scaling from CSV (columns: nprocs, time_sec)."""

import argparse

import matplotlib.pyplot as plt
import pandas as pd


def main():
    parser = argparse.ArgumentParser(description="Plot MPI scaling with ideal line")
    parser.add_argument("--csv", required=True, help="CSV with nprocs,time_sec")
    parser.add_argument("--title", default="MPI Scaling", help="Plot title")
    parser.add_argument(
        "--mode",
        choices=["strong", "weak"],
        default="strong",
        help="strong: ideal time T1/p; weak: ideal time ~ T1",
    )
    parser.add_argument("--out", default="scaling.png", help="Output PNG")
    args = parser.parse_args()

    df = pd.read_csv(args.csv)
    if df.empty:
        raise ValueError("CSV is empty")

    df = df.sort_values("nprocs")
    t1 = float(df.loc[df["nprocs"].idxmin(), "time_sec"])

    if args.mode == "strong":
        df["ideal_time"] = t1 / df["nprocs"]
        ideal_label = "Ideal (strong)"
    else:
        df["ideal_time"] = t1
        ideal_label = "Ideal (weak, constant)"

    plt.figure(figsize=(8, 5))
    plt.plot(df["nprocs"], df["time_sec"], marker="o", label="Measured")
    plt.plot(df["nprocs"], df["ideal_time"], linestyle="--", label=ideal_label)
    plt.xlabel("MPI processes")
    plt.ylabel("Time (s)")
    plt.title(args.title)
    plt.grid(True, linestyle=":", alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(args.out, dpi=200)
    print(f"Saved plot: {args.out}")


if __name__ == "__main__":
    main()
