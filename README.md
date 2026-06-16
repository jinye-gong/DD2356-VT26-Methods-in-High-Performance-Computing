# DD2356 VT26 - Methods in High Performance Computing

Coursework repository for **DD2356 (VT26)** at KTH. Contains source code, run scripts, experiment results, figures, and reports for Assignments 2–4.

## Repository layout

| Directory | Topic | Main technologies |
|-----------|-------|-------------------|
| `assignment2/` | Performance measurement & roofline | MPI, `perf`, roofline model |
| `assignment3/` | Shared-memory programming | OpenMP (parallel, SIMD, tasks, GPU) |
| `assignment4/` | Distributed-memory programming | MPI (point-to-point, collectives, non-blocking) |

Each assignment follows a similar structure:

```text
assignmentN/
├── Exercise 1/ … Exercise 3/   # source (.c), scripts, per-exercise README
├── Bonus/                      # optional (assignment2 & 3)
├── Artifacts/
│   ├── results/{local,school,dardel}/   # scaling / timing CSV
│   ├── plots/{local,school,dardel}/     # generated PNG figures
│   └── screenshots/                     # code / compile / run captures
├── AssignmentN_Report.md       # report (markdown)
└── README.md                   # assignment-level instructions
```

See also `assignment3/DIRECTORY_MAP.md` and `assignment4/DIRECTORY_MAP.md` for detailed folder maps.

## Assignment summaries

### Assignment 2 — Measurement & roofline

- **Exercise 1**: MPI π integration, strong/weak scaling
- **Exercise 2**: Roofline model (local, school cluster, Dardel)
- **Exercise 3**: SpMV benchmark
- **Exercise 4**: Matrix multiply & `perf` analysis
- **Bonus**: MPI ping-pong latency / RTT model

Report: `assignment2/Assignment2_Report.md` (PDF included).

### Assignment 3 — OpenMP

- **Exercise 1**: Matrix multiplication (baseline, SIMD, hybrid, GPU)
- **Exercise 2**: BFS (parallel for, tasks)
- **Exercise 3**: Shallow-water simulation (static / guided / dynamic scheduling)
- **Bonus**: Neuron firing model with OpenMP tasks

Report: `assignment3/Assignment3_Report.md`.

Typical build:

```bash
cd "assignment3/Exercise 1"
gcc -O3 -fopenmp -march=native -o app matmul_ex1_task1.c
OMP_NUM_THREADS=8 ./app
```

### Assignment 4 — MPI

- **Exercise 1**: 1D wave equation + halo exchange (strong scaling)
- **Exercise 2**: Matrix row sums + collectives (weak scaling)
- **Exercise 3**: Game of Life + 2D non-blocking ghost exchange (efficiency)

Reports: `assignment4/Assignment4_Report.md`, `assignment4/Assignment4_Report_zh.md`, PDF.

Typical build:

```bash
cd "assignment4/Exercise 1"
mpicc -O3 -o wave wave_mpi.c -lm
mpirun -np 4 ./wave
```

Cluster jobs: see `assignment4/scripts/` (SLURM) and per-exercise `README.md`.

## Environment

| Component | Assignments |
|-----------|-------------|
| `gcc` / `clang` + OpenMP (`-fopenmp`) | 3 |
| `mpicc` / `mpirun` (Open MPI or MPICH) | 2, 4 |
| Python 3 + `numpy`, `matplotlib` | 2–4 (plotting) |
| Dardel / school cluster (`module load`, `srun`) | 2–4 |

## Git & ignored files

The following are **not** tracked (see `.gitignore`):

- Compiled binaries (`*.o`, `*.out`, `a.out`, exercise build targets)
- Upload archives (`assignment2_upload.zip`, `assignment3.zip`, `assignment3/*.tar.gz`)
- Large regenerated outputs (`assignment3/Exercise 3/output_*.txt`, bonus neuron dumps)
- Editor / Python cache (`.vscode/`, `__pycache__/`, …)

Source code, scripts, CSV results, plots, screenshots, and reports are kept under version control.
