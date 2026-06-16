#!/usr/bin/env bash
# Run from Exe2/ (school cluster: ~/dd2356_a4/Exe2)
set -euo pipefail

EXDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$EXDIR/.." && pwd)"
cd "$EXDIR"

echo "=== Directory: $EXDIR ==="

echo "=== Compile ==="
mpicc -O3 -o row_sum_mpi row_sum_mpi.c
echo "OK: $EXDIR/row_sum_mpi"

echo "=== Correctness (N=1000, io=1, np=4) ==="
export WEAK_N=1000
export ROWSUM_IO=1
mpirun -np 4 ./row_sum_mpi
head -5 row_sums_output.txt

echo "=== Weak scaling (BASE_N=1000 per rank, io=0) ==="
export WEAK_BASE_N=1000
export ROWSUM_IO=0
chmod +x run_scaling_weak.sh
./run_scaling_weak.sh ./row_sum_mpi scaling_weak_school_ex2.csv

if [[ -d "$ROOT/Artifacts/results/school" ]]; then
  cp scaling_weak_school_ex2.csv "$ROOT/Artifacts/results/school/"
fi

PLOT_SCRIPT=""
for candidate in "$EXDIR/plot_scaling_mpi.py" "$ROOT/plot_scaling_mpi.py"; do
  [[ -f "$candidate" ]] && PLOT_SCRIPT="$candidate" && break
done

if command -v python3 >/dev/null 2>&1 && [[ -n "$PLOT_SCRIPT" ]]; then
  OUT_PNG="$EXDIR/weak_scaling_ex2_school.png"
  [[ -d "$ROOT/Artifacts/plots/school" ]] && OUT_PNG="$ROOT/Artifacts/plots/school/weak_scaling_ex2_school.png"
  mkdir -p "$(dirname "$OUT_PNG")"
  python3 "$PLOT_SCRIPT" --csv "$EXDIR/scaling_weak_school_ex2.csv" --mode weak \
    --title "Row Sum Ex2 School Weak Scaling" --out "$OUT_PNG"
  echo "Plot: $OUT_PNG"
fi

echo "=== Done. CSV: $EXDIR/scaling_weak_school_ex2.csv ==="
