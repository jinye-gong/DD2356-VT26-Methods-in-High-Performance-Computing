#!/bin/bash
#SBATCH -A edu26.dd2356
#SBATCH -J dd2356-a4
#SBATCH -t 00:15:00
#SBATCH -N 4                        # 16 进程示例：4 节点 × 4 核/节点
#SBATCH --ntasks-per-node=4
#SBATCH -o logs/%x-%j.out
#SBATCH -e logs/%x-%j.err

set -euo pipefail
cd "${SLURM_SUBMIT_DIR:?}"

source scripts/dardel_setup_env.sh
mkdir -p logs results

EX="${1:-1}"   # 1 | 2 | 3
NP="${2:-16}"

case "$EX" in
  1) cd "Exercise 1"; BIN=wave_mpi; cc -O3 -o "$BIN" wave_mpi.c -lm ;;
  2) cd "Exercise 2"; BIN=row_sum_mpi; cc -O3 -o "$BIN" row_sum_mpi.c ;;
  3) cd "Exercise 3"; BIN=gol_mpi; cc -O3 -o "$BIN" gol_mpi.c ;;
  *) echo "Usage: sbatch dardel_run_mpi.slurm <exercise:1|2|3> [nprocs]"; exit 1 ;;
esac

echo "Running $BIN with $NP MPI tasks on $(hostname)"
srun -n "$NP" "./$BIN" | tee "../results/ex${EX}/run_np${NP}.log"
