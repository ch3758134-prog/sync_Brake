#!/usr/bin/env python3
"""Keil 路径、物理分组和链接元数据检查；不代表实际 Target 编译。"""

from pathlib import Path
from xml.etree import ElementTree as ET
import sys

firmware = Path(__file__).resolve().parents[1]
project_dir = firmware / "target" / "ytm32b1le0" / "KEIL"
project = ET.parse(project_dir / "BrakeSync.uvprojx")

def fail(message: str) -> None:
    print("FAIL:", message, file=sys.stderr)
    raise SystemExit(1)

if project.findtext(".//TargetName") != "BrakeSync":
    fail("TargetName changed")
if project.findtext(".//TargetCommonOption/Device") != "YTM32B1LE05":
    fail("Target MCU changed")
if project.findtext(".//TargetCommonOption/OutputName") != "BrakeSync":
    fail("OutputName changed")

groups = project.findall(".//Target/Groups/Group")
expected = ["app", "asw", "bsw/board", "bsw/mcal",
            "bsw/services", "startup", "tests/target"]
if [g.findtext("GroupName") for g in groups] != expected:
    fail("Keil groups differ from project source directories")

refs = []
for group in groups:
    group_dir = (firmware / group.findtext("GroupName")).resolve()
    for item in group.findall("./Files/File/FilePath"):
        if not item.text:
            fail("empty project file reference")
        path = (project_dir / item.text.replace("\\", "/")).resolve()
        if not path.is_relative_to(group_dir):
            fail(f"file outside matching group directory: {item.text}")
        if not path.is_file():
            fail(f"missing source: {item.text}")
        refs.append(path)

if len(refs) != len(set(refs)):
    fail("duplicate source reference")
if len(refs) != 97:
    fail(f"unexpected file count: {len(refs)}")

for node in project.findall(".//TargetArmAds/Cads/VariousControls/IncludePath"):
    for entry in (node.text or "").split(";"):
        if entry.strip():
            if not (project_dir / entry.replace("\\", "/")).is_dir():
                fail(f"missing include directory: {entry}")

scatter = project.findtext(".//TargetArmAds/LDads/ScatterFile")
if not scatter or not (project_dir / scatter.replace("\\", "/")).is_file():
    fail("missing Scatter file")

for name in ["app/main.c", "app/app.c", "asw/motion_sample.c",
             "bsw/board/ytm32b1le0/board_support.c",
             "bsw/mcal/ytm32b1le0/mcal_time.c",
             "bsw/services/time_service.c",
             "startup/ytm32b1le0/startup.S",
             "startup/ytm32b1le0/vector.S",
             "startup/ytm32b1le0/system_YTM32B1LE0.c",
             "tests/target/gpio_bringup.c"]:
    if (firmware / name).resolve() not in refs:
        fail(f"mandatory source not referenced: {name}")

if not (firmware / "bsw/board/ytm32b1le0/generated/Gpio_Interrupt_Demo.yct").is_file():
    fail("original Board YCT reference missing")

print(f"PASS: {len(refs)} source references, directories, scatter and target metadata")
print("NOTE: This does not run Keil, flash or board tests")
