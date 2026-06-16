import argparse
import numpy as np
import matplotlib.pyplot as plt


def main():
    parser = argparse.ArgumentParser(description="Plot neuron simulation outputs")
    parser.add_argument("--activity", default="neuron_output.txt", help="Path to neuron_output.txt")
    parser.add_argument("--summary", default="neuron_summary.txt", help="Path to neuron_summary.txt")
    parser.add_argument("--scatter-out", default="neuron_activity_scatter.png", help="Output scatter png")
    parser.add_argument("--hist-out", default="neuron_firing_hist.png", help="Output histogram png")
    args = parser.parse_args()

    data = np.loadtxt(args.activity)
    steps = data[:, 0]
    neurons = data[:, 1]
    potentials = data[:, 2]

    plt.figure(figsize=(10, 5))
    plt.scatter(steps, neurons, c=potentials, cmap="coolwarm", s=1)
    plt.colorbar(label="Membrane Potential")
    plt.xlabel("Time Step")
    plt.ylabel("Neuron ID")
    plt.title("Neuron Activity Over Time")
    plt.tight_layout()
    plt.savefig(args.scatter_out, dpi=200)

    fire_counts = []
    with open(args.summary, "r", encoding="utf-8") as f:
        for line in f:
            fire_counts.append(int(line.strip().split()[-2]))
    fire_counts = np.array(fire_counts)

    plt.figure(figsize=(8, 5))
    plt.hist(fire_counts, bins=20, color="blue", edgecolor="black")
    plt.xlabel("Firing Count")
    plt.ylabel("Number of Neurons")
    plt.title("Distribution of Neuron Firings")
    plt.tight_layout()
    plt.savefig(args.hist_out, dpi=200)

    print(f"Saved: {args.scatter_out}")
    print(f"Saved: {args.hist_out}")


if __name__ == "__main__":
    main()
