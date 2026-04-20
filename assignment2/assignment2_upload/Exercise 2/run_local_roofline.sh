#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
make clean
make

# Adjust this to your machine if needed.
export OMP_NUM_THREADS="${OMP_NUM_THREADS:-16}"
./roofline | tee local_roofline_raw.txt

