#!/usr/bin/env bash
# Strong scaling: fixed problem size, varying process count.
# Usage: ./run_scaling_strong.sh [binary] [output.csv]
set -euo pipefail

BIN="${1:-./wave_mpi}"
OUT="${2:-scaling_strong.csv}"
PROCS=(1 2 4 8 16)

export WAVE_N="${WAVE_N:-200000}"
export WAVE_STEPS="${WAVE_STEPS:-500}"
export WAVE_IO="${WAVE_IO:-0}"

echo "nprocs,time_sec" > "$OUT"
for p in "${PROCS[@]}"; do
  echo "Running np=$p (N=$WAVE_N steps=$WAVE_STEPS io=$WAVE_IO) ..."
  t=$(mpirun -np "$p" "$BIN" 2>&1 | awk '/WALLTIME/ {print $2; exit}')
  if [[ -z "${t:-}" ]]; then
    echo "Warning: program should print 'WALLTIME <seconds>' on rank 0 after MPI implementation."
    t="nan"
  fi
  echo "$p,$t" >> "$OUT"
done
echo "Wrote $OUT"
