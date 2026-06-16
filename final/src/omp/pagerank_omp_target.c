/*
 * OpenMP target-offload PageRank.
 *
 * This implementation keeps the same algorithm as the CPU OpenMP version,
 * but uses `target teams distribute parallel for` in the core loops so it
 * can run on GPU when an OpenMP offload runtime/device is available.
 *
 * If no offload device exists, OpenMP target typically falls back to host.
 */

#include "pagerank.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _OPENMP
#include <omp.h>
#endif

int pagerank_omp_target(const Graph *g, const PRParams *params, double *pr_out)
{
    const int64_t n        = g->n;
    const double  d        = params->damping;
    const double  tol      = params->tol;
    const int     max_iter = params->max_iter;
    const double  teleport = (1.0 - d) / (double)n;

    double *pr_cur = pr_out;
    double *pr_nxt = (double *)malloc((size_t)n * sizeof(double));
    if (!pr_nxt) return -1;

#ifdef _OPENMP
    int ndev = omp_get_num_devices();
    fprintf(stderr, "[omp-target] detected devices=%d\n", ndev);
#endif

    #pragma omp target teams distribute parallel for map(from:pr_cur[0:n])
    for (int64_t v = 0; v < n; ++v) pr_cur[v] = 1.0 / (double)n;

    int iter = 0;
    #pragma omp target data map(to: g->row_ptr[0:n+1], g->col_idx[0:g->m], g->out_degree[0:n], g->inv_out_degree[0:n]) \
                            map(tofrom: pr_cur[0:n], pr_nxt[0:n])
    {
        for (; iter < max_iter; ++iter) {
            double dangling_sum = 0.0;
            #pragma omp target teams distribute parallel for reduction(+:dangling_sum)
            for (int64_t v = 0; v < n; ++v) {
                if (g->out_degree[v] == 0) dangling_sum += pr_cur[v];
            }
            const double dangling_contrib = d * dangling_sum / (double)n;

            double delta = 0.0;
            #pragma omp target teams distribute parallel for reduction(+:delta)
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
                fprintf(stderr, "[omp-target] iter=%3d delta=%.3e\n", iter + 1, delta);
            }
            if (delta < tol) { ++iter; break; }
        }
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

    PRParams params = pr_default_params();
    if (getenv("PR_VERBOSE")) params.verbose = 1;

    double *pr = (double *)malloc((size_t)g->n * sizeof(double));
    if (!pr) { graph_free(g); return 3; }

    double t0 = pr_wtime();
    int iter = pagerank_omp_target(g, &params, pr);
    double t1 = pr_wtime();
    fprintf(stderr, "[omp-target] iters=%d time=%.4fs\n", iter, t1 - t0);

    if (argc >= 3) io_dump_ranks(argv[2], pr, g->n);
    free(pr);
    graph_free(g);
    return 0;
}
#endif

