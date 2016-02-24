CC=mpicc
CFLAGS=-c -Wall

all: sc_tree

sc_tree: sc_tree.o
	$(CC) sc_tree.o -o sc_tree

sc_tree.o: sc_tree.c
	$(CC) $(CFLAGS) sc_tree.c

clean:
	$(RM) -f *o sc_tree
