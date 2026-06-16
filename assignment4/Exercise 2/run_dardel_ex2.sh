#!/usr/bin/env bash
# Run from Exe2/ or Exercise 2/ on Dardel
set -euo pipefail

EXDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$EXDIR/.." && pwd)"
cd "$EXDIR"

if [[ -f "$ROOT/scripts/dardel_setup_env.sh" ]]; then
  source "$ROOT/scripts/dardel_setup_env.sh"
fi

echo "=== Directory: $EXDIR ==="
cc -O3 -o row_sum_mpi row_sum_mpi.c

echo "=== Correctness np=4 ==="
export WEAK_N=1000 ROWSUM_IO=1
srun -n 4 ./row_sum_mpi
head -5 row_sums_output.txt || true

echo "=== Weak scaling ==="
export WEAK_BASE_N=1000 ROWSUM_IO=0
PROCS=(1 2 4 8 16)
OUT="$EXDIR/scaling_dardel_ex2.csv"
echo "nprocs,time_sec,N" > "$OUT"
for p in "${PROCS[@]}"; do
  N=$((1000 * p))
  echo "np=$p N=$N"
  if [[ "$p" -le 4 ]]; then
    NODES=1; TPN="$p"
  else
    NODES=$(( (p + 3) / 4 )); TPN=4
  fi
  export WEAK_N="$N"
  t=$(srun -N "$NODES" --ntasks="$p" --ntasks-per-node="$TPN" ./row_sum_mpi 2>&1 | awk '/WALLTIME/ {print $2; exit}')
  echo "$p,$t,$N" >> "$OUT"
done

[[ -d "$ROOT/Artifacts/results/dardel" ]] && cp "$OUT" "$ROOT/Artifacts/results/dardel/"

PLOT_SCRIPT=""
for candidate in "$EXDIR/plot_scaling_mpi.py" "$ROOT/plot_scaling_mpi.py"; do
  [[ -f "$candidate" ]] && PLOT_SCRIPT="$candidate" && break
done
if command -v python3 >/dev/null 2>&1 && [[ -n "$PLOT_SCRIPT" ]]; then
  OUT_PNG="$ROOT/Artifacts/plots/dardel/weak_scaling_ex2_dardel.png"
  mkdir -p "$(dirname "$OUT_PNG")"
  python3 "$PLOT_SCRIPT" --csv "$OUT" --mode weak \
    --title "Row Sum Ex2 Dardel Weak Scaling" --out "$OUT_PNG" || true
fi

echo "=== Done: $OUT ==="
