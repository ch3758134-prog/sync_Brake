#!/usr/bin/env python3
"""Read-only structural validation of BrakeSync's Keil project (not a Target build)."""
from __future__ import annotations

from pathlib import Path
from xml.etree import ElementTree
import sys

firmware_root = Path(__file__).resolve().parents[1]
target_dir = firmware_root / "target" / "ytm32b1le0"
project_dir = target_dir / "KEIL"
project_file = project_dir / "BrakeSync.uvprojx"
project = ElementTree.parse(project_file)

def fail(text: str) -> None:
    print("FAIL:", text, file=sys.stderr)
    sys.exit(1)

if project.findtext(".//TargetName") != "BrakeSync":
    fail("unexpected TargetName")
if project.findtext(".//TargetCommonOption/Device") != "YTM32B1LE05":
    fail("unexpected device")
if project.findtext(".//TargetCommonOption/OutputName") != "BrakeSync":
    fail("unexpected output file name")

files: list[Path] = []
for node in project.findall(".//Group/Files/File/FilePath"):
    if not node.text:
        fail("empty Keil file reference")
    ref = (project_dir / node.text.replace("\\", "/")).resolve()
    if not ref.is_relative_to(firmware_root.resolve()):
        fail(f"file path escapes firmware directory: {node.text}")
    if not ref.is_file():
        fail(f"missing project source: {node.text}")
    files.append(ref)

for node in project.findall(".//TargetArmAds/Cads/VariousControls/IncludePath"):
    for inc in (node.text or "").split(";"):
        if inc.strip():
            path = (project_dir / inc.replace("\\", "/")).resolve()
            if not path.is_dir():
                fail(f"missing include directory: {inc}")

required = [firmware_root / "app" / "main.c",
            firmware_root / "app" / "app.c",
            target_dir / "board" / "board_support.c"]
for item in required:
    if item not in files:
        fail(f"not compiled by Keil project: {item}")

if len(files) != len(set(files)):
    fail("duplicate Keil source entries")

print(f"PASS: {len(files)} file references, include paths and target metadata verified")
print("NOTE: This is NOT a Keil Target compile/Flash test")
