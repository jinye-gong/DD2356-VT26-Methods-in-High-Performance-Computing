# Assignment 4: Distributed-Memory Programming with MPI

<div align="center">
  <strong>Jinye Gong</strong><br>
  jinyeg@kth.se<br><br>
  <strong>Weiyi Lyu</strong><br>
  weiyil@kth.se<br><br>
  <strong>2026-05-20</strong>
</div>

## Contributions

This assignment was completed jointly by both team members.

- **Weiyi Lyu**: Dardel environment setup (`edu26.dd2356`), directory structure, Slurm scripts, and primary runs on Dardel; Exercise 1 implementation and strong-scaling experiments; report structure and MPI sections.
- **Jinye Gong**: School-cluster experiments, Exercise 2 and Exercise 3 implementations, correctness validation and Python visualizations, weak-scaling / efficiency measurements, and final report integration.

Both members contributed to algorithm design, result verification, plotting, and report writing.

## AI Assistance Statement

AI tools were used for Markdown organization, section structuring, and language polishing.

All implementations, command execution, performance measurements, result collection, plotting, and technical conclusions were performed and verified by the authors.

## Screenshot Evidence Index

Screenshots (code, compilation, largest-scale run) are under `Artifacts/screenshots/school/` and `Artifacts/screenshots/dardel/` (Exercises 1–2 on Dardel; Exercise 3 school only per assignment).

| Exercise | School | Dardel |
| --- | --- | --- |
| Ex1 wave | `ex1_*.png` | `ex1_*.png` |
| Ex2 row sum | `ex2_*.png` | `ex2_*.png` |
| Ex3 GoL | `ex3_*.png` | — |

Score-P (Dardel, Ex1–2, np=16): completed with `score-p/9.4-cpeCray-24.11`; metrics reported in §7 of Exercises 1–2.

---

## Exercise 1 - 1D Halo Exchange in a Wave Equation Simulation

### 1. Implementation

We parallelized the 1D explicit wave solver in `Exercise 1/wave_mpi.c` using **1D domain decomposition** and **point-to-point halo exchange**.

#### Parallelization strategy

1. Split the global grid of size `N` across `P` MPI processes.
   - Local size: `local_n = N / P`, remainder distributed to the first `N % P` ranks.
   - Each rank owns contiguous indices `[start, end)` in global coordinates.
2. Allocate local arrays `u`, `u_prev`, `u_next` with **two ghost cells** (left and right).
3. Each time step:
   - Exchange ghost values with neighbors using `MPI_Send` / `MPI_Recv` (or equivalent).
   - Left boundary: rank `0` uses a fixed boundary value; right boundary: rank `P-1` similarly.
   - Update interior points `i = 1 .. local_n` with the finite-difference stencil.
4. Timing: `MPI_Wtime()` around the main loop, preceded by `MPI_Barrier(MPI_COMM_WORLD)`.
5. I/O: only rank `0` writes `wave_output_*.txt` after gathering the global field (for correctness plots). Disable I/O when measuring scalability.

Core halo exchange pattern (conceptual):

```c
/* After local compute setup, each step: */
if (rank > 0) {
    MPI_Send(&u[1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
    MPI_Recv(&u[0], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
if (rank < nprocs - 1) {
    MPI_Send(&u[local_n], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
    MPI_Recv(&u[local_n + 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
```

Design notes:

- Ghost cells hold neighbor values required by the 3-point spatial stencil.
- Synchronization per step ensures all ranks have consistent halos before updating `u_next`.
- Strong scaling keeps **global `N` fixed** while increasing `P`.

#### Parameters used for experiments

| Setting | Correctness / plots | Scalability runs |
| --- | --- | --- |
| `N` | 1000 (default) | e.g. 100000 (increase for scaling) |
| `STEPS` | 100 | e.g. 1000 |
| I/O | every 10 steps | disabled |

### 2. Build and run commands

#### School cluster

```bash
cd ~/dd2356_a4/Exe1
mpicc -O3 -o wave_mpi wave_mpi.c -lm
chmod +x run_school_ex1.sh run_scaling_strong.sh
./run_school_ex1.sh
```

Compile command: **`mpicc -O3 -o wave_mpi wave_mpi.c -lm`**

