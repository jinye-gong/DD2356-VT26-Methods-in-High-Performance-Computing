#define _POSIX_C_SOURCE 200809L

#include "timer.h"

#include <time.h>

double pr_wtime(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + 1e-9 * (double)ts.tv_nsec;
}
