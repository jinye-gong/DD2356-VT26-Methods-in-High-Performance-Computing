#!/usr/bin/env bash
# Parallel efficiency: efficiency = T1 / (p * Tp)
# Usage: ./run_efficiency.sh [binary] [output.csv]
set -euo pipefail

BIN="${1:-./gol_mpi}"
OUT="${2:-efficiency.csv}"
PROCS=(1 2 4 8 16)
export GOL_N="${GOL_N:-2000}"
export GOL_STEPS="${GOL_STEPS:-500}"
export GOL_IO="${GOL_IO:-0}"

echo "nprocs,time_sec,speedup,efficiency" > "$OUT"
t1=""
for p in "${PROCS[@]}"; do
  echo "Running np=$p ..."
  t=$(mpirun -np "$p" "$BIN" 2>&1 | awk '/WALLTIME/ {print $2; exit}')
  if [[ -z "${t:-}" ]]; then
    t="nan"
  fi
  if [[ "$p" -eq 1 ]]; then
    t1="$t"
    sp="1.0"
    eff="1.0"
  else
    sp=$(awk -v t1="$t1" -v t="$t" 'BEGIN { if (t>0) print t1/t; else print "nan" }')
    eff=$(awk -v p="$p" -v sp="$sp" 'BEGIN { print sp/p }')
  fi
  echo "$p,$t,$sp,$eff" >> "$OUT"
done
echo "Wrote $OUT"
