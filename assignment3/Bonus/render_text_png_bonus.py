import argparse
from pathlib import Path

import matplotlib.pyplot as plt


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", required=True)
    parser.add_argument("--output", required=True)
    parser.add_argument("--title", default="")
    args = parser.parse_args()

    txt = Path(args.input).read_text(encoding="utf-8", errors="replace")
    lines = txt.splitlines() or [""]
    max_len = max((len(x) for x in lines), default=1)

    fig_w = min(24, max(10, max_len * 0.11))
    fig_h = max(4, len(lines) * 0.33 + 1.8)
    fig, ax = plt.subplots(figsize=(fig_w, fig_h), dpi=150)
    fig.patch.set_facecolor("#14161a")
    ax.set_facecolor("#14161a")
    ax.axis("off")

    text = txt if txt else "(empty)"
    if args.title:
        text = f"{args.title}\n{'-' * max(10, len(args.title))}\n{text}"
    ax.text(0.01, 0.99, text, va="top", ha="left", family="monospace", fontsize=10, color="#e6e6e6", transform=ax.transAxes)

    out = Path(args.output)
    out.parent.mkdir(parents=True, exist_ok=True)
    fig.tight_layout(pad=0.3)
    fig.savefig(out)
    plt.close(fig)
    print(f"Saved: {out}")


if __name__ == "__main__":
    main()