Run command (scaling): **`mpirun -np <np> ./wave_mpi`** with `WAVE_N=200000 WAVE_STEPS=500 WAVE_IO=0`

#### Dardel

```bash
cd ~/DD2356/assignment4
source scripts/dardel_setup_env.sh
cd "Exercise 1"
cc -O3 -o wave_mpi wave_mpi.c -lm

# Interactive (max 4 ranks per node on one node)
salloc -A edu26.dd2356 -N 1 -t 00:30:00 --ntasks-per-node=4
srun -n 4 ./wave_mpi

# Batch strong scaling at np=16 (4 nodes x 4 tasks/node)
cd ~/DD2356/assignment4
mkdir -p logs
sbatch scripts/dardel_run_mpi.slurm 1 16
```

Compile command for report (Dardel): **`cc -O3 -o wave_mpi wave_mpi.c -lm`** after `module load PDC/24.11 PrgEnv-gnu/8.6.0 cray-mpich/8.1.31`.

Run command for report (Dardel): **`srun -n <np> ./wave_mpi`** inside an allocated job.

### 3. Correctness validation

We compared wave outputs from:

1. **Serial baseline** (single-process run or serial reference code).
2. **Parallel MPI run** on the school cluster.

Validation approach:

- Plot `wave_output_*.txt` with `plot_wave.py` and overlay serial vs parallel curves at the same time steps.
- Check max absolute difference between serial and parallel gathered fields (should be near machine epsilon if algorithm matches).

Correctness check (school cluster, `wave_output_0.txt`):

![Figure 1: School cluster wave output sample (step 0).](Artifacts/screenshots/school/ex1_correctness.png)

Serial vs parallel wave visualization (same setup, `N=200`, `STEPS=50`, `I/O=1`; serial = `np=1`, parallel = `np=4`):

![Figure 1b: Wave evolution comparison (serial vs parallel).](Artifacts/plots/school/wave_serial_vs_parallel_ex1.png)

School cluster screenshots (code / compile / run / scaling):

![Figure 2: MPI halo exchange and local update (school).](Artifacts/screenshots/school/ex1_code.png)

![Figure 3: Compiled executable (school).](Artifacts/screenshots/school/ex1_compile.png)

![Figure 4: Strong-scaling run at np=16 (school).](Artifacts/screenshots/school/ex1_run_np16.png)

![Figure 5: Strong-scaling CSV (school).](Artifacts/screenshots/school/ex1_scaling_csv.png)

### 4. Strong-scaling results (time in seconds)

Fixed global problem size: **`N = 200000`**; **`STEPS = 500`**; process counts: **`1, 2, 4, 8, 16`**. **I/O disabled** (`WAVE_IO=0`) for timing.

| Processes | School Time (s) | School Speedup | Dardel Time (s) | Dardel Speedup |
| ---: | ---: | ---: | ---: | ---: |
| 1 | 0.3130 | 1.0000 | 0.1447 | 1.0000 |
| 2 | 0.0798 | 3.92 | 0.0738 | 1.96 |
| 4 | 0.0474 | 6.60 | 0.0390 | 3.71 |
| 8 | 0.0272 | 11.51 | 0.0231 | 6.27 |
| 16 | 0.0351 | 8.93 | 0.0144 | 10.01 |

Speedup definition: `S(p) = T(1) / T(p)`.  
Raw CSV: `Exercise 1/scaling_school_ex1.csv`, `Exercise 1/scaling_dardel_ex1.csv`.

### 5. Plots: measured strong scaling + ideal scaling (dashed)

School cluster:

![Figure 6: School cluster strong scaling (Exercise 1).](Artifacts/plots/school/strong_scaling_ex1_school.png)

Dardel:

![Figure 7: Dardel strong scaling (Exercise 1).](Artifacts/plots/dardel/strong_scaling_ex1_dardel.png)

### 6. Performance analysis

**School cluster:**

- Runtime drops steadily from 1 to 8 processes (`0.313s → 0.027s`, speedup **11.5×** at 8 ranks), showing good strong scaling while local work per rank is still large enough.
- At **16 processes**, time increases to `0.035s` (speedup **8.9×** vs ideal 16×), indicating overhead dominates: halo latency per step, MPI startup/synchronization, and possible resource contention on the shared Jupyter/cluster node.
- Parallel efficiency at 16 ranks: `8.93/16 ≈ 56%`.

