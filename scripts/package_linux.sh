#!/usr/bin/env bash
set -e; repo="${1:?}"; version="${2:?}"; cd "$repo"
make >/dev/null
mkdir -p dist
B="$(mktemp -d)"; trap 'rm -rf "$B"' EXIT
cp Detcalc "$B/Detcalc"
[ -f README.md ] && cp README.md "$B/"
[ -f PLAN.md ] && cp PLAN.md "$B/"
[ -f LICENSE ] && cp LICENSE "$B/"
tar czf "dist/Detcalc-${version}-linux.tar.gz" -C "$B" .
echo "OK dist/Detcalc-${version}-linux.tar.gz"
