#!/usr/bin/env bash
# Weak scaling: pass total rows via env WEAK_N (implement in row_sum_mpi.c).
# Usage: ./run_scaling_weak.sh [binary] [output.csv]
set -euo pipefail

BIN="${1:-./row_sum_mpi}"
OUT="${2:-scaling_weak.csv}"
PROCS=(1 2 4 8 16)
BASE_N="${WEAK_BASE_N:-1000}"
export ROWSUM_IO="${ROWSUM_IO:-0}"

echo "nprocs,time_sec,N" > "$OUT"
for p in "${PROCS[@]}"; do
  N=$((BASE_N * p))
  echo "Running np=$p N=$N ..."
  export WEAK_N="$N"
  t=$(mpirun -np "$p" "$BIN" 2>&1 | awk '/WALLTIME/ {print $2; exit}')
  if [[ -z "${t:-}" ]]; then
    echo "Warning: print 'WALLTIME <seconds>' on rank 0 after implementation."
    t="nan"
  fi
  echo "$p,$t,$N" >> "$OUT"
done
echo "Wrote $OUT"
