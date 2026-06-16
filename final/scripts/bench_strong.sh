#!/usr/bin/env bash
# Strong-scaling sweep for the OpenMP build.
# Usage: ./bench_strong.sh data/web-Google.txt [threads_list]
set -euo pipefail
cd "$(dirname "$0")/.."

INPUT=${1:-data/sample.edges}
THREADS=${2:-"1 2 4 8 16"}
OUT=results/strong_omp.csv

mkdir -p results
echo "threads,seconds" > "$OUT"
for t in $THREADS; do
  echo "[bench] OMP_NUM_THREADS=$t"
  out=$( OMP_NUM_THREADS=$t bin/pagerank_omp "$INPUT" /dev/null 2>&1 )
  sec=$( echo "$out" | grep -oE 'time=[0-9]+\.[0-9]+s' | head -1 | sed 's/time=//;s/s//' )
  echo "$t,$sec" >> "$OUT"
done
echo "[bench] wrote $OUT"
