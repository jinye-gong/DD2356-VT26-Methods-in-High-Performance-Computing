#!/usr/bin/env python3
"""Render Slurm .out logs as PNG for embedding in Markdown reports."""
from pathlib import Path

import matplotlib.pyplot as plt


def render(path_in: Path, path_out: Path) -> None:
    text = path_in.read_text(encoding="utf-8")
    nlines = max(1, text.count("\n") + 1)
    fig_h = min(22.0, max(6.0, 0.18 * nlines))
    fig = plt.figure(figsize=(11, fig_h))
    fig.patch.set_facecolor("white")
    plt.axis("off")
    plt.text(
        0.02,
        0.98,
        text,
        transform=fig.transFigure,
        fontsize=8,
        va="top",
        ha="left",
        family="monospace",
        color="black",
    )
    plt.savefig(path_out, dpi=140, bbox_inches="tight", facecolor="white")
    plt.close()
    print("Wrote", path_out)


def main() -> None:
    base = Path(__file__).resolve().parent
    render(base / "dardel_strong_19763955.out", base / "figure_dardel_strong_scaling.png")
    render(base / "dardel_weak_19763960.out", base / "figure_dardel_weak_scaling.png")


if __name__ == "__main__":
    main()
