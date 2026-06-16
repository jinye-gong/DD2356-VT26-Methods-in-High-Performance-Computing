#ifndef PR_IO_H
#define PR_IO_H

#include <stdint.h>
#include <stdio.h>

#include "graph.h"

/*
 * Load an edge list from `path`.
 *   - one edge per line: "src dst"   (whitespace separated, integer ids)
 *   - lines starting with '#' or '%' are treated as comments (SNAP style)
 *   - the parser is intentionally minimal; it is not a SNAP general-purpose
 *     reader but handles the SNAP plain-text edge lists used in this project.
 *
 * Returns a CSR-by-destination Graph or NULL on error.
 */
Graph *io_load_edge_list(const char *path);

/* Dump the final PageRank vector to `path` as "vertex_id<TAB>score" lines,
 * sorted by descending score.  Used for verification and reporting. */
int io_dump_ranks(const char *path, const double *pr, int64_t n);

#endif /* PR_IO_H */
