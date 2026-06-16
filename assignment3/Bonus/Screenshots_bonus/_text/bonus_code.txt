#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_NEURONS 1000
#define DEFAULT_STEPS 500
#define DEFAULT_THRESHOLD 50.0

static inline unsigned int lcg_next(unsigned int x) {
    return 1664525u * x + 1013904223u;
}

static void init_state(double *potentials, int *firings, unsigned int *rng_state, int neurons, unsigned int seed) {
    for (int i = 0; i < neurons; i++) {
        unsigned int s = seed ^ (unsigned int)(i * 2654435761u);
        rng_state[i] = lcg_next(s);
        potentials[i] = (double)(rng_state[i] % 20u);
        firings[i] = 0;
    }
}

static void simulate_serial(
    double *potentials,
    int *firings,
    unsigned int *rng_state,
    int neurons,
    int steps,
    double threshold,
    int write_output,
    const char *out_path
) {
    FILE *f = NULL;
    if (write_output) {
        f = fopen(out_path, "w");
        if (!f) {
            fprintf(stderr, "Failed to open %s\n", out_path);
            exit(1);
        }
    }

    for (int step = 0; step < steps; step++) {
        for (int i = 0; i < neurons; i++) {
            rng_state[i] = lcg_next(rng_state[i]);
            potentials[i] += (double)(rng_state[i] % 10u);
            if (potentials[i] > threshold) {
                firings[i]++;
                potentials[i] = 0.0;
            }
            if (f) {
                fprintf(f, "%d %d %.6f\n", step, i, potentials[i]);
            }
        }
    }
    if (f) fclose(f);
}

static void simulate_omp_task(
    double *potentials,
    int *firings,
    unsigned int *rng_state,
    int neurons,
    int steps,
    double threshold,
    int write_output,
    const char *out_path
) {
    FILE *f = NULL;
    if (write_output) {
        f = fopen(out_path, "w");
        if (!f) {
            fprintf(stderr, "Failed to open %s\n", out_path);
            exit(1);
        }
    }

    for (int step = 0; step < steps; step++) {
        if (!write_output) {
#pragma omp parallel
            {
#pragma omp single nowait
                {
/* taskloop keeps task-model semantics with manageable granularity */
#pragma omp taskloop grainsize(256)
                    for (int i = 0; i < neurons; i++) {
                        rng_state[i] = lcg_next(rng_state[i]);
                        potentials[i] += (double)(rng_state[i] % 10u);
                        if (potentials[i] > threshold) {
                            firings[i]++;
                            potentials[i] = 0.0;
                        }
                    }
                }
            }
        } else {
            /* Keep deterministic output order for logging mode. */
#pragma omp parallel
            {
#pragma omp single nowait
                {
/* Use the same taskloop path, then ordered write on host thread. */
#pragma omp taskloop grainsize(256)
                    for (int i = 0; i < neurons; i++) {
                        rng_state[i] = lcg_next(rng_state[i]);
                        potentials[i] += (double)(rng_state[i] % 10u);
                        if (potentials[i] > threshold) {
                            firings[i]++;
                            potentials[i] = 0.0;
                        }
                    }
                }
            }
            for (int i = 0; i < neurons; i++) {
                fprintf(f, "%d %d %.6f\n", step, i, potentials[i]);
            }
        }
    }
    if (f) fclose(f);
}

static void write_summary(const char *path, const int *firings, int neurons) {
    FILE *f = fopen(path, "w");
    if (!f) {
        fprintf(stderr, "Failed to open %s\n", path);
        exit(1);
    }
    for (int i = 0; i < neurons; i++) {
        fprintf(f, "Neuron %d fired %d times\n", i, firings[i]);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    int neurons = DEFAULT_NEURONS;
    int steps = DEFAULT_STEPS;
    double threshold = DEFAULT_THRESHOLD;
    const char *mode = "task"; /* serial | task */
    int write_output = 0;
    unsigned int seed = 42u;

    if (argc > 1) mode = argv[1];
    if (argc > 2) neurons = atoi(argv[2]);
    if (argc > 3) steps = atoi(argv[3]);
    if (argc > 4) threshold = atof(argv[4]);
    if (argc > 5) write_output = atoi(argv[5]);
    if (argc > 6) seed = (unsigned int)strtoul(argv[6], NULL, 10);

    if (neurons <= 0 || steps <= 0 || threshold <= 0.0) {
        fprintf(stderr, "Usage: ./neuron_omp_task [serial|task] [neurons] [steps] [threshold] [write_output 0/1] [seed]\n");
        return 1;
    }

    double *potentials = (double *)malloc((size_t)neurons * sizeof(double));
    int *firings = (int *)malloc((size_t)neurons * sizeof(int));
    unsigned int *rng_state = (unsigned int *)malloc((size_t)neurons * sizeof(unsigned int));
    if (!potentials || !firings || !rng_state) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(potentials);
        free(firings);
        free(rng_state);
        return 1;
    }
    init_state(potentials, firings, rng_state, neurons, seed);

    double t0 = omp_get_wtime();
    if (strcmp(mode, "serial") == 0) {
        simulate_serial(potentials, firings, rng_state, neurons, steps, threshold, write_output, "neuron_output.txt");
    } else {
        simulate_omp_task(potentials, firings, rng_state, neurons, steps, threshold, write_output, "neuron_output.txt");
    }
    double t1 = omp_get_wtime();

    write_summary("neuron_summary.txt", firings, neurons);

    long long total_fires = 0;
    for (int i = 0; i < neurons; i++) total_fires += firings[i];

    printf("Mode:          %s\n", mode);
    printf("Neurons:       %d\n", neurons);
    printf("Steps:         %d\n", steps);
    printf("Threshold:     %.2f\n", threshold);
    printf("OMP threads:   %d\n", omp_get_max_threads());
    printf("Write output:  %d\n", write_output);
    printf("Total firings: %lld\n", total_fires);
    printf("OMP time:      %.6f s\n", t1 - t0);

    free(potentials);
    free(firings);
    free(rng_state);
    return 0;
}
