#!/usr/bin/env bash
# Run from Exe1/ or Exercise 1/ on Dardel
set -euo pipefail

EXDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$EXDIR/.." && pwd)"
cd "$EXDIR"

if [[ -f "$ROOT/scripts/dardel_setup_env.sh" ]]; then
  source "$ROOT/scripts/dardel_setup_env.sh"
fi

echo "=== Directory: $EXDIR ==="
echo "=== Compile ==="
cc -O3 -o wave_mpi wave_mpi.c -lm

echo "=== Correctness (np=4, I/O on) ==="
rm -f wave_output_*.txt
srun -n 4 ./wave_mpi 1000 100 1

echo "=== Strong scaling (I/O off) ==="
export WAVE_N=200000
export WAVE_STEPS=500
export WAVE_IO=0
chmod +x run_scaling_strong.sh

PROCS=(1 2 4 8 16)
OUT="$EXDIR/scaling_dardel_ex1.csv"
echo "nprocs,time_sec" > "$OUT"
for p in "${PROCS[@]}"; do
  echo "np=$p"
  if [[ "$p" -le 4 ]]; then
    NODES=1
    TPN="$p"
  else
    NODES=$(( (p + 3) / 4 ))
    TPN=4
  fi
  t=$(srun -N "$NODES" --ntasks="$p" --ntasks-per-node="$TPN" ./wave_mpi 2>&1 | awk '/WALLTIME/ {print $2; exit}')
  echo "$p,$t" >> "$OUT"
done

if [[ -d "$ROOT/Artifacts/results/dardel" ]]; then
  cp "$OUT" "$ROOT/Artifacts/results/dardel/"
fi

PLOT_SCRIPT=""
for candidate in "$EXDIR/plot_scaling_mpi.py" "$ROOT/plot_scaling_mpi.py"; do
  if [[ -f "$candidate" ]]; then
    PLOT_SCRIPT="$candidate"
    break
  fi
done

if command -v python3 >/dev/null 2>&1 && [[ -n "$PLOT_SCRIPT" ]]; then
  OUT_PNG="$EXDIR/strong_scaling_ex1_dardel.png"
  if [[ -d "$ROOT/Artifacts/plots/dardel" ]]; then
    OUT_PNG="$ROOT/Artifacts/plots/dardel/strong_scaling_ex1_dardel.png"
  fi
  mkdir -p "$(dirname "$OUT_PNG")"
  python3 "$PLOT_SCRIPT" --csv "$OUT" --mode strong \
    --title "Wave Ex1 Dardel Strong Scaling" --out "$OUT_PNG" || true
fi

echo "=== Done: $OUT ==="
