#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* Cray/clang + -std=c11 often omits M_PI; define if missing (portable). */
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

int main(int argc, char** argv) {
    int rank, nproc;
    long long int tosses = 1000; /* argv[1]: strong = total tosses; ignored for weak when WEAK defined */
    if (argc > 1) tosses = atoll(argv[1]);
    long long int local_tosses;
    long long int total_tosses; /* denominator for pi estimate and printed total */
    long long int local_hits = 0, total_hits = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    // Option 1: Strong Scaling
    /* 选项 1 — 强扩展：总投掷次数固定，按进程数均分（余数分给 rank 较小的进程） */
#ifndef ASSIGNMENT_WEAK_SCALING
    local_tosses = tosses / nproc + (rank < (tosses % nproc) ? 1 : 0);
    total_tosses = tosses;
#else
    // Option 2: Weak Scaling
    /* 选项 2 — 弱扩展：每进程本地投掷 100 万次，总投掷次数 = nproc × 1000000 */
    local_tosses = 1000000LL;
    total_tosses = (long long int)nproc * local_tosses;
    (void)tosses; /* argv[1] total not used in weak scaling */
#endif

    // Seed random generator differently per process
    srand(time(NULL) + rank);

    double start_time = MPI_Wtime();

    for (long long int i = 0; i < local_tosses; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;

        if (x * x + y * y <= 1.0) {
            local_hits++;
        }
    }

    // Reduce all local hits into total_hits at rank 0
    MPI_Reduce(&local_hits, &total_hits, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (rank == 0) {
        double pi_estimate = 4.0 * total_hits / total_tosses;
        printf("Estimated Pi: %.12f, True Pi: %.12f\n", pi_estimate, M_PI);
        printf("Error: %.12f\n", fabs(M_PI - pi_estimate));
        printf("Total Tosses: %lld\n", total_tosses);
        printf("Execution Time: %f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
