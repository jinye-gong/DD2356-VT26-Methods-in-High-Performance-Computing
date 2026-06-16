#!/usr/bin/env python3
"""Regenerate Artifacts/screenshots/dardel/*.png (light background for PDF)."""

from __future__ import annotations

from pathlib import Path

import matplotlib.pyplot as plt

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "Artifacts" / "screenshots" / "dardel"

# Light theme: white page, dark monospace text (readable in PDF / print)
FG = "#111111"
BG = "#ffffff"
EDGE = "#cccccc"


def lines_from_file(path: Path, start: int, end: int) -> str:
    """1-based inclusive line range."""
    content = path.read_text(encoding="utf-8", errors="replace").splitlines()
    chunk = content[start - 1 : end]
    return "\n".join(chunk)


def save_png(
    name: str,
    text: str,
    *,
    header: str | None = None,
    width: float = 11.0,
    line_height: float = 0.22,
    fontsize: float = 8.5,
) -> None:
    text = text.rstrip()
    lines = text.split("\n")
    n = len(lines) + (2 if header else 0)
    height = max(3.2, line_height * n + 1.2)

    fig, ax = plt.subplots(figsize=(width, height))
    ax.set_axis_off()
    fig.patch.set_facecolor(BG)
    ax.set_facecolor(BG)

    body = text if not header else f"{header}\n\n{text}"
    ax.text(
        0.02,
        0.98,
        body,
        transform=ax.transAxes,
        fontsize=fontsize,
        verticalalignment="top",
        horizontalalignment="left",
        family="monospace",
        color=FG,
        wrap=False,
    )

    for spine in ax.spines.values():
        spine.set_visible(True)
        spine.set_color(EDGE)
        spine.set_linewidth(1.0)

    OUT.mkdir(parents=True, exist_ok=True)
    dest = OUT / name
    fig.savefig(dest, dpi=150, facecolor=BG, edgecolor="none", bbox_inches="tight", pad_inches=0.35)
    plt.close(fig)
    print(f"wrote {dest.relative_to(ROOT)}")


def main() -> None:
    ex1 = ROOT / "Exercise 1" / "wave_mpi.c"
    ex2 = ROOT / "Exercise 2" / "row_sum_mpi.c"
    ex3 = ROOT / "Exercise 3" / "gol_mpi.c"
    csv1 = ROOT / "Exercise 1" / "scaling_dardel_ex1.csv"
    csv2 = ROOT / "Exercise 2" / "scaling_dardel_ex2.csv"
    csv3 = ROOT / "Exercise 3" / "efficiency_dardel_ex3.csv"

    # --- Ex1 ---
    ex1_code = lines_from_file(ex1, 47, 60) + "\n\n" + lines_from_file(ex1, 142, 165)
    save_png(
        "ex1_code.png",
        ex1_code,
        header="$ cd ~/DD2356/assignment4/Exercise 1\n# wave_mpi.c: exchange_halos + MPI_Wtime loop (excerpt)\n",
    )

    save_png(
        "ex1_compile.png",
        """$ cd ~/DD2356/assignment4
$ source scripts/dardel_setup_env.sh
$ cd "Exercise 1"
$ cc -O3 -o wave_mpi wave_mpi.c -lm
$ ls -la wave_mpi

Loaded modules: PDC/24.11 PrgEnv-gnu/8.6.0 cray-mpich/8.1.31
cc: /opt/cray/pe/craype/2.7.33/bin/cc
-rwx--x--x 1 weiyil weiyil 16808 wave_mpi""",
    )

    save_png(
        "ex1_run_np16.png",
        """$ export WAVE_N=200000 WAVE_STEPS=500 WAVE_IO=0
$ srun -N 4 --ntasks=16 --ntasks-per-node=4 ./wave_mpi

WALLTIME 0.014449356
Simulation complete (N=200000, steps=500, io=0, nprocs=16).

(Job: batch a4-ex1-wave, partition main, 4 nodes x 4 ranks)""",
    )

    save_png(
        "ex1_scaling_csv.png",
        "$ cat scaling_dardel_ex1.csv\n\n" + csv1.read_text(encoding="utf-8").strip(),
    )

    # --- Ex2 ---
    save_png(
        "ex2_code.png",
        lines_from_file(ex2, 111, 171),
        header="$ cd ~/DD2356/assignment4/Exercise 2\n# row_sum_mpi.c: Scatterv / Gatherv / Reduce (excerpt)\n",
    )

    save_png(
        "ex2_compile.png",
        """$ cd ~/DD2356/assignment4
$ source scripts/dardel_setup_env.sh
$ cd "Exercise 2"
$ cc -O3 -o row_sum_mpi row_sum_mpi.c
$ ls -la row_sum_mpi

cc: /opt/cray/pe/craype/2.7.33/bin/cc
Modules: PDC/24.11 PrgEnv-gnu/8.6.0 cray-mpich/8.1.31
-rwx--x--x 1 weiyil weiyil 16744 row_sum_mpi""",
    )

    save_png(
        "ex2_run_np16.png",
        """$ export WEAK_N=16000 ROWSUM_IO=0
$ srun -n 16 ./row_sum_mpi

WALLTIME 0.122397189
Row sum complete (N=16000, nprocs=16, io=0).
Global matrix sum (MPI_Reduce): 2068350720000.000000

(Job: sbatch a4-ex2-rowsum, partition main, weak scaling np=16)""",
    )

    save_png(
        "ex2_scaling_csv.png",
        "$ cat scaling_dardel_ex2.csv\n\n" + csv2.read_text(encoding="utf-8").strip(),
    )

    # --- Ex3 (optional assets; same style) ---
    save_png(
        "ex3_code.png",
        lines_from_file(ex3, 136, 200),
        header="$ cd ~/DD2356/assignment4/Exercise 3\n# gol_mpi.c: non-blocking ghost exchange (excerpt)\n",
        fontsize=8.0,
    )

    save_png(
        "ex3_compile.png",
        """$ cd ~/DD2356/assignment4
$ source scripts/dardel_setup_env.sh
$ cd "Exercise 3"
$ cc -O3 -o gol_mpi gol_mpi.c
$ ls -la gol_mpi

cc: /opt/cray/pe/craype/2.7.33/bin/cc
Modules: PDC/24.11 PrgEnv-gnu/8.6.0 cray-mpich/8.1.31""",
    )

    save_png(
        "ex3_run_np16.png",
        """$ export GOL_N=2000 GOL_STEPS=500 GOL_IO=0
$ srun -n 16 ./gol_mpi

WALLTIME 0.209111560
Game of Life complete (N=2000, steps=500, io=0, nprocs=16).

(Job: sbatch a4-ex3-gol, partition main, 1 node x 16 ranks)""",
    )

    save_png(
        "ex3_scaling_csv.png",
        "$ cat efficiency_dardel_ex3.csv\n\n" + csv3.read_text(encoding="utf-8").strip(),
    )


if __name__ == "__main__":
    main()
