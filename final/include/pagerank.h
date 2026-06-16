#ifndef PR_PAGERANK_H
#define PR_PAGERANK_H

#include <stdint.h>

#include "graph.h"

/* Algorithm parameters shared by every backend. */
typedef struct {
    double damping;     /* d, typical 0.85                          */
    double tol;         /* L1 convergence tolerance, e.g. 1e-7      */
    int    max_iter;    /* hard cap on power-iteration steps        */
    int    verbose;     /* if non-zero, print per-iteration delta   */
} PRParams;

/* Default parameters: d=0.85, tol=1e-7, max_iter=100, verbose=0. */
PRParams pr_default_params(void);

/* Serial reference implementation.  Writes the rank vector into `pr_out`
 * (caller-allocated, size g->n).  Returns the number of iterations run. */
int pagerank_serial(const Graph    *g,
                    const PRParams *params,
                    double         *pr_out);

/* OpenMP shared-memory implementation, same interface as the serial one. */
int pagerank_omp(const Graph    *g,
                 const PRParams *params,
                 double         *pr_out);

#endif /* PR_PAGERANK_H */
