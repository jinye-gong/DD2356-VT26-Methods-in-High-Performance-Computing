#!/bin/bash
#SBATCH -J pingpong
#SBATCH -A edu26.dd2356
#SBATCH -p shared
#SBATCH -N 2
#SBATCH -n 2
#SBATCH --ntasks-per-node=1
#SBATCH -t 00:10:00
#SBATCH -o dardel_pingpong_%j.out

set -euo pipefail
cd "${SLURM_SUBMIT_DIR}"

# Build with Cray MPI wrapper if available.
make clean
make CC=cc

# Two ranks on two different nodes, one rank per node.
srun -N2 -n2 --ntasks-per-node=1 ./ping_pong_mpi 4194304 2000 50 > dardel_pingpong_rtt.csv

echo "Wrote dardel_pingpong_rtt.csv"

