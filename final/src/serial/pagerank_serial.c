/*
 * Serial baseline PageRank via power iteration on a CSR-by-destination graph.
 *
 * Update rule (with damping d and N vertices):
 *
 *     PR_new[v] = (1 - d) / N
 *               + d * ( sum_{u in In(v)} PR_old[u] / outdeg(u)
 *                       + S_dangling / N )
 *
 * where S_dangling = sum of PR_old[u] over all u with outdeg(u) == 0.
 * Convergence test uses the L1 norm of (PR_new - PR_old).
 *
 * This is the reference all other backends are checked against.
 */

#include "pagerank.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pagerank_serial(const Graph    *g,
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

    for (int64_t v = 0; v < n; ++v) pr_cur[v] = 1.0 / (double)n;

    int iter = 0;
    for (; iter < max_iter; ++iter) {
        double dangling_sum = 0.0;
        for (int64_t v = 0; v < n; ++v) {
            if (g->out_degree[v] == 0) dangling_sum += pr_cur[v];
        }
        const double dangling_contrib = d * dangling_sum / (double)n;

        for (int64_t v = 0; v < n; ++v) {
            double s = 0.0;
            const int64_t beg = g->row_ptr[v];
            const int64_t end = g->row_ptr[v + 1];
            for (int64_t k = beg; k < end; ++k) {
                int32_t u = g->col_idx[k];
                s += pr_cur[u] * g->inv_out_degree[u];
            }
            pr_nxt[v] = teleport + dangling_contrib + d * s;
        }

        double delta = 0.0;
        for (int64_t v = 0; v < n; ++v) delta += fabs(pr_nxt[v] - pr_cur[v]);

        double *tmp = pr_cur; pr_cur = pr_nxt; pr_nxt = tmp;

        if (params->verbose) {
            fprintf(stderr, "[serial] iter=%3d  delta=%.3e\n", iter + 1, delta);
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

/* -------- standalone driver: ./pagerank_serial <edges> [out.txt] -------- */
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

    double t0   = pr_wtime();
    int    iter = pagerank_serial(g, &params, pr);
    double t1   = pr_wtime();

    fprintf(stderr, "[serial] iters=%d  time=%.4fs\n", iter, t1 - t0);

    if (argc >= 3) io_dump_ranks(argv[2], pr, g->n);

    free(pr);
    graph_free(g);
    return 0;
}
#endif