**Dardel:**

- Monotonic improvement through **16 ranks** (`0.145s → 0.014s`, speedup **10.0×**), with no regression at the largest scale in this run.
- Scaling is smoother than school at high rank counts; job used up to **4 nodes × 4 MPI ranks/node** for `np=16` as required.
- Efficiency at 16 ranks: `10.01/16 ≈ 63%`; remaining gap to ideal is mainly halo communication and barriers each step.
- Dardel is faster than school at all process counts in this dataset (e.g. `T(1)=0.145s` vs `0.313s`), consistent with dedicated compute nodes vs shared interactive environment.

### 7. Score-P profiling (Dardel, largest scale: np = 16)

After the initial legacy-module failure (`PDCOLD/21.11` / `Score-P/7.0-cpeGNU-21.11`), we used the current Dardel-compatible module:

```bash
module load PDC/24.11 PrgEnv-cray cray-mpich/8.1.31 score-p/9.4-cpeCray-24.11
scorep --mpp=mpi --thread=none cc -O3 -o wave_mpi_scorep wave_mpi.c -lm
export SCOREP_ENABLE_TRACING=false SCOREP_ENABLE_PROFILING=true
export SCOREP_EXPERIMENT_DIRECTORY=scorep_ex1
srun -N 4 -n 16 --ntasks-per-node=4 ./wave_mpi_scorep
scorep-score -r scorep_ex1/profile.cubex
```

`scorep-score` summary (`np=16`, `WAVE_N=200000`, `WAVE_STEPS=500`, `WAVE_IO=0`):

| Metric | Value |
| --- | --- |
| Total MPI calls (`visits`, aggregate) | **15,096** |
| Time spent in MPI calls | **10.60 s** (**98.7%** of total runtime 10.74 s) |
| Dominant MPI routines | `MPI_Init` (9.97 s), `MPI_Sendrecv` (15,000 calls), `MPI_Finalize` (0.43 s) |

---

## Exercise 2 - Parallel Row Sum Computation using MPI Collectives

### 1. Implementation

We parallelized row-sum computation in `Exercise 2/row_sum_mpi.c` using **MPI collective communication**.

#### Parallelization strategy

1. Rank `0` builds the full `N x N` matrix (or generates rows on the fly).
2. **`MPI_Scatterv`**: distribute matrix rows across ranks (variable row counts supported).
3. Local work: each rank computes partial row sums for its rows.
4. **`MPI_Gatherv`**: collect all row sums to rank `0` and write `row_sums_output.txt`.
5. **`MPI_Reduce`**: compute global sum of all matrix elements with `MPI_SUM` on rank `0`.
6. Timing: `MPI_Wtime()` + `MPI_Barrier` around scatter + local compute + gather/reduce.

`MPI_Scatterv` and `MPI_Gatherv` are used as generalized forms of `MPI_Scatter` / `MPI_Gather` to support uneven row distribution when `N` is not divisible by `P`.

Conceptual structure:

```c
MPI_Scatter(matrix, rows_per_rank * N, MPI_DOUBLE,
            local_matrix, local_rows * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

compute_row_sums(local_matrix, local_row_sums, local_rows);

MPI_Gather(local_row_sums, local_rows, MPI_DOUBLE,
           row_sums, local_rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

MPI_Reduce(&local_total, &global_total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
```

#### Weak-scaling setup

- **Weak scaling**: when `p` doubles, **global rows** double so each rank keeps ~constant row count.
- Example: base `N_base = 1000` rows at `p=1`; at `p=16`, global rows `N = 16000`.
- Implement via environment variable `WEAK_N` or command-line argument.

### 2. Build and run commands

#### School cluster

```bash
cd ~/dd2356_a4/Exe2
chmod +x run_school_ex2.sh run_scaling_weak.sh
./run_school_ex2.sh
```

Compile: **`mpicc -O3 -o row_sum_mpi row_sum_mpi.c`**

Weak scaling run: **`mpirun -np <np> ./row_sum_mpi`** with `WEAK_N=<1000*np>` and `ROWSUM_IO=0`.

#### Dardel

