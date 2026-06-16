/*
 * Hybrid MPI + OpenMP PageRank.
 *
 * This is the same algorithm as src/mpi/pagerank_mpi.c, except the per-rank
 * loop over owned vertices uses an OpenMP parallel-for with a reduction
 * for the L1 delta.  Build with `make hybrid`.
 *
 * Recommended runtime layout on Dardel:
 *   srun -n <ranks> -c <threads_per_rank> --cpus-per-task=<threads_per_rank>
 *   OMP_NUM_THREADS=<threads_per_rank> OMP_PROC_BIND=close OMP_PLACES=cores
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "graph.h"
#include "io.h"
#include "pagerank.h"

static void compute_partition(int64_t n, int p, int *counts, int *displs)
{
    int64_t base = n / p, rem = n % p, off = 0;
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

    Graph *g = io_load_edge_list(argv[1]);
    if (!g) { MPI_Abort(MPI_COMM_WORLD, 2); return 2; }
    if (rank == 0) {
        graph_print_stats(g);
#ifdef _OPENMP
        fprintf(stderr, "[hybrid] threads/rank=%d\n", omp_get_max_threads());
#endif
    }

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

    #pragma omp parallel for schedule(static)
    for (int64_t v = 0; v < n; ++v) pr_cur[v] = 1.0 / (double)n;

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    int iter = 0;
    for (; iter < max_iter; ++iter) {
        double local_dangling = 0.0;
        #pragma omp parallel for reduction(+:local_dangling) schedule(static)
        for (int v = v_lo; v < v_hi; ++v) {
            if (g->out_degree[v] == 0) local_dangling += pr_cur[v];
        }
        double dangling_sum = 0.0;
        MPI_Allreduce(&local_dangling, &dangling_sum, 1,
                      MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        const double dangling_contrib = d * dangling_sum / (double)n;

        double local_delta = 0.0;
        #pragma omp parallel for reduction(+:local_delta) schedule(static)
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

        MPI_Allgatherv(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL,
                       pr_nxt, counts, displs, MPI_DOUBLE,
                       MPI_COMM_WORLD);

        double delta = 0.0;
        MPI_Allreduce(&local_delta, &delta, 1,
                      MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

        double *tmp = pr_cur; pr_cur = pr_nxt; pr_nxt = tmp;
        if (params.verbose && rank == 0) {
            fprintf(stderr, "[hybrid] iter=%3d  delta=%.3e\n", iter + 1, delta);
        }
        if (delta < tol) { ++iter; break; }
    }

    double t1 = MPI_Wtime();
    if (rank == 0) {
        fprintf(stderr, "[hybrid] ranks=%d  iters=%d  time=%.4fs\n",
                size, iter, t1 - t0);
        if (argc >= 3) io_dump_ranks(argv[2], pr_cur, n);
    }

    free(pr_cur); free(pr_nxt);
    free(counts); free(displs);
    graph_free(g);
    MPI_Finalize();
    return 0;
}
