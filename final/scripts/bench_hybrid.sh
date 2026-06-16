#!/usr/bin/env bash
# Hybrid MPI + OpenMP sweep under a fixed total core budget.
#
# Usage:
#   ./bench_hybrid.sh data/web-Google.txt 256 "1 2 4 8 16 32 64 128"
#
# Interpretation:
#   - TOTAL_CORES = N * P must equal 256
#   - for each N in ranks_list, P = TOTAL_CORES / N (must be integer)
#   - run: OMP_NUM_THREADS=$P MPI launcher with N ranks using pagerank_hybrid
#
# Output:
#   results/hybrid_sweep.csv columns:
#     ranks,threads,total_cores,seconds
#
set -euo pipefail
cd "$(dirname "$0")/.."

INPUT=${1:-data/sample.edges}
TOTAL_CORES=${2:-64}
RANKS_LIST=${3:-"1 2 4 8 16 32 64"}

OUT=results/hybrid_sweep.csv
mkdir -p results
echo "ranks,threads,total_cores,seconds" > "$OUT"

MPI_LAUNCH=${MPI_LAUNCH:-mpirun}
MPI_NP_FLAG=${MPI_NP_FLAG:--np}

for N in $RANKS_LIST; do
  if (( TOTAL_CORES % N != 0 )); then
    continue
  fi
  P=$(( TOTAL_CORES / N ))

  echo "[bench] hybrid: ranks=$N threads=$P (total=$TOTAL_CORES)"
  MPI_EXTRA_ARGS=${MPI_EXTRA_ARGS:-}
  out=$( OMP_NUM_THREADS=$P ${MPI_LAUNCH} ${MPI_EXTRA_ARGS} ${MPI_NP_FLAG} $N bin/pagerank_hybrid "$INPUT" /dev/null 2>&1 )
  sec=$( echo "$out" | grep -oE 'time=[0-9]+\.[0-9]+s' | head -1 | sed 's/time=//;s/s//' )
  if [[ -z "$sec" ]]; then
    echo "[warn] cannot parse runtime for ranks=$N threads=$P" >&2
    sec="nan"
  fi
  echo "$N,$P,$TOTAL_CORES,$sec" >> "$OUT"
done

echo "[bench] wrote $OUT"

