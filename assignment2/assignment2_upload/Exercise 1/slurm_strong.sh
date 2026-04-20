#!/bin/bash
#SBATCH -J pi_strong
#SBATCH -A edu26.dd2356             # DD2356 课程项目（projinfo: Compute project）
#SBATCH -p shared                   # 或课程/说明里指定的分区
#SBATCH -n 16                       # 本作业最多用 16 个 MPI 进程
#SBATCH -t 0:15:00
#SBATCH -o strong_%j.out

# 编译步骤（在登录节点或提交前完成一次即可）示例：
#   module load PDC
#   module load cpeGNU/24.03
#   module load cray-mpich/8.1.28
#   make CC=cc pi_mpi_strong
#
# 强扩展：总投掷次数固定为 1000 万，进程数 1,2,4,8,16

set -euo pipefail
cd "${SLURM_SUBMIT_DIR}"

EXE=./pi_mpi_strong
TOTAL=10000000

for n in 1 2 4 8 16; do
  echo "=============================="
  echo "Strong scaling: processes = ${n}, total tosses = ${TOTAL}"
  echo "=============================="
  srun -n "${n}" "${EXE}" "${TOTAL}"
  echo
done
