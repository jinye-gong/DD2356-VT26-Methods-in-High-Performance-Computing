#ifndef PR_GRAPH_H
#define PR_GRAPH_H

#include <stddef.h>
#include <stdint.h>

/*
 * CSR representation of a directed graph stored "by destination":
 *   row_ptr[v..v+1] enumerates the *in-edges* of vertex v
 *   col_idx[k]      is the source vertex of one such in-edge
 *
 * This layout is convenient for PageRank because the inner loop
 *   PR_new[v] = sum_{u in In(v)} PR_old[u] / outdeg(u)
 * touches a contiguous range of col_idx for each destination v.
 *
 * We also keep out-degrees of every vertex, which is needed by the
 * power-iteration update and to identify dangling nodes.
 */
typedef struct {
    int64_t  n;            /* number of vertices                   */
    int64_t  m;            /* number of directed edges             */
    int64_t *row_ptr;      /* size n+1, CSR by destination         */
    int32_t *col_idx;      /* size m,  source vertex per in-edge   */
    int32_t *out_degree;   /* size n,  out-degree of each vertex   */
    double  *inv_out_degree; /* size n, precomputed 1/outdeg (0 if dangling) */
} Graph;

/* Allocate / free. */
Graph *graph_alloc(int64_t n, int64_t m);
void   graph_free(Graph *g);

/* Build a CSR-by-destination graph from an edge list (src,dst) pair array.
 * `n_vertices` may be 0, in which case it is inferred as max(id)+1.
 * Self-loops are kept; duplicate edges are kept (caller's responsibility). */
Graph *graph_from_edges(const int32_t *src,
                        const int32_t *dst,
                        int64_t        n_edges,
                        int64_t        n_vertices);

/* Helpers shared by all backends. */
int64_t graph_count_dangling(const Graph *g);
void    graph_print_stats(const Graph *g);

#endif /* PR_GRAPH_H */
