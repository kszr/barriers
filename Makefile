CC=gcc
MPICC=mpicc

## Use the following path for MPICC on the jinx cluster.
# MPICC = /opt/openmpi-1.4.3-gcc44/bin/mpicc
CFLAGS=-c -Wall
OMP_FLAGS=-c -Wall -fopenmp

all: mpi_tournament mpi_mcs omp_dissemination omp_sense

mpi_tournament: mpi_tournament.o
	$(MPICC) mpi_tournament.o -o mpi_tournament

mpi_tournament.o: mpi_tournament.c
	$(MPICC) $(CFLAGS) mpi_tournament.c

mpi_mcs: mpi_mcs.o
	$(MPICC) mpi_mcs.o -o mpi_mcs

mpi_mcs.o: mpi_mcs.c
	$(MPICC) $(CFLAGS) mpi_mcs.c

omp_dissemination: omp_dissemination.o
	$(CC) $(OMP_FLAGS) -lm omp_dissemination.o -o omp_dissemination

omp_dissemination.o: omp_dissemination.c
	$(CC) $(OMP_FLAGS) -lm omp_dissemination.c

omp_sense: omp_sense.o
	$(CC) $(OMP_FLAGS) omp_sense.o -o omp_sense

omp_sense.o: omp_sense.c
	$(CC) $(OMP_FLAGS) omp_sense.c

clean:
	$(RM) -f *o mpi_tournament mpi_mcs omp_dissemination omp_sense
