#!/bin/bash
#SBATCH -J pi_weak
#SBATCH -A edu26.dd2356             # DD2356 课程项目（projinfo: Compute project）
#SBATCH -p shared
#SBATCH -n 16
#SBATCH -t 0:15:00
#SBATCH -o weak_%j.out

# 弱扩展：每个进程本地投掷 100 万次（源码中 Option 2），进程数 1,2,4,8,16
# 编译：
#   make CC=cc pi_mpi_weak

set -euo pipefail
cd "${SLURM_SUBMIT_DIR}"

EXE=./pi_mpi_weak

for n in 1 2 4 8 16; do
  echo "=============================="
  echo "Weak scaling: processes = ${n}, local tosses = 1000000 per rank"
  echo "=============================="
  srun -n "${n}" "${EXE}"
  echo
done
