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
expected = ["app","asw","common","bsw/drivers","bsw/cdd","bsw/internal","bsw/board","bsw/mcal","bsw/services","startup","tests/target"]
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
# 以实际仓库文件覆盖率替代固定文件数，不再漏掉新增的 .c/.h。
# tests/host 与官方 SDK 不属于业务源码清单；其构建由独立目标管理。
first_party_roots = [
    firmware / name for name in
    ("app", "asw", "common", "bsw", "startup", "tests/target")
]
expected_first_party = set()
for root in first_party_roots:
    for path in root.rglob("*"):
        if not path.is_file() or path.suffix not in (".c", ".h", ".S"):
            continue
        if "sdk" in path.relative_to(firmware).parts:
            continue
        expected_first_party.add(path.resolve())

missing_from_keil = expected_first_party.difference(refs)
if missing_from_keil:
    fail("first-party source files missing from Keil: " +
         ", ".join(str(p.relative_to(firmware)) for p in sorted(missing_from_keil)))

unexpected_first_party = [
    p for p in refs if p.relative_to(firmware).parts[:2] == ("tests", "host")
]
if unexpected_first_party:
    fail("host-only test sources must not enter Keil target")

for node in project.findall(".//TargetArmAds/Cads/VariousControls/IncludePath"):
    for entry in (node.text or "").split(";"):
        if entry.strip():
            if not (project_dir / entry.replace("\\", "/")).is_dir():
                fail(f"missing include directory: {entry}")

scatter = project.findtext(".//TargetArmAds/LDads/ScatterFile")
if not scatter or not (project_dir / scatter.replace("\\", "/")).is_file():
    fail("missing Scatter file")

for name in ["app/main.c", "app/app.c", "asw/motion_sample.c",
             "app/app_scheduler.c", "asw/brake_intent.c",
             "asw/motion_signal.c", "asw/diagnostics.c",
             "asw/run_qualification.c", "asw/output_authorization.c",
             "bsw/drivers/drv_can_frame.c",
             "bsw/mcal/ytm32b1le0/drv_gpio_ytm32b1le0.c",
             "bsw/board/ytm32b1le0/board_support.c",
             "bsw/mcal/ytm32b1le0/mcal_time.c",
             "bsw/services/time_service.c",
             "startup/ytm32b1le0/startup.S",
             "startup/ytm32b1le0/vector.S",
             "bsw/mcal/ytm32b1le0/sdk/platform/devices/YTM32B1LE0/startup/system_YTM32B1LE0.c",
             "tests/target/gpio_bringup.c",
             "tests/target/test_gpio.c",
             "tests/target/test_timebase.c"]:
    if (firmware / name).resolve() not in refs:
        fail(f"mandatory source not referenced: {name}")

if not (firmware / "bsw/board/ytm32b1le0/generated/Gpio_Interrupt_Demo.yct").is_file():
    fail("original Board YCT reference missing")

# 校验厂商内部 #include 也必须可通过 Keil include search path 找到。
# 该依赖曾在迁移 system_YTM32B1LE0.h 后造成 21 个 Target 编译错误。
vendor_devices = (firmware / "bsw/mcal/ytm32b1le0/sdk/platform/devices").resolve()
vendor_header = vendor_devices / "device_registers.h"
required_header = "YTM32B1LE0/startup/system_YTM32B1LE0.h"
if not vendor_header.is_file() or required_header not in vendor_header.read_text(encoding="utf-8"):
    fail("unexpected vendor device_registers.h include contract")
include_roots = [
    (project_dir / entry.replace("\\", "/")).resolve()
    for node in project.findall(".//TargetArmAds/Cads/VariousControls/IncludePath")
    for entry in (node.text or "").split(";") if entry.strip()
]
if not any((root / required_header).is_file() for root in include_roots):
    fail(f"vendor header include cannot resolve: {required_header}")

print(f"PASS: {len(refs)} source references, directories, scatter and target metadata")
print("NOTE: This does not run Keil, flash or board tests")
