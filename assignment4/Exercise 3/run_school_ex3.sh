#!/usr/bin/env bash
# Run from Exe3/ (school cluster: ~/dd2356_a4/Exe3)
set -euo pipefail

EXDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$EXDIR/.." && pwd)"
cd "$EXDIR"

echo "=== Directory: $EXDIR ==="

echo "=== Compile ==="
mpicc -O3 -o gol_mpi gol_mpi.c
echo "OK: $EXDIR/gol_mpi"

echo "=== Correctness (N=200, steps=40, io=1, np=4) ==="
export GOL_N=200
export GOL_STEPS=40
export GOL_IO=1
rm -f gol_output_*.txt
mpirun -np 4 ./gol_mpi

echo "=== Efficiency scaling (N=2000, steps=500, io=0) ==="
export GOL_N=2000
export GOL_STEPS=500
export GOL_IO=0
chmod +x run_efficiency.sh
./run_efficiency.sh ./gol_mpi efficiency_school_ex3.csv

if [[ -d "$ROOT/Artifacts/results/school" ]]; then
  cp efficiency_school_ex3.csv "$ROOT/Artifacts/results/school/"
fi

echo "=== Done. CSV: $EXDIR/efficiency_school_ex3.csv ==="
