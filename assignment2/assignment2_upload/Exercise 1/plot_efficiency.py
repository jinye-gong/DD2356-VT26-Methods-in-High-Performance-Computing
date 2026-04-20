#!/usr/bin/env python3
"""
Exercise 1 — Task 3 & 4: weak / strong scaling efficiency from measured runtimes.

Definitions (standard in HPC courses):
  Strong scaling efficiency: E_s(p) = T_1 / (p * T_p)  (same total work, p processes)
  Weak scaling efficiency:   E_w(p) = T_1 / T_p        (baseline T_1 at p=1, local work fixed per rank)

Runtimes from your experiments (seconds).
"""
from __future__ import annotations

import math

try:
    import matplotlib.pyplot as plt
except ImportError as e:
    raise SystemExit("Install matplotlib: pip install matplotlib") from e

PROCS = [1, 2, 4, 8, 16]

# Dardel (compute nodes, Slurm)
DARDEL_STRONG = [0.175691, 0.085037, 0.043196, 0.022678, 0.012897]
DARDEL_WEAK = [0.017239, 0.019204, 0.017522, 0.018024, 0.025563]

# Jupyter "DD2356 - Medium CPU only"
JUPYTER_STRONG = [0.275108, 0.137692, 0.074518, 0.059016, 0.028300]
JUPYTER_WEAK = [0.027189, 0.027208, 0.032771, 0.048876, 0.048557]


def strong_efficiency(times: list[float]) -> list[float]:
    t1 = times[0]
    return [t1 / (p * t) for p, t in zip(PROCS, times)]


def weak_efficiency(times: list[float]) -> list[float]:
    t1 = times[0]
    return [t1 / t for t in times]


def main() -> None:
    ds = strong_efficiency(DARDEL_STRONG)
    js = strong_efficiency(JUPYTER_STRONG)
    dw = weak_efficiency(DARDEL_WEAK)
    jw = weak_efficiency(JUPYTER_WEAK)

    # Task 3 — weak scaling efficiency
    plt.figure(figsize=(7, 4.5))
    plt.plot(PROCS, dw, "o-", label="Dardel", linewidth=2, markersize=8)
    plt.plot(PROCS, jw, "s--", label="School cluster (Jupyter Medium CPU)", linewidth=2, markersize=8)
    plt.xscale("log", base=2)
    plt.xticks(PROCS, [str(p) for p in PROCS])
    plt.xlabel("Number of processes")
    plt.ylabel("Weak scaling efficiency")
    plt.ylim(0, max(1.05, max(dw + jw) * 1.05))
    plt.axhline(1.0, color="gray", linestyle=":", linewidth=1)
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.title("Task 3: Weak scaling efficiency (ideal = 1)")
    plt.tight_layout()
    out3 = "task3_weak_scaling_efficiency.png"
    plt.savefig(out3, dpi=150)
    plt.close()
    print("Wrote", out3)

    # Task 4 — strong scaling efficiency
    plt.figure(figsize=(7, 4.5))
    plt.plot(PROCS, ds, "o-", label="Dardel", linewidth=2, markersize=8)
    plt.plot(PROCS, js, "s--", label="School cluster (Jupyter Medium CPU)", linewidth=2, markersize=8)
    plt.xscale("log", base=2)
    plt.xticks(PROCS, [str(p) for p in PROCS])
    plt.xlabel("Number of processes")
    plt.ylabel("Strong scaling efficiency")
    plt.ylim(0, max(1.05, max(ds + js) * 1.05))
    plt.axhline(1.0, color="gray", linestyle=":", linewidth=1)
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.title("Task 4: Strong scaling efficiency (ideal = 1)")
    plt.tight_layout()
    out4 = "task4_strong_scaling_efficiency.png"
    plt.savefig(out4, dpi=150)
    plt.close()
    print("Wrote", out4)

    # Numeric table for report
    lines = [
        "Weak scaling efficiency E_w(p) = T_1 / T_p",
        f"{'p':>4} {'Dardel':>10} {'Jupyter':>10}",
    ]
    for p, a, b in zip(PROCS, dw, jw):
        lines.append(f"{p:>4} {a:>10.4f} {b:>10.4f}")
    lines.append("")
    lines.append("Strong scaling efficiency E_s(p) = T_1 / (p * T_p)")
    lines.append(f"{'p':>4} {'Dardel':>10} {'Jupyter':>10}")
    for p, a, b in zip(PROCS, ds, js):
        lines.append(f"{p:>4} {a:>10.4f} {b:>10.4f}")
    table_txt = "\n".join(lines) + "\n"
    with open("efficiency_table.txt", "w", encoding="utf-8") as f:
        f.write(table_txt)
    print("Wrote efficiency_table.txt")
    print(table_txt)

    obs = """Task 3 (weak scaling) — 1–2 sentences for the report:
Both systems stay near ideal efficiency at low process counts, but efficiency drops at higher p—especially on the school Jupyter node—due to MPI communication and contention on a shared medium-CPU environment; Dardel maintains higher weak-scaling efficiency at 16 processes.

Task 4 (strong scaling) — 1–2 sentences for the report:
Dardel shows strong-scaling efficiency close to or above ideal for small p (timing noise) and a gradual decline to ~0.85 at 16 processes; the school cluster degrades earlier (notably at 8–16 processes), consistent with overhead and resource sharing on a single interactive node.
"""
    with open("task3_task4_observations.txt", "w", encoding="utf-8") as f:
        f.write(obs)
    print("Wrote task3_task4_observations.txt")


if __name__ == "__main__":
    main()
