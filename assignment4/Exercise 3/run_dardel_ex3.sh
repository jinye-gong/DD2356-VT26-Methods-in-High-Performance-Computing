#!/usr/bin/env bash
# Run from Exe3/ or Exercise 3/ on Dardel
set -euo pipefail

EXDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$EXDIR/.." && pwd)"
cd "$EXDIR"

if [[ -f "$ROOT/scripts/dardel_setup_env.sh" ]]; then
  source "$ROOT/scripts/dardel_setup_env.sh"
fi

echo "=== Directory: $EXDIR ==="
cc -O3 -o gol_mpi gol_mpi.c

echo "=== Correctness np=4 ==="
export GOL_N=200 GOL_STEPS=50 GOL_IO=1
rm -f gol_output_*.txt
srun -n 4 ./gol_mpi

echo "=== Efficiency scaling ==="
export GOL_N=2000 GOL_STEPS=500 GOL_IO=0
PROCS=(1 2 4 8 16)
OUT="$EXDIR/efficiency_dardel_ex3.csv"
echo "nprocs,time_sec,speedup,efficiency" > "$OUT"
t1=""
for p in "${PROCS[@]}"; do
  echo "np=$p"
  t=$(srun -n "$p" ./gol_mpi 2>&1 | awk '/WALLTIME/ {print $2; exit}')
  if [[ "$p" -eq 1 ]]; then
    t1="$t"; sp="1.0"; eff="1.0"
  else
    sp=$(awk -v t1="$t1" -v t="$t" 'BEGIN { if (t>0) print t1/t; else print "nan" }')
    eff=$(awk -v p="$p" -v sp="$sp" 'BEGIN { print sp/p }')
  fi
  echo "$p,$t,$sp,$eff" >> "$OUT"
done

[[ -d "$ROOT/Artifacts/results/dardel" ]] && cp "$OUT" "$ROOT/Artifacts/results/dardel/"
echo "=== Done: $OUT ==="
