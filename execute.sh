#! /bin/bash
for ARRAY_SIZE_POWER in 16 20 24
do 
    ./jim-haslett-csc6220-project-serial-quicksort $ARRAY_SIZE_POWER
    mpirun -n 8 ./jim-haslett-csc6220-project-standard $ARRAY_SIZE_POWER
    mpirun -n 8 ./jim-haslett-csc6220-project-variant $ARRAY_SIZE_POWER
done # ARRAY_SIZE_POWER