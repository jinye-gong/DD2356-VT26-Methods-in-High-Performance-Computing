import argparse
import pandas as pd
import matplotlib.pyplot as plt


def main():
    parser = argparse.ArgumentParser(description="Plot strong scaling with ideal line")
    parser.add_argument("--csv", required=True, help="Input CSV from run_scaling.sh")
    parser.add_argument("--title", default="Strong Scaling", help="Plot title")
    parser.add_argument("--out", default="strong_scaling.png", help="Output PNG filename")
    args = parser.parse_args()

    df = pd.read_csv(args.csv)
    if df.empty:
        raise ValueError("CSV is empty")

    df = df.sort_values("threads")
    t1 = float(df.loc[df["threads"].idxmin(), "time_sec"])
    df["ideal_time"] = t1 / df["threads"]

    plt.figure(figsize=(8, 5))
    plt.plot(df["threads"], df["time_sec"], marker="o", label="Measured")
    plt.plot(df["threads"], df["ideal_time"], linestyle="--", label="Ideal")
    plt.xlabel("Threads")
    plt.ylabel("Time (s)")
    plt.title(args.title)
    plt.grid(True, linestyle=":", alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(args.out, dpi=200)
    print(f"Saved plot: {args.out}")


if __name__ == "__main__":
    main()
