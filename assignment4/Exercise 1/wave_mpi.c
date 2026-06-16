#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define C 1.0
#define DT 0.01
#define DX 1.0

static int global_n = 1000;
static int global_steps = 100;
static int enable_io = 1;

static void parse_args(int argc, char **argv) {
    if (argc >= 2) global_n = atoi(argv[1]);
    if (argc >= 3) global_steps = atoi(argv[2]);
    if (argc >= 4) enable_io = atoi(argv[3]);

    const char *env_n = getenv("WAVE_N");
    const char *env_steps = getenv("WAVE_STEPS");
    const char *env_io = getenv("WAVE_IO");
    if (env_n) global_n = atoi(env_n);
    if (env_steps) global_steps = atoi(env_steps);
    if (env_io) enable_io = atoi(env_io);

    if (global_n < 3) global_n = 3;
    if (global_steps < 1) global_steps = 1;
}

static void partition_range(int rank, int nprocs, int n, int *start, int *local_n) {
    int base = n / nprocs;
    int rem = n % nprocs;
    if (rank < rem) {
        *local_n = base + 1;
        *start = rank * (base + 1);
    } else {
        *local_n = base;
        *start = rem * (base + 1) + (rank - rem) * base;
    }
}

static void exchange_halos(int rank, int nprocs, int local_n, double *u) {
    MPI_Status status;

    if (rank > 0) {
        MPI_Sendrecv(&u[1], 1, MPI_DOUBLE, rank - 1, 10,
                     &u[0], 1, MPI_DOUBLE, rank - 1, 11,
                     MPI_COMM_WORLD, &status);
    }
    if (rank < nprocs - 1) {
        MPI_Sendrecv(&u[local_n], 1, MPI_DOUBLE, rank + 1, 11,
                     &u[local_n + 1], 1, MPI_DOUBLE, rank + 1, 10,
                     MPI_COMM_WORLD, &status);
    }
}

static void compute_step_local(int rank, int nprocs, int start, int local_n,
                               const double *u, const double *u_prev, double *u_next) {
    const double coef = C * C * DT * DT / (DX * DX);
    (void)nprocs;

    for (int i = 1; i <= local_n; i++) {
        int g = start + i - 1;
        if (g <= 0 || g >= global_n - 1) {
            u_next[i] = u[i];
            continue;
        }
        u_next[i] = 2.0 * u[i] - u_prev[i]
                    + coef * (u[i + 1] - 2.0 * u[i] + u[i - 1]);
    }
}

static void write_output_parallel(int rank, int nprocs, int start, int local_n,
                                  const double *u, int step) {
    double *gather_buf = NULL;
    int *counts = NULL;
    int *displs = NULL;

    if (rank == 0) {
        gather_buf = (double *)malloc((size_t)global_n * sizeof(double));
        counts = (int *)malloc((size_t)nprocs * sizeof(int));
        displs = (int *)malloc((size_t)nprocs * sizeof(int));
    }

    for (int r = 0; r < nprocs; r++) {
        int s, ln;
        partition_range(r, nprocs, global_n, &s, &ln);
        if (rank == 0) {
            counts[r] = ln;
            displs[r] = s;
        }
    }

    MPI_Gatherv((void *)&u[1], local_n, MPI_DOUBLE,
                gather_buf, counts, displs, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    if (rank == 0) {
        char filename[64];
        snprintf(filename, sizeof(filename), "wave_output_%d.txt", step);
        FILE *f = fopen(filename, "w");
        for (int i = 0; i < global_n; i++) {
            fprintf(f, "%.17g\n", gather_buf[i]);
        }
        fclose(f);
        free(gather_buf);
        free(counts);
        free(displs);
    }
}

int main(int argc, char **argv) {
    int rank, nprocs;
    int start, local_n;
    double *u = NULL, *u_prev = NULL, *u_next = NULL;
    double t0, t1;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    parse_args(argc, argv);
    partition_range(rank, nprocs, global_n, &start, &local_n);

    int local_size = local_n + 2;
    u = (double *)calloc((size_t)local_size, sizeof(double));
    u_prev = (double *)calloc((size_t)local_size, sizeof(double));
    u_next = (double *)calloc((size_t)local_size, sizeof(double));

    for (int i = 1; i <= local_n; i++) {
        int g = start + i - 1;
        double val = sin(2.0 * M_PI * g / global_n);
        u[i] = val;
        u_prev[i] = val;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t0 = MPI_Wtime();

    for (int step = 0; step < global_steps; step++) {
        exchange_halos(rank, nprocs, local_n, u);
        compute_step_local(rank, nprocs, start, local_n, u, u_prev, u_next);

        for (int i = 1; i <= local_n; i++) {
            u_prev[i] = u[i];
            u[i] = u_next[i];
        }

        if (enable_io && step % 10 == 0) {
            write_output_parallel(rank, nprocs, start, local_n, u, step);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t1 = MPI_Wtime();

    if (rank == 0) {
        printf("WALLTIME %.9f\n", t1 - t0);
        printf("Simulation complete (N=%d, steps=%d, io=%d, nprocs=%d).\n",
               global_n, global_steps, enable_io, nprocs);
    }

    free(u);
    free(u_prev);
    free(u_next);
    MPI_Finalize();
    return 0;
}
