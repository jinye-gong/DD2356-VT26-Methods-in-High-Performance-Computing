#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"

gcc -O2 -o transpose.out transpose.c

for n in 64 128 2048; do
  echo "=== transpose base N=${n} ==="
  perf stat -e \
cycles,instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses \
./transpose.out "$n"
  echo
done

echo "=== transpose blocked N=2048 block=32 ==="
perf stat -e \
cycles,instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses \
./transpose.out 2048 blocked 32

cat <<'EOF'

Use these formulas in the report:
  IPC = instructions / cycles
  L1 miss rate  ~= L1-dcache-load-misses / L1-dcache-loads
  LLC miss rate ~= LLC-load-misses / LLC-loads

Bandwidth is printed by the program itself as:
  Base Rate = 8*N*N / time   (MB/s shown as *1e-6)

EOF

