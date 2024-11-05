CC      =       /opt/ohpc/pub/mpi/openmpi3-gnu7/3.1.0/bin/mpicc
CCLINK  =       /opt/ohpc/pub/mpi/openmpi3-gnu7/3.1.0/bin/mpicc
SHELL   =       /bin/sh

all: standard

standard: jim-haslett-csc6220-project-standard.c
	$(CC) -o jim-haslett-csc6220-project-standard jim-haslett-csc6220-project-standard.c

variant: jim-haslett-csc6220-project-variant.c
	$(CC) -o jim-haslett-csc6220-project-variant jim-haslett-csc6220-project-variant.c

serial-quicksort: jim-haslett-csc6220-project-serial-quicksort
	gcc -o jim-haslett-csc6220-project-serial-quicksort jim-haslett-csc6220-project-serial-quicksort.c

clean:
	/bin/rm -f jim-haslett-csc6220-project-standard jim-haslett-csc6220-project-variant jim-haslett-csc6220-project-serial-quicksort