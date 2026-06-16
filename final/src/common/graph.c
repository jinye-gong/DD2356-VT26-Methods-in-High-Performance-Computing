#include "graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Graph *graph_alloc(int64_t n, int64_t m)
{
    Graph *g = (Graph *)calloc(1, sizeof(Graph));
    if (!g) return NULL;
    g->n          = n;
    g->m          = m;
    g->row_ptr    = (int64_t *)calloc((size_t)(n + 1), sizeof(int64_t));
    g->col_idx    = (int32_t *)calloc((size_t)(m > 0 ? m : 1), sizeof(int32_t));
    g->out_degree = (int32_t *)calloc((size_t)(n > 0 ? n : 1), sizeof(int32_t));
    g->inv_out_degree = (double *)calloc((size_t)(n > 0 ? n : 1), sizeof(double));
    if (!g->row_ptr || !g->col_idx || !g->out_degree || !g->inv_out_degree) {
        graph_free(g);
        return NULL;
    }
    return g;
}

void graph_free(Graph *g)
{
    if (!g) return;
    free(g->row_ptr);
    free(g->col_idx);
    free(g->out_degree);
    free(g->inv_out_degree);
    free(g);
}

Graph *graph_from_edges(const int32_t *src,
                        const int32_t *dst,
                        int64_t        n_edges,
                        int64_t        n_vertices)
{
    if (n_vertices <= 0) {
        int32_t max_id = -1;
        for (int64_t e = 0; e < n_edges; ++e) {
            if (src[e] > max_id) max_id = src[e];
            if (dst[e] > max_id) max_id = dst[e];
        }
        n_vertices = (int64_t)max_id + 1;
    }

    Graph *g = graph_alloc(n_vertices, n_edges);
    if (!g) return NULL;

    /* Histogram of in-degrees -> row_ptr; out-degrees collected separately. */
    for (int64_t e = 0; e < n_edges; ++e) {
        g->row_ptr[dst[e] + 1] += 1;
        g->out_degree[src[e]]  += 1;
    }
    for (int64_t v = 0; v < n_vertices; ++v) {
        if (g->out_degree[v] > 0) {
            g->inv_out_degree[v] = 1.0 / (double)g->out_degree[v];
        } else {
            g->inv_out_degree[v] = 0.0;
        }
    }
    for (int64_t v = 0; v < n_vertices; ++v) {
        g->row_ptr[v + 1] += g->row_ptr[v];
    }

    /* Scatter sources into col_idx using a moving cursor. */
    int64_t *cursor = (int64_t *)malloc((size_t)n_vertices * sizeof(int64_t));
    if (!cursor) { graph_free(g); return NULL; }
    memcpy(cursor, g->row_ptr, (size_t)n_vertices * sizeof(int64_t));
    for (int64_t e = 0; e < n_edges; ++e) {
        int32_t d = dst[e];
        g->col_idx[cursor[d]++] = src[e];
    }
    free(cursor);
    return g;
}

int64_t graph_count_dangling(const Graph *g)
{
    int64_t c = 0;
    for (int64_t v = 0; v < g->n; ++v) {
        if (g->out_degree[v] == 0) ++c;
    }
    return c;
}

void graph_print_stats(const Graph *g)
{
    int64_t dangling = graph_count_dangling(g);
    fprintf(stderr,
            "[graph] n=%lld  m=%lld  dangling=%lld  avg_deg=%.2f\n",
            (long long)g->n,
            (long long)g->m,
            (long long)dangling,
            g->n ? (double)g->m / (double)g->n : 0.0);
}