```bash
cd ~/DD2356/assignment4
source scripts/dardel_setup_env.sh
cd "Exercise 2"
cc -O3 -o row_sum_mpi row_sum_mpi.c

# Batch weak scaling (np = 1, 2, 4, 8, 16; N = 1000 * np)
sbatch --account=edu26.dd2356 scripts/dardel_ex2_scaling.slurm
```

Compile (Dardel): **`cc -O3 -o row_sum_mpi row_sum_mpi.c`** after loading `PDC/24.11`, `PrgEnv-gnu/8.6.0`, `cray-mpich/8.1.31`.

Run (Dardel, inside allocation): **`srun -n <np> ./row_sum_mpi`** with `WEAK_N=<1000*np>` and `ROWSUM_IO=0` for timing runs.

### 3. Correctness validation

Compare `row_sums_output.txt` from serial and parallel runs:

```bash
python3 plot_row_sums.py
```

School cluster screenshots:

![Figure 8: MPI Scatterv/Gatherv/Reduce (school).](Artifacts/screenshots/school/ex2_code.png)

![Figure 9: Compiled executable (school).](Artifacts/screenshots/school/ex2_compile.png)

![Figure 10: Weak-scaling run at np=16, N=16000 (school).](Artifacts/screenshots/school/ex2_run_np16.png)

![Figure 11: Weak-scaling CSV (school).](Artifacts/screenshots/school/ex2_scaling_csv.png)

Dardel screenshots (code / compile / run / scaling):

![Figure 11a: MPI Scatterv/Gatherv/Reduce (Dardel).](Artifacts/screenshots/dardel/ex2_code.png)

![Figure 11b: Compiled executable (Dardel).](Artifacts/screenshots/dardel/ex2_compile.png)

![Figure 11c: Weak-scaling run at np=16, N=16000 (Dardel).](Artifacts/screenshots/dardel/ex2_run_np16.png)

![Figure 11d: Weak-scaling CSV (Dardel).](Artifacts/screenshots/dardel/ex2_scaling_csv.png)

At `np=16`, `MPI_Reduce` global sum = `2.06835072e12` (matches formula for initialized matrix; same on school and Dardel).

Serial vs parallel row-sum visualization (`WEAK_N=1000`, serial = `np=1`, parallel = `np=4`):

![Figure 11e: Row sums comparison (serial vs parallel).](Artifacts/plots/school/row_sums_serial_vs_parallel_ex2.png)

### 4. Weak-scaling results (time in seconds)

Base rows per rank: **`N_base = 1000`** (global `N = 1000 × p`); process counts: `1, 2, 4, 8, 16`. I/O disabled for timing.

| Processes | Global N | School Time (s) | Dardel Time (s) |
| ---: | ---: | ---: | ---: |
| 1 | 1000 | 0.00532 | 0.00343 |
| 2 | 2000 | 0.01924 | 0.01228 |
| 4 | 4000 | 0.09108 | 0.04039 |
| 8 | 8000 | 0.32106 | 0.09062 |
| 16 | 16000 | 1.61996 | 0.12240 |

Raw CSV: `Exercise 2/scaling_weak_school_ex2.csv`, `Exercise 2/scaling_dardel_ex2.csv` (copies in `Artifacts/results/dardel/`).

Ideal weak scaling: `T(p) ≈ T(1)`.

### 5. Plots: measured weak scaling + ideal (flat) line

**School cluster:**

![Figure 12: School cluster weak scaling (Exercise 2).](Artifacts/plots/school/weak_scaling_ex2_school.png)

**Dardel:**

![Figure 13: Dardel weak scaling (Exercise 2).](Artifacts/plots/dardel/weak_scaling_ex2_dardel.png)

### 6. Performance analysis

**School cluster:**

- Weak scaling is **not flat**: time grows from `0.005s` (1 proc) to `1.62s` (16 procs) while per-rank row count stays ~1000.
- Main reason: rank `0` still builds the full `N×N` matrix (`O(N²)` memory and init), and `MPI_Scatterv` / `MPI_Gatherv` move `O(N²)` data each run. As `N` grows in weak scaling, these collective costs dominate.
- At `np=16`, communication and root allocation for `N=16000` (~2 GB matrix) explain the large increase.

**Dardel:**

