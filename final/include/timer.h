#ifndef PR_TIMER_H
#define PR_TIMER_H

/* Monotonic wall-clock timer in seconds (CLOCK_MONOTONIC).
 * In MPI builds the file pagerank_mpi.c uses MPI_Wtime() directly. */
double pr_wtime(void);

#endif /* PR_TIMER_H */
