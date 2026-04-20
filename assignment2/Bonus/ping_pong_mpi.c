#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * MPI ping-pong RTT benchmark.
 *
 * Usage:
 *   mpirun -np 2 ./ping_pong_mpi [max_bytes] [repeats] [warmup]
 *
 * Default:
 *   max_bytes = 4 MiB, repeats = 1000, warmup = 20
 *
 * Output:
 *   CSV: bytes,rtt_us
 */
int main(int argc, char *argv[]) {
  int rank, nproc;
  int max_bytes = 4 * 1024 * 1024;
  int repeats = 1000;
  int warmup = 20;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  if (nproc != 2) {
    if (rank == 0) {
      fprintf(stderr, "This benchmark requires exactly 2 MPI processes.\n");
    }
    MPI_Finalize();
    return 1;
  }

  if (argc > 1) max_bytes = atoi(argv[1]);
  if (argc > 2) repeats = atoi(argv[2]);
  if (argc > 3) warmup = atoi(argv[3]);
  if (max_bytes < 1) max_bytes = 1;
  if (repeats < 1) repeats = 1;
  if (warmup < 0) warmup = 0;

  char *send_buf = (char *)malloc((size_t)max_bytes);
  char *recv_buf = (char *)malloc((size_t)max_bytes);
  if (!send_buf || !recv_buf) {
    fprintf(stderr, "Allocation failed for %d bytes buffers\n", max_bytes);
    free(send_buf);
    free(recv_buf);
    MPI_Finalize();
    return 1;
  }
  memset(send_buf, 1, (size_t)max_bytes);
  memset(recv_buf, 0, (size_t)max_bytes);

  if (rank == 0) {
    printf("# max_bytes=%d repeats=%d warmup=%d\n", max_bytes, repeats, warmup);
    printf("bytes,rtt_us\n");
  }

  for (int bytes = 1; bytes <= max_bytes; bytes *= 2) {
    double total_time = 0.0;

    for (int it = 0; it < warmup + repeats; ++it) {
      if (rank == 0) {
        const double t0 = MPI_Wtime();
        MPI_Send(send_buf, bytes, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(recv_buf, bytes, MPI_BYTE, 1, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        const double t1 = MPI_Wtime();
        if (it >= warmup) total_time += (t1 - t0);
      } else {
        MPI_Recv(recv_buf, bytes, MPI_BYTE, 0, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        MPI_Send(send_buf, bytes, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
      }
    }

    if (rank == 0) {
      const double avg_rtt_us = (total_time / (double)repeats) * 1e6;
      printf("%d,%.6f\n", bytes, avg_rtt_us);
    }
  }

  free(send_buf);
  free(recv_buf);
  MPI_Finalize();
  return 0;
}
