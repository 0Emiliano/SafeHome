from __future__ import annotations

import os
from pathlib import Path
import re
import textwrap

os.environ.setdefault("MPLCONFIGDIR", str(Path(__file__).resolve().parent.parent / "SEpython" / ".matplotlib"))

import matplotlib
matplotlib.use("Agg")
from matplotlib.backends.backend_pdf import PdfPages
import matplotlib.pyplot as plt


ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "documentacion_rubrica_9_tareas.md"
OUTPUT = ROOT / "manual_usuario_safehome.pdf"


def clean_markdown_line(line: str, in_code: bool) -> str:
    if in_code:
        return line.rstrip()

    line = line.rstrip()
    line = re.sub(r"`([^`]+)`", r"\1", line)
    line = line.replace("**", "")
    line = line.replace("|", " | ")
    return line


def markdown_to_lines(markdown: str) -> list[str]:
    lines: list[str] = []
    in_code = False
    code_language = ""
    previous_blank = False

    for raw in markdown.splitlines():
        stripped = raw.strip()

        if stripped.startswith("```"):
            if not in_code:
                in_code = True
                code_language = stripped.replace("```", "").strip()
                if code_language == "mermaid":
                    lines.append("[Diagrama incluido en la version Markdown]")
                else:
                    if not previous_blank:
                        lines.append("")
                        previous_blank = True
            else:
                in_code = False
                code_language = ""
                if not previous_blank:
                    lines.append("")
                    previous_blank = True
            continue

        if in_code and code_language == "mermaid":
            continue

        cleaned = clean_markdown_line(raw, in_code)
        is_blank = not cleaned.strip()
        if is_blank and previous_blank:
            continue
        lines.append(cleaned)
        previous_blank = is_blank

    return lines


def style_for_line(line: str) -> tuple[int, str, float]:
    stripped = line.strip()
    if stripped.startswith("# "):
        return 17, stripped[2:], 0.46
    if stripped.startswith("## "):
        return 12, stripped[3:], 0.34
    if stripped.startswith("### "):
        return 9, stripped[4:], 0.25
    return 7, line, 0.19


def wrap_line(line: str, font_size: int) -> list[str]:
    stripped = line.strip()
    if not stripped:
        return [""]

    width = 104 if font_size <= 7 else 80
    if stripped.startswith("- "):
        wrapped = textwrap.wrap(stripped[2:], width=width - 4)
        return ["- " + wrapped[0]] + ["  " + item for item in wrapped[1:]] if wrapped else [line]

    if re.match(r"^\d+\.\s", stripped):
        wrapped = textwrap.wrap(stripped, width=width)
        return wrapped

    if " | " in stripped:
        return textwrap.wrap(stripped, width=112) or [stripped]

    return textwrap.wrap(stripped, width=width) or [stripped]


def render_pdf(lines: list[str]) -> None:
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    y = 10.15
    page = 1

    with PdfPages(OUTPUT) as pdf:
        fig, ax = new_page(page)

        for line in lines:
            font_size, printable, spacing = style_for_line(line)
            wrapped_lines = wrap_line(printable, font_size)

            needed = max(1, len(wrapped_lines)) * spacing + 0.03
            if y - needed < 0.45:
                pdf.savefig(fig)
                plt.close(fig)
                page += 1
                fig, ax = new_page(page)
                y = 10.15

            for idx, wrapped in enumerate(wrapped_lines):
                weight = "bold" if font_size >= 11 else "normal"
                family = "monospace" if line.startswith("    ") or line.startswith("\t") else "DejaVu Sans"
                ax.text(
                    0.45,
                    y,
                    wrapped,
                    fontsize=font_size,
                    fontweight=weight,
                    family=family,
                    va="top",
                    wrap=False,
                )
                y -= spacing

            if font_size >= 11:
                y -= 0.06

        pdf.savefig(fig)
        plt.close(fig)


def new_page(page: int):
    fig = plt.figure(figsize=(8.5, 11))
    ax = fig.add_axes([0, 0, 1, 1])
    ax.set_xlim(0, 8.5)
    ax.set_ylim(0, 11)
    ax.axis("off")
    ax.text(0.45, 10.62, "SafeHome - Manual de usuario", fontsize=8, color="#555555")
    ax.text(7.45, 0.28, f"Pagina {page}", fontsize=7, color="#555555")
    return fig, ax


def main() -> None:
    markdown = SOURCE.read_text(encoding="utf-8")
    lines = markdown_to_lines(markdown)
    render_pdf(lines)
    print(OUTPUT)


if __name__ == "__main__":
    main()
