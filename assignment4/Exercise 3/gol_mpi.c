#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

static int global_n = 500;
static int global_steps = 100;
static int enable_io = 1;

typedef struct {
    int *cur;
    int *nxt;
    int li;
    int lj;
    int stride;
} Grid;

static void parse_args(int argc, char **argv) {
    const char *env_n = getenv("GOL_N");
    const char *env_steps = getenv("GOL_STEPS");
    const char *env_io = getenv("GOL_IO");
    if (env_n) global_n = atoi(env_n);
    if (env_steps) global_steps = atoi(env_steps);
    if (env_io) enable_io = atoi(env_io);

    /* Command-line overrides environment (avoids stale GOL_IO after correctness run). */
    if (argc >= 2) global_n = atoi(argv[1]);
    if (argc >= 3) global_steps = atoi(argv[2]);
    if (argc >= 4) enable_io = atoi(argv[3]);

    if (global_n < 3) global_n = 3;
    if (global_steps < 1) global_steps = 1;
}

static void partition_1d(int rank, int nprocs, int n, int *start, int *len) {
    int base = n / nprocs;
    int rem = n % nprocs;
    if (rank < rem) {
        *len = base + 1;
        *start = rank * (base + 1);
    } else {
        *len = base;
        *start = rem * (base + 1) + (rank - rem) * base;
    }
}

static int grid_at(const Grid *g, int i, int j) {
    return g->cur[i * g->stride + j];
}

static void grid_set(Grid *g, int i, int j, int v) {
    g->cur[i * g->stride + j] = v;
}

static void grid_alloc(Grid *g, int li, int lj) {
    g->li = li;
    g->lj = lj;
    g->stride = lj + 2;
    size_t cells = (size_t)(li + 2) * (size_t)(lj + 2);
    g->cur = (int *)calloc(cells, sizeof(int));
    g->nxt = (int *)calloc(cells, sizeof(int));
}

static void grid_free(Grid *g) {
    free(g->cur);
    free(g->nxt);
    g->cur = g->nxt = NULL;
}

static unsigned int mix_u32(unsigned int x) {
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

static void initialize_grid(Grid *g, int start_i, int start_j) {
    for (int i = 1; i <= g->li; i++) {
        for (int j = 1; j <= g->lj; j++) {
            int gi = start_i + i - 1;
            int gj = start_j + j - 1;
            unsigned int key = (unsigned int)(gi * 1315423911u) ^ (unsigned int)(gj * 2654435761u);
            unsigned int rnd = mix_u32(key);
            /* Deterministic sparse initialization: ~25% live cells. */
            grid_set(g, i, j, (rnd % 100u) < 25u ? 1 : 0);
        }
    }
}

static int count_neighbors(const Grid *g, int i, int j) {
    int s = 0;
    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            if (di == 0 && dj == 0) continue;
            s += grid_at(g, i + di, j + dj);
        }
    }
    return s;
}

static void update_interior(Grid *g) {
    for (int i = 2; i < g->li; i++) {
        for (int j = 2; j < g->lj; j++) {
            int n = count_neighbors(g, i, j);
            int alive = grid_at(g, i, j);
            int next = alive;
            if (alive && (n < 2 || n > 3)) next = 0;
            else if (!alive && n == 3) next = 1;
            g->nxt[i * g->stride + j] = next;
        }
    }
}

static void update_boundaries(Grid *g) {
    for (int i = 1; i <= g->li; i++) {
        for (int j = 1; j <= g->lj; j++) {
            if (i > 1 && i < g->li && j > 1 && j < g->lj) continue;
            int n = count_neighbors(g, i, j);
            int alive = grid_at(g, i, j);
            int next = alive;
            if (alive && (n < 2 || n > 3)) next = 0;
            else if (!alive && n == 3) next = 1;
            g->nxt[i * g->stride + j] = next;
        }
    }
}

static void grid_swap(Grid *g) {
    int *tmp = g->cur;
    g->cur = g->nxt;
    g->nxt = tmp;
}

static int cart_neighbor(MPI_Comm cart, const int dims[2], const int coords[2],
                         int di, int dj) {
    int c[2] = {
        (coords[0] + di + dims[0]) % dims[0],
        (coords[1] + dj + dims[1]) % dims[1]
    };
    int rank;
    MPI_Cart_rank(cart, c, &rank);
    return rank;
}

