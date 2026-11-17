#!/usr/bin/env python3
"""Keil/architecture metadata checks, NOT a Target compile or safety validation."""
from pathlib import Path
from xml.etree import ElementTree as ET
import sys

firmware = Path(__file__).resolve().parents[1]
project_dir = firmware / "target" / "ytm32b1le0" / "KEIL"
proj = ET.parse(project_dir / "BrakeSync.uvprojx")
board = firmware / "bsw" / "board" / "ytm32b1le0"
sdk = firmware / "bsw" / "mcal" / "ytm32b1le0" / "sdk"

def fail(message: str) -> None:
    print("FAIL:", message, file=sys.stderr)
    sys.exit(1)

if proj.findtext(".//TargetName") != "BrakeSync":
    fail("unexpected TargetName")
if proj.findtext(".//TargetCommonOption/Device") != "YTM32B1LE05":
    fail("unexpected Target MCU")
if proj.findtext(".//TargetCommonOption/OutputName") != "BrakeSync":
    fail("unexpected OutputName")

# 防止 Keil 出现一套与代码物理分层不同的临时逻辑目录。
groups = proj.findall(".//Group")
if [group.findtext("GroupName") for group in groups] != ["asw", "bsw"]:
    fail("Keil project groups must be asw / bsw only")

for group in groups:
    logical_name = group.findtext("GroupName")
    for item in group.findall("./Files/File/FilePath"):
        if not item.text:
            fail("empty file reference in group")
        absolute = (project_dir / item.text.replace("\\", "/")).resolve()
        expected_prefix = firmware / logical_name.lower()
        if not absolute.is_relative_to(expected_prefix.resolve()):
            fail(f"Keil group {logical_name} references outside physical {logical_name}: {item.text}")

sources = []
for node in proj.findall(".//Group/Files/File/FilePath"):
    if not node.text:
        fail("empty Keil project source path")
    path = (project_dir / node.text.replace("\\", "/")).resolve()
    if not path.is_relative_to(firmware.resolve()):
        fail(f"source escapes firmware: {node.text}")
    if not path.is_file():
        fail(f"missing source: {node.text}")
    if path.is_relative_to((firmware / "tests" / "host").resolve()):
        fail(f"Host mock is linked into Keil: {node.text}")
    sources.append(path)

if len(sources) != len(set(sources)):
    fail("duplicate Keil source references")

for node in proj.findall(".//TargetArmAds/Cads/VariousControls/IncludePath"):
    for inc in (node.text or "").split(";"):
        if inc.strip():
            path = (project_dir / inc.replace("\\", "/")).resolve()
            if not path.is_dir():
                fail(f"missing include directory: {inc}")

scatter = proj.findtext(".//TargetArmAds/LDads/ScatterFile")
if not scatter or not (project_dir / scatter.replace("\\", "/")).resolve().is_file():
    fail("scatter file is missing")

required = [
    firmware / "asw" / "app" / "main.c",
    firmware / "asw" / "app" / "app.c",
    board / "board_support.c",
    firmware / "bsw" / "mcal" / "ytm32b1le0" / "mcal_time.c",
    firmware / "bsw" / "services" / "time_service.c",
    board / "bringup" / "gpio_bringup.c",
]
for item in required:
    if item.resolve() not in sources:
        fail(f"missing configured source: {item}")

if not any(path.is_relative_to(sdk.resolve()) for path in sources):
    fail("Keil project does not include vendor MCU SDK")

if not (board / "generated" / "Gpio_Interrupt_Demo.yct").is_file():
    fail("original YCT config missing")

print(f"PASS: Keil project metadata, {len(sources)} source paths, includes and scatter")
print("NOTE: no Keil compile, download or brake hardware test was run")
