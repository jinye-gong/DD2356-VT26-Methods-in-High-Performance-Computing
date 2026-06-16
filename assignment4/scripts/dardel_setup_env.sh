#!/bin/bash
# Source on Dardel before compile/run:  source scripts/dardel_setup_env.sh
module purge --force 2>/dev/null || true
module load PDC/24.11
module load PrgEnv-gnu/8.6.0
module load cray-mpich/8.1.31
# Cray PE：用 cc（不要用 mpicc）
export MPICC=cc
echo "Loaded: $(module list 2>&1)"
echo "cc: $(which cc)"
