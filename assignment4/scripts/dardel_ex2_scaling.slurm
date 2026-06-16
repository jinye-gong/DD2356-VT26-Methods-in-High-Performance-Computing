#!/bin/bash
#SBATCH -A edu26.dd2356
#SBATCH -p main
#SBATCH -J a4-ex2-rowsum
#SBATCH -t 00:25:00
#SBATCH -N 4
#SBATCH --ntasks-per-node=4
#SBATCH -o logs/a4-ex2-%j.out
#SBATCH -e logs/a4-ex2-%j.err

set -euo pipefail
cd "${SLURM_SUBMIT_DIR:?}"
mkdir -p logs

if [[ -d "Exercise 2" ]]; then
  bash "Exercise 2/run_dardel_ex2.sh"
elif [[ -d Exe2 ]]; then
  bash Exe2/run_dardel_ex2.sh
else
  echo "No Exercise 2/ or Exe2/" >&2
  exit 1
fi
