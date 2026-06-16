#include <limits.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int n;
    int m;
    int *row_ptr;
    int *col_idx;
} GraphCSR;

static void die(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

static GraphCSR generate_random_graph(int n, int degree, unsigned int seed) {
    GraphCSR g;
    g.n = n;
    g.m = n * degree;
    g.row_ptr = (int *)malloc((size_t)(n + 1) * sizeof(int));
    g.col_idx = (int *)malloc((size_t)g.m * sizeof(int));
    if (!g.row_ptr || !g.col_idx) {
        die("Failed to allocate graph memory");
    }

    srand(seed);
    g.row_ptr[0] = 0;
    for (int i = 0; i < n; i++) {
        g.row_ptr[i + 1] = g.row_ptr[i] + degree;
        for (int d = 0; d < degree; d++) {
            int v = rand() % n;
            if (v == i) {
                v = (v + 1) % n;
            }
            g.col_idx[g.row_ptr[i] + d] = v;
        }
    }
    return g;
}

static void free_graph(GraphCSR *g) {
    free(g->row_ptr);
    free(g->col_idx);
    g->row_ptr = NULL;
    g->col_idx = NULL;
}

static void bfs_serial(const GraphCSR *g, int source, int *dist) {
    int n = g->n;
    int *queue = (int *)malloc((size_t)n * sizeof(int));
    if (!queue) {
        die("Failed to allocate serial queue");
    }

    for (int i = 0; i < n; i++) {
        dist[i] = -1;
    }
    int head = 0, tail = 0;
    queue[tail++] = source;
    dist[source] = 0;

    while (head < tail) {
        int u = queue[head++];
        int du = dist[u];
        for (int e = g->row_ptr[u]; e < g->row_ptr[u + 1]; e++) {
            int v = g->col_idx[e];
            if (dist[v] == -1) {
                dist[v] = du + 1;
                queue[tail++] = v;
            }
        }
    }
    free(queue);
}

static void bfs_omp_parallel(const GraphCSR *g, int source, int *dist) {
    int n = g->n;
    int *frontier = (int *)malloc((size_t)n * sizeof(int));
    int *next_frontier = (int *)malloc((size_t)n * sizeof(int));
    if (!frontier || !next_frontier) {
        die("Failed to allocate frontier buffers");
    }

    for (int i = 0; i < n; i++) {
        dist[i] = -1;
    }
    int frontier_size = 1;
    frontier[0] = source;
    dist[source] = 0;
    int level = 0;

    while (frontier_size > 0) {
        int next_size = 0;

#pragma omp parallel
        {
            int *local_next = (int *)malloc((size_t)n * sizeof(int));
            int local_count = 0;
            if (!local_next) {
                die("Failed to allocate local frontier");
            }

#pragma omp for schedule(dynamic, 64)
            for (int i = 0; i < frontier_size; i++) {
                int u = frontier[i];
                for (int e = g->row_ptr[u]; e < g->row_ptr[u + 1]; e++) {
                    int v = g->col_idx[e];
                    if (__sync_bool_compare_and_swap(&dist[v], -1, level + 1)) {
                        local_next[local_count++] = v;
                    }
                }
            }

            int base;
#pragma omp atomic capture
            {
                base = next_size;
                next_size += local_count;
            }
            memcpy(next_frontier + base, local_next, (size_t)local_count * sizeof(int));
            free(local_next);
        }

        int *tmp = frontier;
        frontier = next_frontier;
        next_frontier = tmp;
        frontier_size = next_size;
        level++;
    }

    free(frontier);
    free(next_frontier);
}

static int verify_result(const int *a, const int *b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char **argv) {
    int n = 50000;
    int degree = 8;
    if (argc > 1) {
        n = atoi(argv[1]);
    }
    if (argc > 2) {
        degree = atoi(argv[2]);
    }
    if (n <= 1 || degree <= 0) {
        die("Usage: ./bfs_ex2_task1_parallel [num_vertices] [degree]");
    }

    GraphCSR g = generate_random_graph(n, degree, 2026U);
    int *dist_serial = (int *)malloc((size_t)n * sizeof(int));
    int *dist_parallel = (int *)malloc((size_t)n * sizeof(int));
    if (!dist_serial || !dist_parallel) {
        die("Failed to allocate distance arrays");
    }

    double t0 = omp_get_wtime();
    bfs_serial(&g, 0, dist_serial);
    double t1 = omp_get_wtime();

    double t2 = omp_get_wtime();
    bfs_omp_parallel(&g, 0, dist_parallel);
    double t3 = omp_get_wtime();

    int ok = verify_result(dist_serial, dist_parallel, n);
    int visited = 0;
    for (int i = 0; i < n; i++) {
        if (dist_parallel[i] >= 0) {
            visited++;
        }
    }

    printf("Graph: vertices=%d edges=%d degree=%d\n", n, g.m, degree);
    printf("OMP max threads: %d\n", omp_get_max_threads());
    printf("Serial time:   %.6f s\n", t1 - t0);
    printf("OMP time:      %.6f s\n", t3 - t2);
    printf("Speedup:       %.3f x\n", (t1 - t0) / (t3 - t2));
    printf("Visited:       %d\n", visited);
    printf("Verification:  %s\n", ok ? "PASS" : "FAIL");

    free(dist_serial);
    free(dist_parallel);
    free_graph(&g);
    return ok ? 0 : 2;
}
