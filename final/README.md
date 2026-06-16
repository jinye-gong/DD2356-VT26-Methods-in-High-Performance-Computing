# Final Project — Parallel PageRank (DD2356 VT26)

Reference for the chosen application: *Graph analytics — PageRank learning
implementation* ([timothyasp/PageRank](https://github.com/timothyasp/PageRank)).
That repo is a single-threaded Python/NetworkX prototype; this project
re-implements the algorithm in C with three HPC backends and uses NetworkX
only as a correctness oracle.

## What is built

| Backend  | Source                              | Binary                  |
|----------|-------------------------------------|-------------------------|
| serial   | `src/serial/pagerank_serial.c`      | `bin/pagerank_serial`   |
| OpenMP   | `src/omp/pagerank_omp.c`            | `bin/pagerank_omp`      |
| MPI      | `src/mpi/pagerank_mpi.c`            | `bin/pagerank_mpi`      |
| hybrid   | `src/hybrid/pagerank_hybrid.c`      | `bin/pagerank_hybrid`   |

All backends share `src/common/` (CSR graph, edge-list reader, timer) and the
public headers under `include/`.

## Directory layout

```
final/
├── include/        # public headers (graph.h, pagerank.h, io.h, timer.h)
├── src/
│   ├── common/     # CSR + I/O + timer
│   ├── serial/     # baseline power iteration
│   ├── omp/        # OpenMP version
│   ├── mpi/        # MPI version (1-D vertex partition)
│   └── hybrid/     # MPI + OpenMP
├── data/           # sample.edges + download.sh for SNAP datasets
├── scripts/        # run / bench / verify / plot
├── results/        # CSVs, PNGs, dumped rank vectors (gitignored)
├── docs/           # design notes
├── Makefile
└── Final Project Proposal.pdf
```

## Build

Local laptop (Ubuntu, no MPI yet):

```bash
make serial omp        # builds the two backends that don't need MPI
```

With MPI installed (`sudo apt install mpich` or `openmpi-bin libopenmpi-dev`):

```bash
make all               # serial + omp + mpi + hybrid
```

On **Dardel** (Cray PrgEnv):

```bash
module load PDC craype-x86-rome
module load cpe/23.12 cray-mpich
make CC=cc MPICC=cc CFLAGS_EXTRA="-O3 -march=znver2"
```

## Run

```bash
# smoke test on the 6-node sample graph
bin/pagerank_serial data/sample.edges results/ranks_serial.txt

OMP_NUM_THREADS=4 bin/pagerank_omp data/sample.edges results/ranks_omp.txt

mpirun -np 4 bin/pagerank_mpi data/sample.edges results/ranks_mpi.txt

OMP_NUM_THREADS=2 mpirun -np 4 bin/pagerank_hybrid data/sample.edges results/ranks_hybrid.txt
```

A one-shot driver that runs every available backend and verifies against
NetworkX:

```bash
./scripts/run_local.sh
```

## Real datasets

```bash
cd data
./download.sh web-Stanford       # ~280k vertices,  small but real
./download.sh web-Google         # ~875k vertices,  good OMP scale test
./download.sh soc-LiveJournal1   # ~4.8M vertices,  good MPI scale test
```

The reader auto-skips `#` / `%` comment lines, so SNAP plain-text files work
out of the box.

## Benchmark + plot

```bash
# OpenMP strong scaling over 1..16 threads on web-Google
./scripts/bench_strong.sh data/web-Google.txt "1 2 4 8 16"
python3 scripts/plot.py results/strong_omp.csv

# MPI strong scaling over 1..8 ranks
./scripts/bench_mpi.sh data/web-Google.txt "1 2 4 8"
python3 scripts/plot.py results/strong_mpi.csv
```

## Dardel (Slurm) quick start

Ready-made batch scripts are under `jobs/`:

- `jobs/dardel_build.sbatch`
- `jobs/dardel_strong_omp.sbatch`
- `jobs/dardel_strong_mpi.sbatch`
- `jobs/dardel_weak_mpi.sbatch`
- `jobs/dardel_hybrid.sbatch`

Submit examples:

```bash
# if your project root on Dardel is not ~/pagerank/final:
sbatch --export=ALL,PROJECT_ROOT=/cfs/klemming/home/u/....../final jobs/dardel_build.sbatch

sbatch --export=ALL,PROJECT_ROOT=/cfs/klemming/home/u/....../final,INPUT=data/web-Google.txt jobs/dardel_strong_omp.sbatch

sbatch --export=ALL,PROJECT_ROOT=/cfs/klemming/home/u/....../final,INPUT=data/soc-LiveJournal1.txt,RANKS="128 256 384 512" jobs/dardel_strong_mpi.sbatch

sbatch --export=ALL,PROJECT_ROOT=/cfs/klemming/home/u/....../final,INPUT=data/soc-LiveJournal1.txt,RANKS="128 256 384 512",EDGES_PER_RANK=200000 jobs/dardel_weak_mpi.sbatch

sbatch --export=ALL,PROJECT_ROOT=/cfs/klemming/home/u/....../final,INPUT=data/web-Google.txt,TOTAL_CORES=256,RANKS_LIST="1 2 4 8 16 32 64 128 256" jobs/dardel_hybrid.sbatch
```

## Verify correctness

```bash
pip install networkx
python3 scripts/verify.py data/sample.edges results/ranks_serial.txt
```

The verifier compares the L1 distance against `networkx.pagerank(g, alpha=0.85)`
and checks that the top-10 ordering matches.

## Algorithm + design notes

See `docs/design.md`.

## Roadmap (suggested project milestones)

1. **Week 1.** Serial baseline + verification harness (done in this skeleton).
2. **Week 2.** OpenMP scaling study on `web-Google`, NUMA experiments.
3. **Week 3.** MPI 1-D partition, validate, strong/weak scaling on Dardel.
4. **Week 4.** Hybrid MPI+OpenMP; overlap `Allgatherv` with computation.
5. **Week 5.** Distributed graph build (each rank reads its slice only).
6. **Week 6.** Report + plots.