- Time increases from `0.0034s` to `0.122s` (×36) vs school `×304` for the same weak-scaling sequence.
- Dardel still shows overhead at large `N` (collectives + root `O(N²)` setup), but dedicated nodes and faster interconnect reduce the gap vs ideal weak scaling.
- At `np=16`, Dardel is **~13× faster** than school (`0.122s` vs `1.62s`).
- Score-P at largest scale is reported in §7 below.

### 7. Score-P profiling (Dardel, largest scale: np = 16)

We profiled `row_sum_mpi` at largest scale (`np=16`, `WEAK_N=16000`, `ROWSUM_IO=0`) with `score-p/9.4-cpeCray-24.11` on the `PDC/24.11` stack.

| Metric | Value |
| --- | --- |
| Total MPI calls (`visits`, aggregate) | **144** |
| Time spent in MPI calls | **16.95 s** (**96.0%** of total runtime 17.65 s) |
| Dominant MPI routines | `MPI_Init` (10.17 s), `MPI_Barrier` (4.59 s), `MPI_Scatterv` (1.57 s) |

---

## Exercise 3 - 2D Game of Life with MPI and Non-Blocking Communication

### 1. Implementation

We parallelized Conway's Game of Life in `Exercise 3/gol_mpi.c` with **2D domain decomposition** and **non-blocking ghost-cell exchange**.

#### Parallelization strategy

1. Factorize `P` processes into a 2D grid `Px × Py` (e.g., for `P=16`, use `4×4`).
2. Each rank owns a subdomain with **one-cell-wide ghost layers** on all four sides.
3. Each generation:
   - Post `MPI_Irecv` for ghost rows/columns from neighbors.
   - Post `MPI_Isend` of boundary data to neighbors.
   - Optionally compute interior cells while communication is in flight.
   - `MPI_Waitall` before updating boundary cells.
   - Apply Conway rules locally.
4. Periodic boundaries: map neighbor ranks with modulo in x and y (torus).
5. Timing: `MPI_Wtime()` + `MPI_Barrier` around the main loop; disable `write_output` for scalability runs.

Non-blocking halo pattern (conceptual):

```c
MPI_Request reqs[8];
/* Irecv ghost rows/cols from up/down/left/right */
MPI_Irecv(..., &reqs[0]);
MPI_Isend(..., &reqs[1]);
/* ... */
update_interior();          /* overlap with communication */
MPI_Waitall(nreq, reqs, MPI_STATUSES_IGNORE);
update_boundaries();
```

#### Conway rules (unchanged)

- Live cell with `<2` or `>3` neighbors dies.
- Dead cell with exactly `3` neighbors becomes alive.
- Otherwise state is unchanged.

### 2. Build and run commands

#### School cluster (efficiency)

```bash
cd ~/dd2356_a4/Exe3
chmod +x run_school_ex3.sh run_efficiency.sh
./run_school_ex3.sh
```

Compile: **`mpicc -O3 -o gol_mpi gol_mpi.c`**

Run (efficiency): **`mpirun -np <np> ./gol_mpi`** with `GOL_N=2000 GOL_STEPS=500 GOL_IO=0`

### 3. Correctness validation and visualization

MPI uses **directional MPI tags** (`TAG_N2S`, `TAG_S2N`, etc.) so sender/receiver pairs match on every edge (fixes deadlock from mismatched tags).

School cluster screenshots:

![Figure 14: MPI Isend/Irecv ghost exchange (school).](Artifacts/screenshots/school/ex3_code.png)

![Figure 15: Compiled executable (school).](Artifacts/screenshots/school/ex3_compile.png)

![Figure 16: Efficiency run at np=16 (school).](Artifacts/screenshots/school/ex3_run_np16.png)

![Figure 17: Efficiency CSV (school).](Artifacts/screenshots/school/ex3_scaling_csv.png)

Game-of-Life correctness visualization (`GOL_N=200`, `GOL_STEPS=40`, `GOL_IO=1`, `np=4`; snapshots at steps 0/10/20/30):

![Figure 17b: Game of Life snapshots for correctness validation.](Artifacts/plots/school/gol_visualization_ex3.png)

Visualize correctness with `python3 plot_gol.py` on `gol_output_*.txt` from a correctness run (`GOL_IO=1`).

### 4. Parallel efficiency (school cluster)

Problem size: **`N = 2000`**, **`STEPS = 500`**, **I/O disabled**.

