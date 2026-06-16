#!/usr/bin/env bash
set -euo pipefail

# Usage:
#   ./run_scaling_bfs.sh [num_vertices] [degree] [output_csv] [binary]
# Example:
#   ./run_scaling_bfs.sh 200000 16 scaling_local_ex2_task3_bfs_parallel.csv ./bfs_ex2_task1_parallel

N="${1:-200000}"
DEGREE="${2:-16}"
OUT_CSV="${3:-scaling_bfs.csv}"
BIN="${4:-./bfs_ex2_task1_parallel}"

THREADS=(1 2 4 8 16 32 64)

if [[ ! -x "$BIN" ]]; then
  echo "Error: binary not found or not executable: $BIN"
  exit 1
fi

echo "threads,time_sec,speedup,efficiency" > "$OUT_CSV"

T1=""
for t in "${THREADS[@]}"; do
  export OMP_NUM_THREADS="$t"

  output="$("$BIN" "$N" "$DEGREE")"
  time_sec="$(printf "%s\n" "$output" | awk '/OMP time:/ {print $3}')"

  if [[ -z "$time_sec" ]]; then
    echo "Warning: failed to parse runtime for threads=$t, skipping"
    continue
  fi

  if [[ -z "$T1" ]]; then
    T1="$time_sec"
  fi

  speedup="$(awk -v t1="$T1" -v tp="$time_sec" 'BEGIN { printf "%.6f", t1/tp }')"
  efficiency="$(awk -v s="$speedup" -v p="$t" 'BEGIN { printf "%.6f", s/p }')"

  echo "$t,$time_sec,$speedup,$efficiency" >> "$OUT_CSV"
  echo "threads=$t time=${time_sec}s speedup=${speedup}x efficiency=${efficiency}"
done

echo "Saved: $OUT_CSV"
