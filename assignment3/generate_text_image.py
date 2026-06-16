import argparse
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


def render_text_to_png(text: str, out_path: Path, title: str = "") -> None:
    lines = text.splitlines() or [""]
    if title:
        lines = [title, "-" * max(10, len(title))] + lines

    try:
        font = ImageFont.truetype("DejaVuSansMono.ttf", 20)
    except Exception:
        font = ImageFont.load_default()

    padding = 20
    line_height = 28
    max_chars = max(len(line) for line in lines) if lines else 1
    width = min(2400, max(900, padding * 2 + max_chars * 12))
    height = max(400, padding * 2 + line_height * (len(lines) + 1))

    img = Image.new("RGB", (width, height), color=(20, 22, 26))
    draw = ImageDraw.Draw(img)

    y = padding
    for i, line in enumerate(lines):
        color = (220, 220, 220)
        if i == 0 and title:
            color = (120, 200, 255)
        draw.text((padding, y), line, font=font, fill=color)
        y += line_height

    out_path.parent.mkdir(parents=True, exist_ok=True)
    img.save(out_path)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", required=True, help="Input text file")
    parser.add_argument("--output", required=True, help="Output png file")
    parser.add_argument("--title", default="", help="Optional title")
    args = parser.parse_args()

    text = Path(args.input).read_text(encoding="utf-8", errors="replace")
    render_text_to_png(text, Path(args.output), args.title)
    print(f"Saved: {args.output}")


if __name__ == "__main__":
    main()
