/*
 * Distributed-memory PageRank skeleton (MPI).
 *
 * Partitioning strategy used here (1-D row block):
 *   - Vertices [0..n) are split into P contiguous chunks of (almost) equal
 *     size; rank r owns vertices [v_lo[r], v_hi[r]).
 *   - Each rank loads the FULL edge list, builds the FULL CSR-by-destination
 *     graph, but only updates pr[v] for its own vertex range.  This is
 *     intentionally simple for the first MPI milestone: it lets us validate
 *     correctness and measure compute-vs-communication balance without yet
 *     paying for a distributed graph build.
 *   - After every iteration each rank holds an updated slice of the rank
 *     vector; we MPI_Allgatherv it so every rank can read the latest pr_cur
 *     in the next iteration.
 *
 * Planned extensions (left as TODOs for the project):
 *   - Distributed graph load (each rank reads its assigned slice only)
 *   - 2-D / edge-cut partitioning to reduce communication volume
 *   - Overlap of Allgatherv with local computation via non-blocking colls
 *   - Hybrid MPI + OpenMP inner loop (see src/hybrid)
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>

#include "graph.h"
#include "io.h"
#include "pagerank.h"

static void compute_partition(int64_t n, int p, int *counts, int *displs)
{
    int64_t base = n / p;
    int64_t rem  = n % p;
    int64_t off  = 0;
    for (int r = 0; r < p; ++r) {
        int64_t c = base + (r < rem ? 1 : 0);
        counts[r] = (int)c;
        displs[r] = (int)off;
        off      += c;
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0) fprintf(stderr, "usage: %s <edge_list> [out.txt]\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    /* Phase 1 (TODO): replace the replicated load below with a distributed
     * reader.  For now every rank parses the whole file. */
    Graph *g = io_load_edge_list(argv[1]);
    if (!g) { MPI_Abort(MPI_COMM_WORLD, 2); return 2; }
    if (rank == 0) graph_print_stats(g);

    const int64_t n = g->n;
    int *counts = (int *)malloc((size_t)size * sizeof(int));
    int *displs = (int *)malloc((size_t)size * sizeof(int));
    compute_partition(n, size, counts, displs);
    const int v_lo = displs[rank];
    const int v_hi = v_lo + counts[rank];

    PRParams params = pr_default_params();
    if (getenv("PR_VERBOSE") && rank == 0) params.verbose = 1;

    const double d        = params.damping;
    const double tol      = params.tol;
    const int    max_iter = params.max_iter;
    const double teleport = (1.0 - d) / (double)n;

    double *pr_cur = (double *)malloc((size_t)n * sizeof(double));
    double *pr_nxt = (double *)malloc((size_t)n * sizeof(double));
    if (!pr_cur || !pr_nxt) MPI_Abort(MPI_COMM_WORLD, 3);
    for (int64_t v = 0; v < n; ++v) pr_cur[v] = 1.0 / (double)n;

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    int iter = 0;
    for (; iter < max_iter; ++iter) {
        /* Dangling-mass reduction is global. */
        double local_dangling = 0.0;
        for (int v = v_lo; v < v_hi; ++v) {
            if (g->out_degree[v] == 0) local_dangling += pr_cur[v];
        }
        double dangling_sum = 0.0;
        MPI_Allreduce(&local_dangling, &dangling_sum, 1,
                      MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        const double dangling_contrib = d * dangling_sum / (double)n;

        /* Local update for the owned vertex slice. */
        double local_delta = 0.0;
        for (int v = v_lo; v < v_hi; ++v) {
            double s = 0.0;
            const int64_t beg = g->row_ptr[v];
            const int64_t end = g->row_ptr[v + 1];
            for (int64_t k = beg; k < end; ++k) {
                int32_t u = g->col_idx[k];
                s += pr_cur[u] * g->inv_out_degree[u];
            }
            double v_new = teleport + dangling_contrib + d * s;
            local_delta += fabs(v_new - pr_cur[v]);
            pr_nxt[v]    = v_new;
        }

        /* Gather all slices into every rank's pr_nxt -> next pr_cur.        */
        MPI_Allgatherv(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL,
                       pr_nxt, counts, displs, MPI_DOUBLE,
                       MPI_COMM_WORLD);

        double delta = 0.0;
        MPI_Allreduce(&local_delta, &delta, 1,
                      MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

        double *tmp = pr_cur; pr_cur = pr_nxt; pr_nxt = tmp;

        if (params.verbose && rank == 0) {
            fprintf(stderr, "[mpi] iter=%3d  delta=%.3e\n", iter + 1, delta);
        }
        if (delta < tol) { ++iter; break; }
    }

    double t1 = MPI_Wtime();
    if (rank == 0) {
        fprintf(stderr, "[mpi] ranks=%d  iters=%d  time=%.4fs\n",
                size, iter, t1 - t0);
        if (argc >= 3) io_dump_ranks(argv[2], pr_cur, n);
    }

    free(pr_cur); free(pr_nxt);
    free(counts); free(displs);
    graph_free(g);
    MPI_Finalize();
    return 0;
}
