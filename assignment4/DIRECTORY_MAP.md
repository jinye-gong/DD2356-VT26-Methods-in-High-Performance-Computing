# Assignment4 Directory Map

## 1) Source / execution

- `Exercise 1/` — 1D wave equation, halo exchange (`MPI_Send`/`MPI_Recv`)
- `Exercise 2/` — Matrix row sums (`MPI_Scatter`, `MPI_Gather`, `MPI_Reduce`)
- `Exercise 3/` — Game of Life, 2D ghost cells (`MPI_Isend`/`MPI_Irecv`)

Each exercise folder has its own `README.md`, source (`.c`), optional `run_scaling_*.sh`, and `plot_*.py`.

## 2) Artifacts

- `Artifacts/results/local/` — local `mpirun` timing CSV
- `Artifacts/results/school/` — school cluster CSV
- `Artifacts/results/dardel/` — Dardel CSV
- `Artifacts/plots/{local,school,dardel}/` — scaling / efficiency PNGs

## 3) Root scripts

- `plot_scaling_mpi.py` — plot strong or weak scaling from CSV (`nprocs`, `time_sec`)

## 4) Submission zip suggestion

1. `Exercise 1/`, `Exercise 2/`, `Exercise 3/`
2. Root `README.md`
3. `Artifacts/` (optional, for your own traceability)
4. Report PDF (outside zip or alongside)
