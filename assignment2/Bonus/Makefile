CC ?= mpicc
CFLAGS ?= -O3
TARGET = ping_pong_mpi

.PHONY: all clean

all: $(TARGET)

$(TARGET): ping_pong_mpi.c
	$(CC) $(CFLAGS) -o $(TARGET) ping_pong_mpi.c

clean:
	rm -f $(TARGET)