Note: Figure 17b is a correctness visualization run (`N=200`, `STEPS=40`, `I/O=1`), while the efficiency table/plot below use the timing configuration (`N=2000`, `STEPS=500`, `I/O=0`).

| Processes | Time (s) | Speedup S(p) | Efficiency E(p)=S(p)/p |
| ---: | ---: | ---: | ---: |
| 1 | 2.9266 | 1.0000 | 1.0000 |
| 2 | 1.5657 | 1.8691 | 0.9346 |
| 4 | 0.8291 | 3.5299 | 0.8825 |
| 8 | 0.3771 | 7.7613 | 0.9702 |
| 16 | 0.2823 | 10.3681 | 0.6480 |

Raw CSV: `Exercise 3/efficiency_school_ex3.csv`.

### 5. Plot: parallel efficiency vs process count (school cluster)

![Figure 18: School cluster parallel efficiency (Exercise 3).](Artifacts/plots/school/efficiency_ex3_school.png)

### 6. Performance analysis (school cluster)

- Speedup grows to **10.37×** at 16 processes; efficiency stays **>88%** through 8 processes, then drops to **65%** at 16.
- The drop at 16 processes is typical when ghost communication and `MPI_Waitall` overhead grow relative to per-rank interior work, even with interior/computation overlap during edge exchange.
- Non-blocking `MPI_Irecv`/`MPI_Isend` allow `update_interior()` while edge messages are in flight; boundary cells are updated after the first `Waitall` and corner exchange.

---

## Appendix: Reproducibility Files

Core documentation:

- `README.md`: project overview and compile/run summary.
- `Exercise 1/README.md`, `Exercise 2/README.md`, `Exercise 3/README.md`: per-exercise instructions.
- `scripts/README-dardel.md`: Dardel modules, `edu26.dd2356`, Slurm examples.

Source code:

- `Exercise 1/wave_mpi.c` — 1D wave + halo exchange
- `Exercise 2/row_sum_mpi.c` — row sums + collectives
- `Exercise 3/gol_mpi.c` — Game of Life + non-blocking ghosts

Scripts:

- `Exercise 1/run_scaling_strong.sh`, `Exercise 1/plot_wave.py`
- `Exercise 2/run_scaling_weak.sh`, `Exercise 2/plot_row_sums.py`
- `Exercise 3/run_efficiency.sh`, `Exercise 3/plot_gol.py`
- `plot_scaling_mpi.py` — strong/weak scaling plots
- `scripts/dardel_setup_env.sh`, `scripts/dardel_run_mpi.slurm`

Results and plots:

- Exercise 1 (completed):
  - `Exercise 1/scaling_school_ex1.csv`, `Exercise 1/scaling_dardel_ex1.csv`
  - `Artifacts/plots/school/strong_scaling_ex1_school.png`
  - `Artifacts/plots/school/wave_serial_vs_parallel_ex1.png`
  - `Artifacts/plots/dardel/strong_scaling_ex1_dardel.png`
- Exercise 2 (completed):
  - `Exercise 2/scaling_weak_school_ex2.csv`, `Exercise 2/scaling_dardel_ex2.csv`
  - `Artifacts/plots/school/weak_scaling_ex2_school.png`
  - `Artifacts/plots/school/row_sums_serial_vs_parallel_ex2.png`
  - `Artifacts/plots/dardel/weak_scaling_ex2_dardel.png`
  - `Artifacts/screenshots/school/ex2_*.png`
  - `Artifacts/screenshots/dardel/ex2_*.png`
- Exercise 3 (school efficiency, per assignment):
  - `Exercise 3/efficiency_school_ex3.csv`
  - `Artifacts/plots/school/efficiency_ex3_school.png`
  - `Artifacts/plots/school/gol_visualization_ex3.png`
  - `Artifacts/screenshots/school/ex3_*.png`
- Score-P (Ex1–2, Dardel): completed with `score-p/9.4-cpeCray-24.11`; metrics reported in Exercise 1/2 §7
- Screenshots: `Artifacts/screenshots/school/`, `Artifacts/screenshots/dardel/` (Ex1–2)

Dardel project account: **`edu26.dd2356`**

Working directory on Dardel: **`~/DD2356/assignment4`**
