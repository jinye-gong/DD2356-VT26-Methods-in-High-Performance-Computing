#!/usr/bin/env bash
# Fetch a SNAP directed graph and decompress it under data/.
# Usage: ./download.sh <web-Stanford|web-Google|web-BerkStan|soc-LiveJournal1>

set -euo pipefail
cd "$(dirname "$0")"

name="${1:-web-Stanford}"
case "$name" in
  web-Stanford)      url="https://snap.stanford.edu/data/web-Stanford.txt.gz" ;;
  web-Google)        url="https://snap.stanford.edu/data/web-Google.txt.gz" ;;
  web-BerkStan)      url="https://snap.stanford.edu/data/web-BerkStan.txt.gz" ;;
  soc-LiveJournal1)  url="https://snap.stanford.edu/data/soc-LiveJournal1.txt.gz" ;;
  *) echo "Unknown dataset: $name" >&2; exit 1 ;;
esac

out="${name}.txt"
if [[ -f "$out" ]]; then
  echo "[data] $out already present, skipping."
  exit 0
fi

echo "[data] fetching $url ..."
curl -L "$url" -o "${name}.txt.gz"
gunzip -f "${name}.txt.gz"
echo "[data] wrote $(pwd)/$out"
