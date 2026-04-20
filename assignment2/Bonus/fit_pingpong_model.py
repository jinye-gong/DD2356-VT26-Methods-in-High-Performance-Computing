#!/usr/bin/env python3
"""
Fit RTT(bytes) to linear model: RTT = alpha + beta * bytes
and plot measured vs predicted curves.
"""
from __future__ import annotations

import argparse
import csv
from pathlib import Path

import numpy as np
import matplotlib.pyplot as plt


def load_csv(path: Path) -> tuple[np.ndarray, np.ndarray]:
    sizes = []
    rtts = []
    with path.open("r", encoding="utf-8") as f:
        reader = csv.reader(f)
        for row in reader:
            if not row:
                continue
            if row[0].startswith("#") or row[0] == "bytes":
                continue
            sizes.append(float(row[0]))
            rtts.append(float(row[1]))
    return np.array(sizes), np.array(rtts)


def fit_linear(x: np.ndarray, y: np.ndarray) -> tuple[float, float, float]:
    # y = alpha + beta*x
    beta, alpha = np.polyfit(x, y, 1)
    y_hat = alpha + beta * x
    ss_res = np.sum((y - y_hat) ** 2)
    ss_tot = np.sum((y - np.mean(y)) ** 2)
    r2 = 1.0 - ss_res / ss_tot if ss_tot > 0 else 1.0
    return alpha, beta, r2


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", required=True, help="CSV file: bytes,rtt_us")
    parser.add_argument("--system", required=True, help="System name label")
    parser.add_argument("--output-prefix", required=True, help="Output file prefix")
    args = parser.parse_args()

    in_path = Path(args.input)
    sizes, rtts = load_csv(in_path)
    alpha, beta, r2 = fit_linear(sizes, rtts)
    pred = alpha + beta * sizes

    print(f"System: {args.system}")
    print(f"Model: RTT_us = {alpha:.6f} + {beta:.9f} * bytes")
    print(f"R^2: {r2:.6f}")

    model_txt = Path(f"{args.output_prefix}_model.txt")
    model_txt.write_text(
        f"System: {args.system}\n"
        f"RTT_us = {alpha:.6f} + {beta:.9f} * bytes\n"
        f"R^2 = {r2:.6f}\n",
        encoding="utf-8",
    )

    plt.figure(figsize=(7.5, 5.0))
    plt.plot(sizes, rtts, "o", label="Measured RTT")
    plt.plot(sizes, pred, "-", label="Predicted RTT (linear fit)")
    plt.xscale("log", base=2)
    plt.xlabel("Message size (bytes)")
    plt.ylabel("RTT (microseconds)")
    plt.title(f"{args.system}: Measured vs Predicted RTT")
    plt.grid(True, which="both", linestyle="--", alpha=0.3)
    plt.legend()
    plt.tight_layout()
    out_png = Path(f"{args.output_prefix}_rtt_fit.png")
    plt.savefig(out_png, dpi=160)
    plt.close()

    print(f"Wrote {model_txt}")
    print(f"Wrote {out_png}")


if __name__ == "__main__":
    main()

