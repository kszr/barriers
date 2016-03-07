## When running on the cluster (or even, possibly, your own machine), make sure
## that the variables MPICC and CC are set correctly. In particular, change 'gcc-5'
## to 'gcc', if necessary, and use the commented out definition of MPICC when
## running on the cluster.

CC=gcc-5
MPICC=mpicc

## Use the following path for MPICC on the jinx cluster.
# MPICC = /opt/openmpi-1.4.3-gcc44/bin/mpicc
CFLAGS=-c -Wall

all: mpi_tournament mpi_mcs omp_sense omp_dissemination mpi_omp_barrier

mpi_tournament: mpi_tournament.o
	$(MPICC) mpi_tournament.o -o mpi_tournament

mpi_tournament.o: mpi_tournament.c
	$(MPICC) $(CFLAGS) mpi_tournament.c

mpi_mcs: mpi_mcs.o
	$(MPICC) mpi_mcs.o -o mpi_mcs

mpi_mcs.o: mpi_mcs.c
	$(MPICC) $(CFLAGS) mpi_mcs.c

omp_dissemination: omp_dissemination.o
	$(CC) -lm -fopenmp omp_dissemination.o -o omp_dissemination

omp_dissemination.o: omp_dissemination.c
	$(CC) $(CFLAGS) -lm -fopenmp omp_dissemination.c

omp_sense: omp_sense.o
	$(CC) -fopenmp omp_sense.o -o omp_sense

omp_sense.o: omp_sense.c
	$(CC) $(CFLAGS) -fopenmp omp_sense.c

mpi_omp_barrier: mpi_omp_barrier.o
	$(MPICC) -fopenmp mpi_omp_barrier.o -o mpi_omp_barrier

mpi_omp_barrier.o: mpi_omp_barrier.c
	$(MPICC) $(CFLAGS) -fopenmp mpi_omp_barrier.c

clean:
	$(RM) -f *o mpi_tournament mpi_mcs omp_dissemination omp_sense mpi_omp_barrier
