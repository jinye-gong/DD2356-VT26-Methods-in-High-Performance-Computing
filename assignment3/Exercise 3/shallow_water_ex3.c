#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline size_t idx(int i, int j, int n) {
    return (size_t)i * (size_t)n + (size_t)j;
}

static void initialize(double *h, double *u, double *v, int n) {
#pragma omp parallel for collapse(2) schedule(static)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            h[idx(i, j, n)] = 1.0;
            u[idx(i, j, n)] = 0.0;
            v[idx(i, j, n)] = 0.0;
        }
    }
}

static void copy_boundaries(double *dst, const double *src, int n) {
#pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++) {
        dst[idx(i, 0, n)] = src[idx(i, 0, n)];
        dst[idx(i, n - 1, n)] = src[idx(i, n - 1, n)];
        dst[idx(0, i, n)] = src[idx(0, i, n)];
        dst[idx(n - 1, i, n)] = src[idx(n - 1, i, n)];
    }
}

static void write_output(const char *path, const double *h, int n) {
    FILE *f = fopen(path, "w");
    if (!f) {
        fprintf(stderr, "Failed to open output file: %s\n", path);
        return;
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fprintf(f, "%.6f ", h[idx(i, j, n)]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

static omp_sched_t parse_schedule(const char *s) {
    if (strcmp(s, "static") == 0) {
        return omp_sched_static;
    }
    if (strcmp(s, "dynamic") == 0) {
        return omp_sched_dynamic;
    }
    if (strcmp(s, "guided") == 0) {
        return omp_sched_guided;
    }
    fprintf(stderr, "Unknown schedule '%s', fallback to static.\n", s);
    return omp_sched_static;
}

int main(int argc, char **argv) {
    int n = 500;
    int iter = 1000;
    const double dt = 0.01;
    const double dx = 1.0;
    const char *sched_name = "static";
    const char *output_path = "output.txt";

    if (argc > 1) n = atoi(argv[1]);
    if (argc > 2) iter = atoi(argv[2]);
    if (argc > 3) sched_name = argv[3];
    if (argc > 4) output_path = argv[4];
    if (n < 4 || iter < 1) {
        fprintf(stderr, "Usage: ./shallow_water_ex3 [N] [ITER] [static|dynamic|guided] [output.txt]\n");
        return 1;
    }

    omp_set_schedule(parse_schedule(sched_name), 32);

    size_t bytes = (size_t)n * (size_t)n * sizeof(double);
    double *h = (double *)malloc(bytes);
    double *h_new = (double *)malloc(bytes);
    double *u = (double *)malloc(bytes);
    double *v = (double *)malloc(bytes);
    if (!h || !h_new || !u || !v) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(h);
        free(h_new);
        free(u);
        free(v);
        return 1;
    }

    initialize(h, u, v, n);
    memcpy(h_new, h, bytes);

    double t0 = omp_get_wtime();
    for (int it = 0; it < iter; it++) {
#pragma omp parallel for collapse(2) schedule(runtime)
        for (int i = 1; i < n - 1; i++) {
            for (int j = 1; j < n - 1; j++) {
                double dudx = (u[idx(i + 1, j, n)] - u[idx(i - 1, j, n)]) / (2.0 * dx);
                double dvdy = (v[idx(i, j + 1, n)] - v[idx(i, j - 1, n)]) / (2.0 * dx);
                h_new[idx(i, j, n)] = h[idx(i, j, n)] - dt * (dudx + dvdy);
            }
        }
        copy_boundaries(h_new, h, n);
        double *tmp = h;
        h = h_new;
        h_new = tmp;
    }
    double t1 = omp_get_wtime();

    write_output(output_path, h, n);

    printf("Grid size:     %d x %d\n", n, n);
    printf("Iterations:    %d\n", iter);
    printf("OMP threads:   %d\n", omp_get_max_threads());
    printf("Schedule:      %s\n", sched_name);
    printf("OMP time:      %.6f s\n", t1 - t0);
    printf("Output:        %s\n", output_path);

    free(h);
    free(h_new);
    free(u);
    free(v);
    return 0;
}
