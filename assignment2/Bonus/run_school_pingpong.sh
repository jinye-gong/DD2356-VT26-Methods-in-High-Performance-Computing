#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
make clean
make

# Two MPI ranks on the school cluster session.
mpirun -np 2 ./ping_pong_mpi 4194304 2000 50 > school_pingpong_rtt.csv

echo "Wrote school_pingpong_rtt.csv"

