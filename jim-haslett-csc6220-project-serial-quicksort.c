/**
 * Copyright 20Jim Haslett
 *
 * This work part of a university assignment.  If you are taking the course
 * this work was assigned for, do the right thing, and solve the assignment
 * yourself!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define OUTFILENAME "result.txt"
#define REPORTFILENAME "report.txt"

int power(int base, __uint8_t power){
    if(power == 0) return 1;

    int value = base;
    for(int i = 0; i < power - 1; i++) {
        value *= base;
    }
    return value;
}


int comparer(const void *e1, const void *e2)
{
    return (*((int *)e1) - *((int *)e2));
}


double wtime() {
    /**
     * wtime function used to generate a wall time, in seconds, similar to MPI_wtime()
     * 
     * Based entirely on the MPI_wtime implementation:
     * https://github.com/open-mpi/ompi/blob/main/ompi/mpi/c/wtime.c
     */

    double wtime;
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
    wtime  = (double)tp.tv_nsec/1.0e+9;
    wtime += tp.tv_sec;
    return wtime;
}

int main(int argc,char* argv[]) {

    /* Input power from CLI an set array size to 2**power */
    int array_size_power = atoi(argv[1]);
    int array_size = power(2, array_size_power);

    /* Full array */
    int *start_array;

    /* Timer variables */
    double start_time, end_time;


    /* Allocate memory for array */
    start_array = (int *)malloc(array_size*sizeof(int));

    /* Initialize array */
    srand(time(NULL));
    for(int i = 0; i < array_size; i++) {
        start_array[i] = rand() % 128;
    }

    /* Record start time */
    start_time = wtime();

    /* Sort local array using built-in quicksort */
    qsort(start_array, array_size, sizeof(int), comparer);
    
    /* record end time */
    end_time = wtime();

    /* Output result matrix to file */
    FILE *outfile; /* File Pointer for output */
    outfile = fopen(OUTFILENAME, "w");
    for(int i = 0; i < array_size; i++) {
        fprintf(outfile, "%4d", start_array[i]);
    }
    fprintf(outfile, "\n");
    fclose(outfile);        


    /* calculate times */
    double total_time = end_time - start_time;

    /* output execution time to file */
    outfile = fopen(REPORTFILENAME, "a");
    fprintf(outfile, "Serial Quicksort - 2**%d array - time: %f \n", array_size_power, total_time);
    fclose(outfile);

    /* Print execution time to screen*/
    printf("Serial Quicksort - 2**%d array - time: %f \n", array_size_power, total_time);


    /* Free allocated memory */
    free(start_array);
    
    exit(0);
}