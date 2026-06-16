/*
 * OpenMP shared-memory PageRank.
 *
 * Parallel structure (each iteration):
 *   1. parallel reduction over dangling-vertex PR mass
 *   2. parallel for over destination vertices v (the outer loop in serial)
 *   3. parallel reduction for the L1 convergence delta
 *
 * The destination-by-destination loop is naturally race-free: every thread
 * writes its own pr_nxt[v] and only reads pr_cur[u], so no atomics are
 * needed.  Static scheduling is a good default for power-law graphs; the
 * Makefile lets you override with OMP_SCHEDULE at run time.
 */

#include "pagerank.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _OPENMP
#include <omp.h>
#endif

int pagerank_omp(const Graph    *g,
                 const PRParams *params,
                 double         *pr_out)
{
    const int64_t  n         = g->n;
    const double   d         = params->damping;
    const double   tol       = params->tol;
    const int      max_iter  = params->max_iter;
    const double   teleport  = (1.0 - d) / (double)n;

    double *pr_cur = pr_out;
    double *pr_nxt = (double *)malloc((size_t)n * sizeof(double));
    if (!pr_nxt) return -1;

    #pragma omp parallel for schedule(static)
    for (int64_t v = 0; v < n; ++v) pr_cur[v] = 1.0 / (double)n;

    int iter = 0;
    for (; iter < max_iter; ++iter) {
        double dangling_sum = 0.0;
        #pragma omp parallel for reduction(+:dangling_sum) schedule(static)
        for (int64_t v = 0; v < n; ++v) {
            if (g->out_degree[v] == 0) dangling_sum += pr_cur[v];
        }
        const double dangling_contrib = d * dangling_sum / (double)n;

        double delta = 0.0;
        #pragma omp parallel for reduction(+:delta) schedule(static)
        for (int64_t v = 0; v < n; ++v) {
            double s = 0.0;
            const int64_t beg = g->row_ptr[v];
            const int64_t end = g->row_ptr[v + 1];
            for (int64_t k = beg; k < end; ++k) {
                int32_t u = g->col_idx[k];
                s += pr_cur[u] * g->inv_out_degree[u];
            }
            double v_new = teleport + dangling_contrib + d * s;
            delta       += fabs(v_new - pr_cur[v]);
            pr_nxt[v]    = v_new;
        }

        double *tmp = pr_cur; pr_cur = pr_nxt; pr_nxt = tmp;

        if (params->verbose) {
            fprintf(stderr, "[omp] iter=%3d  delta=%.3e\n", iter + 1, delta);
        }
        if (delta < tol) { ++iter; break; }
    }

    if (pr_cur != pr_out) {
        memcpy(pr_out, pr_cur, (size_t)n * sizeof(double));
        free(pr_cur);
    } else {
        free(pr_nxt);
    }
    return iter;
}

#ifndef PR_NO_MAIN
#include "io.h"
#include "timer.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s <edge_list> [out.txt]\n", argv[0]);
        return 1;
    }
    Graph *g = io_load_edge_list(argv[1]);
    if (!g) return 2;
    graph_print_stats(g);

#ifdef _OPENMP
    fprintf(stderr, "[omp] max_threads=%d\n", omp_get_max_threads());
#endif

    PRParams params = pr_default_params();
    if (getenv("PR_VERBOSE")) params.verbose = 1;

    double *pr = (double *)malloc((size_t)g->n * sizeof(double));
    if (!pr) { graph_free(g); return 3; }

    double t0   = pr_wtime();
    int    iter = pagerank_omp(g, &params, pr);
    double t1   = pr_wtime();

    fprintf(stderr, "[omp] iters=%d  time=%.4fs\n", iter, t1 - t0);

    if (argc >= 3) io_dump_ranks(argv[2], pr, g->n);

    free(pr);
    graph_free(g);
    return 0;
}
#endif
