#!/usr/bin/env python3
"""Protect official Yuntu demo SDK/CMSIS/YCT/startup bytes against silent edits."""
import hashlib
import sys
from pathlib import Path

base = Path(__file__).resolve().parents[1] / "platform/ytm32b1le0"
manifest = base / "ORIGIN_GIT_BLOBS.tsv"
expected = {}
for line in manifest.read_text(encoding="utf-8").splitlines():
    if not line.strip() or line.startswith("#"):
        continue
    sha, rel = line.split("\t", 1)
    if rel in expected:
        raise SystemExit("FAIL duplicate origin path: " + rel)
    expected[rel] = sha
for directory in ("sdk", "config", "startup"):
    present = {p.relative_to(base).as_posix() for p in (base / directory).rglob("*") if p.is_file()}
    listed = {p for p in expected if p.startswith(directory + "/")}
    if present != listed:
        raise SystemExit("FAIL origin set mismatch in " + directory)
for rel, expected_sha in expected.items():
    data = (base / rel).read_bytes()
    git_sha = hashlib.sha1(f"blob {len(data)}\0".encode("ascii") + data).hexdigest()
    if git_sha != expected_sha:
        raise SystemExit("FAIL upstream blob changed: " + rel)
print(f"PASS: {len(expected)} Yuntu SDK/CMSIS/YCT/startup original Git blobs match")
