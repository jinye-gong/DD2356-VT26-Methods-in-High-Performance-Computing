#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void fill_matrix(double *m, int n, unsigned int seed) {
    srand(seed);
    for (int i = 0; i < n * n; i++) {
        m[i] = (double)(rand() % 1000) / 1000.0;
    }
}

static void zero_matrix(double *m, int n) {
    memset(m, 0, (size_t)n * (size_t)n * sizeof(double));
}

static void matmul_serial(const double *A, const double *B, double *C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

/* Task 4: OpenMP target offload version */
static void matmul_omp_gpu(const double *A, const double *B, double *C, int n) {
#ifndef DISABLE_OFFLOAD
#pragma omp target data map(to : A[0:n * n], B[0:n * n]) map(from : C[0:n * n])
    {
#pragma omp target teams distribute parallel for collapse(2)
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                double sum = 0.0;
#pragma omp simd reduction(+ : sum)
                for (int k = 0; k < n; k++) {
                    sum += A[i * n + k] * B[k * n + j];
                }
                C[i * n + j] = sum;
            }
        }
    }
#else
#pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
#pragma omp simd reduction(+ : sum)
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
#endif
}

static int verify(const double *ref, const double *test, int n) {
    const double eps = 1e-8;
    for (int i = 0; i < n * n; i++) {
        if (fabs(ref[i] - test[i]) > eps) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char **argv) {
    int n = 1024;
    if (argc > 1) {
        n = atoi(argv[1]);
        if (n <= 0) {
            fprintf(stderr, "Invalid matrix size: %s\n", argv[1]);
            return 1;
        }
    }

    size_t bytes = (size_t)n * (size_t)n * sizeof(double);
    double *A = (double *)malloc(bytes);
    double *B = (double *)malloc(bytes);
    double *C_serial = (double *)malloc(bytes);
    double *C_gpu = (double *)malloc(bytes);

    if (!A || !B || !C_serial || !C_gpu) {
        fprintf(stderr, "Memory allocation failed for n=%d\n", n);
        free(A);
        free(B);
        free(C_serial);
        free(C_gpu);
        return 1;
    }

    fill_matrix(A, n, 42U);
    fill_matrix(B, n, 7U);
    zero_matrix(C_serial, n);
    zero_matrix(C_gpu, n);

    double t0 = omp_get_wtime();
    matmul_serial(A, B, C_serial, n);
    double t1 = omp_get_wtime();

    int devices = omp_get_num_devices();
    double t2 = omp_get_wtime();
    matmul_omp_gpu(A, B, C_gpu, n);
    double t3 = omp_get_wtime();

    int ok = verify(C_serial, C_gpu, n);
    printf("Matrix size: %d x %d\n", n, n);
    printf("OpenMP target devices: %d\n", devices);
    printf("Serial time:   %.6f s\n", t1 - t0);
    printf("OMP time:      %.6f s\n", t3 - t2);
    printf("Speedup:       %.3f x\n", (t1 - t0) / (t3 - t2));
    printf("Verification:  %s\n", ok ? "PASS" : "FAIL");

    free(A);
    free(B);
    free(C_serial);
    free(C_gpu);
    return ok ? 0 : 2;
}
