#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

static int global_n = 1000;
static int enable_io = 1;

static void parse_args(int argc, char **argv) {
    if (argc >= 2) global_n = atoi(argv[1]);
    if (argc >= 3) enable_io = atoi(argv[2]);

    const char *env_n = getenv("WEAK_N");
    const char *env_io = getenv("ROWSUM_IO");
    if (env_n) global_n = atoi(env_n);
    if (env_io) enable_io = atoi(env_io);

    if (global_n < 1) global_n = 1;
}

static int local_row_count(int rank, int nprocs, int n) {
    int base = n / nprocs;
    int rem = n % nprocs;
    return base + (rank < rem ? 1 : 0);
}

static int row_start(int rank, int nprocs, int n) {
    int base = n / nprocs;
    int rem = n % nprocs;
    if (rank < rem) return rank * (base + 1);
    return rem * (base + 1) + (rank - rem) * base;
}

static void initialize_matrix_rows(double *rows, int row_start_idx, int nrows, int ncols) {
    for (int i = 0; i < nrows; i++) {
        int gi = row_start_idx + i;
        for (int j = 0; j < ncols; j++) {
            rows[i * ncols + j] = (double)gi + j * 0.01;
        }
    }
}

static void compute_row_sums_local(const double *matrix, double *row_sums,
                                 int nrows, int ncols) {
    for (int i = 0; i < nrows; i++) {
        row_sums[i] = 0.0;
        for (int j = 0; j < ncols; j++) {
            row_sums[i] += matrix[i * ncols + j];
        }
    }
}

static double local_matrix_total(const double *matrix, int nrows, int ncols) {
    double total = 0.0;
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            total += matrix[i * ncols + j];
        }
    }
    return total;
}

static void write_output(const double *row_sums, int n) {
    FILE *f = fopen("row_sums_output.txt", "w");
    if (!f) return;
    for (int i = 0; i < n; i++) {
        fprintf(f, "%.17g\n", row_sums[i]);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    int rank, nprocs;
    int local_rows, start_row;
    size_t local_elems;
    double *full_matrix = NULL;
    double *local_matrix = NULL;
    double *local_row_sums = NULL;
    double *all_row_sums = NULL;
    double local_total = 0.0, global_total = 0.0;
    double t0, t1;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    parse_args(argc, argv);
    local_rows = local_row_count(rank, nprocs, global_n);
    start_row = row_start(rank, nprocs, global_n);
    local_elems = (size_t)local_rows * (size_t)global_n;

    if (rank == 0) {
        full_matrix = (double *)malloc((size_t)global_n * (size_t)global_n * sizeof(double));
        if (!full_matrix) {
            fprintf(stderr, "Rank 0: failed to allocate matrix N=%d\n", global_n);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        initialize_matrix_rows(full_matrix, 0, global_n, global_n);
    }

    local_matrix = (double *)malloc(local_elems * sizeof(double));
    local_row_sums = (double *)malloc((size_t)local_rows * sizeof(double));
    if (!local_matrix || !local_row_sums) {
        fprintf(stderr, "Rank %d: allocation failed\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t0 = MPI_Wtime();

    {
        int *sendcounts = NULL;
        int *displs = NULL;
        if (rank == 0) {
            sendcounts = (int *)malloc((size_t)nprocs * sizeof(int));
            displs = (int *)malloc((size_t)nprocs * sizeof(int));
            for (int r = 0; r < nprocs; r++) {
                int lr = local_row_count(r, nprocs, global_n);
                sendcounts[r] = lr * global_n;
                displs[r] = row_start(r, nprocs, global_n) * global_n;
            }
        }
        MPI_Scatterv(full_matrix, sendcounts, displs, MPI_DOUBLE,
                     local_matrix, (int)local_elems, MPI_DOUBLE,
                     0, MPI_COMM_WORLD);
        if (rank == 0) {
            free(sendcounts);
            free(displs);
        }
    }

    compute_row_sums_local(local_matrix, local_row_sums, local_rows, global_n);
    local_total = local_matrix_total(local_matrix, local_rows, global_n);

    if (rank == 0) {
        all_row_sums = (double *)malloc((size_t)global_n * sizeof(double));
        if (!all_row_sums) MPI_Abort(MPI_COMM_WORLD, 1);
    }

    {
        int *recvcounts = NULL;
        int *displs = NULL;
        if (rank == 0) {
            recvcounts = (int *)malloc((size_t)nprocs * sizeof(int));
            displs = (int *)malloc((size_t)nprocs * sizeof(int));
            for (int r = 0; r < nprocs; r++) {
                recvcounts[r] = local_row_count(r, nprocs, global_n);
                displs[r] = row_start(r, nprocs, global_n);
            }
        }
        MPI_Gatherv(local_row_sums, local_rows, MPI_DOUBLE,
                    all_row_sums, recvcounts, displs, MPI_DOUBLE,
                    0, MPI_COMM_WORLD);
        if (rank == 0) {
            free(recvcounts);
            free(displs);
        }
    }

    MPI_Reduce(&local_total, &global_total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    t1 = MPI_Wtime();

    if (rank == 0) {
        if (enable_io) write_output(all_row_sums, global_n);
        printf("WALLTIME %.9f\n", t1 - t0);
        printf("Row sum complete (N=%d, nprocs=%d, io=%d).\n",
               global_n, nprocs, enable_io);
        printf("Global matrix sum (MPI_Reduce): %.6f\n", global_total);
    }

    free(local_matrix);
    free(local_row_sums);
    if (rank == 0) {
        free(full_matrix);
        free(all_row_sums);
    }

    MPI_Finalize();
    return 0;
}
