CC=mpicc
CFLAGS=-c -Wall

all: tourney_tree

tourney_tree: tourney_tree.o
	$(CC) tourney_tree.o -o tourney_tree

tourney_tree.o: tourney_tree.c
	$(CC) $(CFLAGS) tourney_tree.c

clean:
	$(RM) -f *o tourney_tree *.dSYM
