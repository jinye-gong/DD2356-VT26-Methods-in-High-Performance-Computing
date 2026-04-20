#!/bin/bash
#SBATCH -J roofline
#SBATCH -A edu26.dd2356
#SBATCH -p shared
#SBATCH -N 1
#SBATCH -n 1
#SBATCH -t 00:20:00
#SBATCH -o dardel_roofline_%j.out

set -euo pipefail
cd "${SLURM_SUBMIT_DIR}"

# Build with Cray compiler wrapper and OpenMP enabled flags from Makefile.
make clean
make CXX=CC

export OMP_NUM_THREADS=16
./roofline | tee dardel_roofline_raw.txt

