#! /bin/bash

mpirun -n 8 ./jim-haslett-csc6220-project-standard $1
mpirun -n 8 ./jim-haslett-csc6220-project-variant $1
./jim-haslett-csc6220-project-serial-quicksort $1