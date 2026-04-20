# Dardel: load compiler + MPI first, e.g.
#   module load PDC
#   module load cpeGNU/24.03  # or course-recommended PrgEnv
#   module load cray-mpich/8.1.28  # version as on system
# Then: make CC=cc  (Cray wrapper)  OR  make CC=mpicc

CC ?= mpicc
CFLAGS ?= -O3 -std=c11
LDFLAGS ?= -lm

.PHONY: all clean strong weak

all: pi_mpi_strong pi_mpi_weak

# Strong scaling: default build (no extra defines)
pi_mpi_strong: pi_mpi.c
	$(CC) $(CFLAGS) -o $@ pi_mpi.c $(LDFLAGS)

# Weak scaling: fixed 1M local tosses per rank (see Option 2 in source)
pi_mpi_weak: pi_mpi.c
	$(CC) $(CFLAGS) -DASSIGNMENT_WEAK_SCALING -o $@ pi_mpi.c $(LDFLAGS)

clean:
	rm -f pi_mpi_strong pi_mpi_weak
