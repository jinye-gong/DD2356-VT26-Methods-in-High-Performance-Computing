# Design notes

## Algorithm

We use vanilla **power iteration** on the Google matrix

\[
  M = d\,A^\top D^{-1} + \frac{1-d}{N} \mathbf{1}\mathbf{1}^\top
\]

with explicit handling of dangling vertices (out-degree 0). At each step the
rank vector update is

\[
  \mathrm{pr}_{\text{new}}[v]
  = \frac{1-d}{N}
  + d \left( \sum_{u \in \mathrm{In}(v)} \frac{\mathrm{pr}_{\text{old}}[u]}{\mathrm{outdeg}(u)}
             + \frac{S_{\text{dangling}}}{N} \right)
\]

Convergence test: \(\lVert \mathrm{pr}_\text{new} - \mathrm{pr}_\text{old} \rVert_1 < \text{tol}\).

## Data structure

CSR **by destination** (in-edges per vertex). The inner loop of one update
streams `col_idx[row_ptr[v]..row_ptr[v+1])` contiguously, which is the layout
that gives the best cache and prefetch behaviour for the power-iteration
kernel. Out-degrees are kept in a separate `int32_t` array.

## Parallel strategies

| Backend | Granularity                      | Communication                     |
|---------|----------------------------------|-----------------------------------|
| serial  | -                                | -                                 |
| omp     | parallel-for over vertices       | shared memory                     |
| mpi     | 1-D vertex partition             | `Allreduce` + `Allgatherv` / iter |
| hybrid  | MPI ranks + OpenMP inside        | same as mpi                       |

The MPI version currently replicates the graph build on every rank to keep the
first milestone simple. Distributing the build (each rank reads only its slice)
is the natural follow-up once correctness is established.

## Things to investigate in the report

1. Strong scaling (fixed input) for OpenMP, MPI, hybrid.
2. Weak scaling using synthetic R-MAT graphs at fixed work per rank.
3. Sensitivity to graph topology (web vs social vs synthetic).
4. Overlap of `Allgatherv` with computation using non-blocking collectives.
5. Effect of NUMA placement (`OMP_PROC_BIND`, `OMP_PLACES`).
