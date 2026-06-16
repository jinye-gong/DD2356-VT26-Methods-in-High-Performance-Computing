#include "io.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Graph *io_load_edge_list(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "[io] cannot open %s\n", path);
        return NULL;
    }

    /* Two-pass over the file: first pass counts edges, second pass fills.
     * This avoids one large realloc churn on multi-GB inputs.            */
    int64_t cap_guess = 1 << 14;
    int64_t n_edges   = 0;
    int32_t *src = (int32_t *)malloc((size_t)cap_guess * sizeof(int32_t));
    int32_t *dst = (int32_t *)malloc((size_t)cap_guess * sizeof(int32_t));
    if (!src || !dst) { fclose(f); free(src); free(dst); return NULL; }

    char    line[1024];
    int32_t max_id = -1;
    while (fgets(line, sizeof line, f)) {
        char *p = line;
        while (*p && isspace((unsigned char)*p)) ++p;
        if (*p == '\0' || *p == '#' || *p == '%') continue;

        int32_t s, d;
        if (sscanf(p, "%d %d", &s, &d) != 2) continue;

        if (n_edges == cap_guess) {
            cap_guess *= 2;
            src = (int32_t *)realloc(src, (size_t)cap_guess * sizeof(int32_t));
            dst = (int32_t *)realloc(dst, (size_t)cap_guess * sizeof(int32_t));
            if (!src || !dst) { fclose(f); free(src); free(dst); return NULL; }
        }
        src[n_edges] = s;
        dst[n_edges] = d;
        if (s > max_id) max_id = s;
        if (d > max_id) max_id = d;
        ++n_edges;
    }
    fclose(f);

    Graph *g = graph_from_edges(src, dst, n_edges, (int64_t)max_id + 1);
    free(src);
    free(dst);
    return g;
}

typedef struct { int32_t id; double score; } RankRow;
static int cmp_rank_desc(const void *a, const void *b)
{
    double sa = ((const RankRow *)a)->score;
    double sb = ((const RankRow *)b)->score;
    return (sa < sb) - (sa > sb);
}

int io_dump_ranks(const char *path, const double *pr, int64_t n)
{
    FILE *f = fopen(path, "w");
    if (!f) return -1;

    RankRow *rows = (RankRow *)malloc((size_t)n * sizeof(RankRow));
    if (!rows) { fclose(f); return -1; }
    for (int64_t v = 0; v < n; ++v) { rows[v].id = (int32_t)v; rows[v].score = pr[v]; }
    qsort(rows, (size_t)n, sizeof(RankRow), cmp_rank_desc);

    for (int64_t i = 0; i < n; ++i) {
        fprintf(f, "%d\t%.12e\n", rows[i].id, rows[i].score);
    }
    free(rows);
    fclose(f);
    return 0;
}
