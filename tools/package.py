#!/usr/bin/env python3

import argparse
import shutil
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

DEFAULT_OUTPUT = ROOT / "dist" / "WearableDevices.zip"


def build(output: str | Path):
    stage = ROOT / "dist" / "stage"
    if stage.exists():
        shutil.rmtree(stage)
    core = stage / "src" / "core" / "gamedata"
    core.parent.mkdir(parents=True, exist_ok=True)
    shutil.copytree(ROOT / "gamedata", core)
    options = ROOT / "options"
    if options.is_dir():
        for option in sorted(options.iterdir()):
            shutil.copytree(option / "gamedata", stage / "src" / option.name / "gamedata")
    shutil.copytree(ROOT / "fomod", stage / "fomod")
    shutil.copy2(ROOT / "README.md", stage / "README.md")

    output = Path(output)
    output.parent.mkdir(parents=True, exist_ok=True)
    if output.exists():
        output.unlink()
    with zipfile.ZipFile(output, "w", zipfile.ZIP_DEFLATED) as zf:
        for path in sorted(stage.rglob("*")):
            if path.is_file():
                zf.write(path, path.relative_to(stage))
    shutil.rmtree(stage)
    print(f"built {output}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", default=str(DEFAULT_OUTPUT))
    build(parser.parse_args().output)
