#!/usr/bin/env bash
set -e; repo="${1:?}"; version="${2:?}"; cd "$repo"
x86_64-w64-mingw32-gcc -Iinclude -O2 -std=c11 -Wall -Wextra -Wpedantic main.c -o Detcalc.exe -lm \
  || x86_64-w64-mingw32-gcc -O2 -std=c11 main.c -o Detcalc.exe -lm
mkdir -p dist
zip -q "dist/Detcalc-${version}-windows.zip" Detcalc.exe
[ -f README.md ] && zip -q "dist/Detcalc-${version}-windows.zip" README.md 2>/dev/null || true
echo "OK dist/Detcalc-${version}-windows.zip"
