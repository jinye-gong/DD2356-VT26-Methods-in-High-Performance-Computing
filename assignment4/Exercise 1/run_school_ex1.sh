#!/usr/bin/env bash
# Run from Exe1/ (school cluster: ~/dd2356_a4/Exe1)
set -euo pipefail

EXDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$EXDIR/.." && pwd)"
cd "$EXDIR"

echo "=== Directory: $EXDIR ==="

echo "=== Compile ==="
mpicc -O3 -o wave_mpi wave_mpi.c -lm
echo "OK: $EXDIR/wave_mpi"

echo "=== Correctness run (N=1000, steps=100, I/O on) ==="
rm -f wave_output_*.txt
mpirun -np 4 ./wave_mpi 1000 100 1

echo "=== Strong scaling (N=200000, steps=500, I/O off) ==="
export WAVE_N=200000
export WAVE_STEPS=500
export WAVE_IO=0
chmod +x run_scaling_strong.sh
./run_scaling_strong.sh ./wave_mpi scaling_school_ex1.csv

if [[ -d "$ROOT/Artifacts/results/school" ]]; then
  cp scaling_school_ex1.csv "$ROOT/Artifacts/results/school/"
fi

PLOT_SCRIPT=""
for candidate in "$EXDIR/plot_scaling_mpi.py" "$ROOT/plot_scaling_mpi.py"; do
  if [[ -f "$candidate" ]]; then
    PLOT_SCRIPT="$candidate"
    break
  fi
done

if command -v python3 >/dev/null 2>&1 && [[ -n "$PLOT_SCRIPT" ]]; then
  OUT_PNG="$EXDIR/strong_scaling_ex1_school.png"
  if [[ -d "$ROOT/Artifacts/plots/school" ]]; then
    OUT_PNG="$ROOT/Artifacts/plots/school/strong_scaling_ex1_school.png"
  fi
  mkdir -p "$(dirname "$OUT_PNG")"
  python3 "$PLOT_SCRIPT" \
    --csv "$EXDIR/scaling_school_ex1.csv" \
    --mode strong \
    --title "Wave Ex1 School Strong Scaling" \
    --out "$OUT_PNG"
  echo "Plot: $OUT_PNG"
else
  echo "Skip plot (need python3 and plot_scaling_mpi.py in Exe1/ or dd2356_a4/)"
fi

echo "=== Done. CSV: $EXDIR/scaling_school_ex1.csv ==="
