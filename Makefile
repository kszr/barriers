CC=mpicc
CFLAGS=-c -Wall

all: mpi_tournament mpi_mcs

mpi_tournament: mpi_tournament.o
	$(CC) mpi_tournament.o -o mpi_tournament

mpi_tournament.o: mpi_tournament.c
	$(CC) $(CFLAGS) mpi_tournament.c

mpi_mcs: mpi_mcs.o
	$(CC) mpi_mcs.o -o mpi_mcs

mpi_mcs.o: mpi_mcs.c
	$(CC) $(CFLAGS) mpi_tournament.c

clean:
	$(RM) -f *o mpi_tournament mpi_mcs