static void exchange_ghosts(Grid *g, MPI_Comm cart, const int dims[2],
                            const int coords[2]) {
    int li = g->li, lj = g->lj, stride = g->stride;
    int rank_n, rank_s, rank_w, rank_e;
    MPI_Request reqs[24];
    int nreq = 0;

    /* Directional tags: sender and receiver of the same message use the SAME tag. */
    enum {
        TAG_N2S = 10, /* north -> south (north row sent to south ghost) */
        TAG_S2N = 11, /* south -> north */
        TAG_W2E = 20, /* west -> east */
        TAG_E2W = 21, /* east -> west */
        TAG_NW2SE = 30,
        TAG_SE2NW = 31,
        TAG_NE2SW = 32,
        TAG_SW2NE = 33
    };

    MPI_Cart_shift(cart, 0, 1, &rank_n, &rank_s);
    MPI_Cart_shift(cart, 1, 1, &rank_w, &rank_e);

    /* North neighbor sends row into my north ghost; I send row 1 southward for their ghost. */
    MPI_Irecv(&g->cur[0 * stride + 1], lj, MPI_INT, rank_n, TAG_N2S, cart, &reqs[nreq++]);
    MPI_Isend(&g->cur[1 * stride + 1], lj, MPI_INT, rank_n, TAG_S2N, cart, &reqs[nreq++]);

    MPI_Irecv(&g->cur[(li + 1) * stride + 1], lj, MPI_INT, rank_s, TAG_S2N, cart, &reqs[nreq++]);
    MPI_Isend(&g->cur[li * stride + 1], lj, MPI_INT, rank_s, TAG_N2S, cart, &reqs[nreq++]);

    MPI_Datatype col_type;
    MPI_Type_vector(li, 1, stride, MPI_INT, &col_type);
    MPI_Type_commit(&col_type);

    MPI_Irecv(&g->cur[1 * stride + 0], 1, col_type, rank_w, TAG_W2E, cart, &reqs[nreq++]);
    MPI_Isend(&g->cur[1 * stride + 1], 1, col_type, rank_w, TAG_E2W, cart, &reqs[nreq++]);

    MPI_Irecv(&g->cur[1 * stride + lj + 1], 1, col_type, rank_e, TAG_E2W, cart, &reqs[nreq++]);
    MPI_Isend(&g->cur[1 * stride + lj], 1, col_type, rank_e, TAG_W2E, cart, &reqs[nreq++]);

    if (li >= 2 && lj >= 2) {
        update_interior(g);
    }

    MPI_Waitall(nreq, reqs, MPI_STATUSES_IGNORE);
    MPI_Type_free(&col_type);

    int rank_nw = cart_neighbor(cart, dims, coords, -1, -1);
    int rank_ne = cart_neighbor(cart, dims, coords, -1, +1);
    int rank_sw = cart_neighbor(cart, dims, coords, +1, -1);
    int rank_se = cart_neighbor(cart, dims, coords, +1, +1);

    nreq = 0;
    MPI_Irecv(&g->cur[0 * stride + 0], 1, MPI_INT, rank_nw, TAG_NW2SE, cart, &reqs[nreq++]);
    MPI_Isend(&g->cur[1 * stride + 1], 1, MPI_INT, rank_nw, TAG_SE2NW, cart, &reqs[nreq++]);

    MPI_Irecv(&g->cur[0 * stride + lj + 1], 1, MPI_INT, rank_ne, TAG_NE2SW, cart, &reqs[nreq++]);
    MPI_Isend(&g->cur[1 * stride + lj], 1, MPI_INT, rank_ne, TAG_SW2NE, cart, &reqs[nreq++]);

    MPI_Irecv(&g->cur[(li + 1) * stride + 0], 1, MPI_INT, rank_sw, TAG_SW2NE, cart, &reqs[nreq++]);
    MPI_Isend(&g->cur[li * stride + 1], 1, MPI_INT, rank_sw, TAG_NE2SW, cart, &reqs[nreq++]);

    MPI_Irecv(&g->cur[(li + 1) * stride + lj + 1], 1, MPI_INT, rank_se, TAG_SE2NW, cart, &reqs[nreq++]);
    MPI_Isend(&g->cur[li * stride + lj], 1, MPI_INT, rank_se, TAG_NW2SE, cart, &reqs[nreq++]);

    MPI_Waitall(nreq, reqs, MPI_STATUSES_IGNORE);
}

