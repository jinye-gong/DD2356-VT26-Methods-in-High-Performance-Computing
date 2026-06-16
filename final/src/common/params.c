#include "pagerank.h"

PRParams pr_default_params(void)
{
    PRParams p;
    p.damping  = 0.85;
    p.tol      = 1e-7;
    p.max_iter = 100;
    p.verbose  = 0;
    return p;
}
