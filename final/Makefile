# --------------------------------------------------------------------------
# Parallel PageRank - top-level Makefile
#
# Targets
#   make serial   -> bin/pagerank_serial (gcc, no OpenMP)
#   make omp      -> bin/pagerank_omp    (gcc + -fopenmp)
#   make mpi      -> bin/pagerank_mpi    (mpicc)
#   make hybrid   -> bin/pagerank_hybrid (mpicc + -fopenmp)
#   make all      -> as many as the local toolchain supports
#   make clean    -> remove bin/ and *.o
#
# Compiler overrides (Dardel example):
#   make CC=cc CFLAGS_EXTRA="-O3 -march=native"          # Cray serial/omp
#   make MPICC=cc                                         # Cray MPI wrapper
# --------------------------------------------------------------------------

CC      ?= gcc
MPICC   ?= mpicc
INCDIR   = include
CFLAGS  ?= -O3 -std=c11 -Wall -Wextra -Wno-unused-parameter -funroll-loops
CFLAGS  += -I$(INCDIR) $(CFLAGS_EXTRA)
LDLIBS  ?= -lm

COMMON_SRC = src/common/graph.c src/common/io.c src/common/timer.c src/common/params.c
SERIAL_SRC = $(COMMON_SRC) src/serial/pagerank_serial.c
OMP_SRC    = $(COMMON_SRC) src/omp/pagerank_omp.c
OMP_TARGET_SRC = $(COMMON_SRC) src/omp/pagerank_omp_target.c
MPI_SRC    = $(COMMON_SRC) src/mpi/pagerank_mpi.c
HYBRID_SRC = $(COMMON_SRC) src/hybrid/pagerank_hybrid.c

BIN = bin

# Auto-detect optional toolchains so `make all` does not blow up
HAS_MPI := $(shell command -v $(MPICC) >/dev/null 2>&1 && echo yes || echo no)

.PHONY: all serial omp omp_target mpi hybrid clean dirs

all: serial omp
ifeq ($(HAS_MPI),yes)
all: mpi hybrid
else
all:
	@echo "[make] $(MPICC) not found - skipping mpi/hybrid targets."
endif

dirs:
	@mkdir -p $(BIN)

serial: dirs $(BIN)/pagerank_serial
omp:    dirs $(BIN)/pagerank_omp
omp_target: dirs $(BIN)/pagerank_omp_target
mpi:    dirs $(BIN)/pagerank_mpi
hybrid: dirs $(BIN)/pagerank_hybrid

$(BIN)/pagerank_serial: $(SERIAL_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

$(BIN)/pagerank_omp: $(OMP_SRC)
	$(CC) $(CFLAGS) -fopenmp $^ -o $@ $(LDLIBS)

$(BIN)/pagerank_omp_target: $(OMP_TARGET_SRC)
	$(CC) $(CFLAGS) -fopenmp $^ -o $@ $(LDLIBS)

$(BIN)/pagerank_mpi: $(MPI_SRC)
ifeq ($(HAS_MPI),yes)
	$(MPICC) $(CFLAGS) $^ -o $@ $(LDLIBS)
else
	@echo "[make] $(MPICC) not found - cannot build $@. Install an MPI (e.g. apt install mpich) or set MPICC=cc on Dardel."
	@exit 1
endif

$(BIN)/pagerank_hybrid: $(HYBRID_SRC)
ifeq ($(HAS_MPI),yes)
	$(MPICC) $(CFLAGS) -fopenmp $^ -o $@ $(LDLIBS)
else
	@echo "[make] $(MPICC) not found - cannot build $@."
	@exit 1
endif

clean:
	rm -rf $(BIN) *.o src/*/*.o