static void place_patch(int *global_grid, int si, int sj, int ln_i, int ln_j,
                        const int *patch) {
    for (int i = 0; i < ln_i; i++) {
        for (int j = 0; j < ln_j; j++) {
            global_grid[(si + i) * global_n + (sj + j)] = patch[i * ln_j + j];
        }
    }
}

static void write_output_parallel(int rank, int nprocs, MPI_Comm cart,
                                  const Grid *g, int step) {
    int cart_dims[2], periods[2];
    int *patch = (int *)malloc((size_t)g->li * (size_t)g->lj * sizeof(int));

    for (int i = 0; i < g->li; i++) {
        for (int j = 0; j < g->lj; j++) {
            patch[i * g->lj + j] = grid_at(g, i + 1, j + 1);
        }
    }

    {
        int dummy_coords[2];
        MPI_Cart_get(cart, 2, cart_dims, periods, dummy_coords);
    }

    if (rank == 0) {
        int *global_grid =
            (int *)calloc((size_t)global_n * (size_t)global_n, sizeof(int));
        int *recvbuf = (int *)malloc((size_t)global_n * (size_t)global_n * sizeof(int));

        for (int r = 0; r < nprocs; r++) {
            int rc[2], si, sj, ln_i, ln_j;
            MPI_Cart_coords(cart, r, 2, rc);
            partition_1d(rc[0], cart_dims[0], global_n, &si, &ln_i);
            partition_1d(rc[1], cart_dims[1], global_n, &sj, &ln_j);

            if (r == 0) {
                place_patch(global_grid, si, sj, ln_i, ln_j, patch);
            } else {
                MPI_Recv(recvbuf, ln_i * ln_j, MPI_INT, r, 100, cart, MPI_STATUS_IGNORE);
                place_patch(global_grid, si, sj, ln_i, ln_j, recvbuf);
            }
        }

        char filename[64];
        snprintf(filename, sizeof(filename), "gol_output_%d.txt", step);
        FILE *f = fopen(filename, "w");
        for (int gi = 0; gi < global_n; gi++) {
            for (int gj = 0; gj < global_n; gj++) {
                fprintf(f, "%d ", global_grid[gi * global_n + gj]);
            }
            fprintf(f, "\n");
        }
        fclose(f);
        free(global_grid);
        free(recvbuf);
    } else {
        MPI_Send(patch, g->li * g->lj, MPI_INT, 0, 100, cart);
    }

    free(patch);
    (void)nprocs;
}

int main(int argc, char **argv) {
    int rank, nprocs;
    MPI_Comm cart = MPI_COMM_NULL;
    int dims[2] = {0, 0};
    int periods[2] = {1, 1};
    int coords[2];
    int start_i, start_j, li, lj;
    Grid grid;
    double t0, t1;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    parse_args(argc, argv);

    MPI_Dims_create(nprocs, 2, dims);
    if (dims[0] == 0) dims[0] = 1;
    if (dims[1] == 0) dims[1] = nprocs;

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cart);
    MPI_Cart_coords(cart, rank, 2, coords);

    partition_1d(coords[0], dims[0], global_n, &start_i, &li);
    partition_1d(coords[1], dims[1], global_n, &start_j, &lj);

    grid_alloc(&grid, li, lj);
    initialize_grid(&grid, start_i, start_j);

    MPI_Barrier(cart);
    t0 = MPI_Wtime();

    if (enable_io) {
        /* Save initial global state before the first update. */
        write_output_parallel(rank, nprocs, cart, &grid, 0);
    }

    for (int step = 1; step <= global_steps; step++) {
        exchange_ghosts(&grid, cart, dims, coords);
        update_boundaries(&grid);
        grid_swap(&grid);

        if (enable_io && (step % 10 == 0)) {
            write_output_parallel(rank, nprocs, cart, &grid, step);
        }
    }

    MPI_Barrier(cart);
    t1 = MPI_Wtime();

    if (rank == 0) {
        printf("WALLTIME %.9f\n", t1 - t0);
        printf("Game of Life complete (N=%d, steps=%d, io=%d, nprocs=%d).\n",
               global_n, global_steps, enable_io, nprocs);
    }

    grid_free(&grid);
    MPI_Comm_free(&cart);
    MPI_Finalize();
    return 0;
}
