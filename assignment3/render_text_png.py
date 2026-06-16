import argparse
from pathlib import Path

import matplotlib.pyplot as plt


def render_text_to_png(input_path: Path, output_path: Path, title: str) -> None:
    text = input_path.read_text(encoding="utf-8", errors="replace")
    lines = text.splitlines() if text else [""]

    max_len = max((len(line) for line in lines), default=1)
    fig_w = min(24, max(10, max_len * 0.11))
    fig_h = max(4, len(lines) * 0.33 + 1.8)

    fig, ax = plt.subplots(figsize=(fig_w, fig_h), dpi=150)
    fig.patch.set_facecolor("#14161a")
    ax.set_facecolor("#14161a")
    ax.axis("off")

    display_text = text if text else "(empty)"
    if title:
        display_text = f"{title}\n{'-' * max(12, len(title))}\n{display_text}"

    ax.text(
        0.01,
        0.99,
        display_text,
        va="top",
        ha="left",
        family="monospace",
        fontsize=10,
        color="#e6e6e6",
        transform=ax.transAxes,
    )

    output_path.parent.mkdir(parents=True, exist_ok=True)
    fig.tight_layout(pad=0.3)
    fig.savefig(output_path)
    plt.close(fig)


def main() -> None:
    parser = argparse.ArgumentParser(description="Render text file to png")
    parser.add_argument("--input", required=True, help="Input text file path")
    parser.add_argument("--output", required=True, help="Output png path")
    parser.add_argument("--title", default="", help="Optional title")
    args = parser.parse_args()

    render_text_to_png(Path(args.input), Path(args.output), args.title)
    print(f"Saved: {args.output}")


if __name__ == "__main__":
    main()
