#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
make clean
make

# In DD2356 "Medium CPU only" Jupyter terminal, start with available vCPUs.
export OMP_NUM_THREADS="${OMP_NUM_THREADS:-16}"
./roofline | tee school_roofline_raw.txt

