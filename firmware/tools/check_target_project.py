#!/usr/bin/env python3
"""Keil path / vendor source ownership audit; NOT a Target build."""
from pathlib import Path
from xml.etree import ElementTree as ET
import sys
fw = Path(__file__).resolve().parents[1]
base = fw / "target/ytm32b1le0/KEIL"
x = ET.parse(base / "BrakeSync.uvprojx")
def die(s):
    print("FAIL: "+s, file=sys.stderr)
    raise SystemExit(1)
if x.findtext(".//TargetName") != "BrakeSync" or x.findtext(".//TargetCommonOption/Device") != "YTM32B1LE05":
    die("Target identification differs")
refs = set()
for group in x.findall(".//Target/Groups/Group"):
    name = group.findtext("GroupName") or ""
    for f in group.findall("./Files/File/FilePath"):
        q = (base / (f.text or "").replace("\\", "/")).resolve()
        if not q.is_file() or not q.is_relative_to(fw): die("missing or escaped ref "+str(f.text))
        if q in refs: die("duplicated ref "+str(f.text))
        refs.add(q)
        is_sdk = "/sdk/" in q.as_posix()
        if is_sdk != name.startswith(("40 Vendor","41 Vendor","42 Vendor","43 Vendor")):
            die("first-party and vendor group mixed "+str(f.text))
expected = set()
for d in (fw / "source", fw / "platform/ytm32b1le0", fw / "tests/target"):
    for f in d.rglob("*"):
        if f.is_file() and f.suffix in (".c",".h",".S"): expected.add(f.resolve())
if expected != refs: die("files not correctly covered: missing "+str(len(expected-refs))+" unexpected "+str(len(refs-expected)))
paths = [(base/p.replace("\\","/")).resolve() for el in x.findall(".//TargetArmAds/Cads/VariousControls/IncludePath") for p in (el.text or "").split(";") if p.strip()]
if not paths or any(not p.is_dir() for p in paths): die("invalid include paths")
for name in ("board_gpio_ids.h","drv_gpio.h","evb_app.h","sdk_project_config.h","mcal_time.h","bs_control_contract.h"):
    if not any((p/name).is_file() for p in paths): die("include unresolved: "+name)
hdr = fw / "platform/ytm32b1le0/sdk/platform/devices/device_registers.h"
vend = "YTM32B1LE0/startup/system_YTM32B1LE0.h"
if vend not in hdr.read_text(encoding="utf-8") or not any((p/vend).is_file() for p in paths):
    die("vendor internal header not resolvable")
sc = x.findtext(".//TargetArmAds/LDads/ScatterFile")
if not sc or not (base/sc.replace("\\","/")).is_file(): die("scatter missing")
if not (fw/"platform/ytm32b1le0/config/Gpio_Interrupt_Demo.yct").is_file(): die("YCT reference missing")
print(f"PASS: {len(refs)} Keil refs, SDK ownership, include and scatter")
print("NOTE: Keil ArmClang Build, Flash, Board Test NOT_RUN")
