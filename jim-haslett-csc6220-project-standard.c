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
#include "mpi.h"

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



/* This is the CompareSplit function from the textbook */  /* it is horribly unreadable code */
void CompareSplit(int nlocal, int *elmnts, int *relmnts, int *wspace, int keepsmall)
{
    int i, j, k;
    
    for (i=0; i<nlocal; i++)
        wspace[i] = elmnts[i]; /* Copy the elmnts array into the wspace array */
    
    if (keepsmall) { /* Keep the nlocal smaller elements */
        for (i=j=k=0; k<nlocal; k++) {
            if (j == nlocal || (i < nlocal && wspace[i] < relmnts[j]))
                elmnts[k] = wspace[i++];
            else
                elmnts[k] = relmnts[j++];
        }
    }
    else { /* Keep the nlocal larger elements */
        for (i=k=nlocal-1, j=nlocal-1; k>=0; k--) {
            // if (j == 0 || (i >= 0 && wspace[i] >= relmnts[j]))  /* This is a typo in the textbook! should be j<0 */ 
            if (j < 0 || (i >= 0 && wspace[i] >= relmnts[j]))
                elmnts[k] = wspace[i--];
            else
                elmnts[k] = relmnts[j--];
        }
    }
}

/**
 * TESTING FUNCTION 
 */
void print_status(int *local, int *received, int num, int rank, int iteration) {
    char *tmp = (char *) malloc((num*2*4 + 30)*sizeof(char));
    int j = 0;
    j += sprintf(tmp+j, "I: %d r: %d  L: ", iteration, rank);
    for(int i = 0; i < num; i++) {
        j += sprintf(tmp+j, "%4d", local[i]);
    }
    j += sprintf(tmp+j, "  R: ");
    for(int i = 0; i < num; i++) {
        j += sprintf(tmp+j, "%4d", received[i]);
    }
    j += sprintf(tmp+j, "\n");
    printf(tmp);
    free(tmp);
}


int main(int argc,char* argv[]) {

    /* MPI rank */
    int rank;

    /* Initialize MPI */
    MPI_Init(&argc, &argv);

    /* Fetch rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Fetch number of processes */
    int number_of_processes;
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);

    /* MPI_Recv status variable */
    MPI_Status status;

    int array_size_power = atoi(argv[1]);
    int array_size = power(2, array_size_power);

    int *start_array;

    /* Number of elements in the local array on eacn process */
    int number_of_local_elements = array_size / number_of_processes;
    /* Local array for each process */
    int *local_array;
    /* Received array for each process */
    int *recieved_array;
    /* Working space for sort operation */
    int *working_space;
    /* odd and even ranks (right or left) */
    int odd_rank, even_rank;

    /* Timer variables */
    double start_time, end_time;

/**
 * test
 */        
    int *test;
/**
 * /test
 */        

    if(rank == 0) {

        /* Allocate memory for array */
        start_array = (int *)malloc(array_size*sizeof(int));

        /* Initialize array */
        srand(time(NULL));
        for(int i = 0; i < array_size; i++) {
            start_array[i] = rand() % 128;
        }

        /* Record start time */
        start_time = MPI_Wtime();

    } // if rank==0

    /* Allocate memory for local arrays */
    local_array = (int *)malloc(number_of_local_elements*sizeof(int));
    recieved_array = (int *)malloc(number_of_local_elements*sizeof(int));
    working_space = (int *)malloc(number_of_local_elements*sizeof(int));

    /* Distribute start_array to each process */
    MPI_Scatter( start_array, number_of_local_elements, MPI_INT, local_array, number_of_local_elements, MPI_INT, 0, MPI_COMM_WORLD);

    /* Sort local array using built-in quicksort */
    qsort(local_array, number_of_local_elements, sizeof(int), comparer);
    

    /* Determine rank of even and odd processes we interact with */
    if(rank % 2 == 0) {
        /* even rank, for odd operations look right, even operations look left */
        odd_rank = rank - 1;
        even_rank = rank + 1;
    }
    else {
        /* odd rank, for odd operations look left, even operations look right */
        odd_rank = rank + 1;
        even_rank = rank - 1;
    }

    /* set processes interact ranks for processors at end of the line to MPI_PROC_NULL */
    if( odd_rank == -1 || odd_rank == number_of_processes ) {
        odd_rank = MPI_PROC_NULL;
    }
    if( even_rank == -1 || even_rank == number_of_processes ) {
        even_rank = MPI_PROC_NULL;
    }

    /* Main Loop */
    for(int i = 0; i < number_of_processes - 1; i++) {
        if(i%2 == 1) {
            /* Odd Phase */
            MPI_Sendrecv(local_array, number_of_local_elements, MPI_INT, odd_rank, 1, recieved_array, number_of_local_elements, 
                MPI_INT, odd_rank, 1, MPI_COMM_WORLD, &status);
        }
        else {
            /* Even Phase */
            MPI_Sendrecv(local_array, number_of_local_elements, MPI_INT, even_rank, 1, recieved_array, number_of_local_elements, 
                MPI_INT, even_rank, 1, MPI_COMM_WORLD, &status);

        }
        /**
         * This is another typo in the textbook.  status.MPI_SOURCE is -2 if the the rank we exchange with is MPI_PROC_NULL. 
         * That means on rank 0 odd_rank is -2, on Odd phases, -2 < 0 so CompareSplit kees the larger, not the smaller set.
         * Instead, this should be skipped if status.MPI_SOURCE == MPI_PROC_NULL;
         */
        // CompareSplit(number_of_local_elements, local_array, recieved_array, working_space, rank < status.MPI_SOURCE);
        if(status.MPI_SOURCE != MPI_PROC_NULL) {
            CompareSplit(number_of_local_elements, local_array, recieved_array, working_space, rank < status.MPI_SOURCE);
        }
    }

/**
 * TESTING!!
 */
    if(rank==0) {
        test = (int *)malloc(array_size*sizeof(int));
        /* deep copy */
        for(int i = 0; i < array_size; i++) {
            test[i] = start_array[i];
        }
        qsort(test, array_size, sizeof(int), comparer);
    }
/**
 * /TESTING!!
 */
    MPI_Gather( local_array, number_of_local_elements, MPI_INT, start_array, number_of_local_elements, MPI_INT, 0, MPI_COMM_WORLD);

    /* Free allocated memory */
    free(local_array);
    free(recieved_array);
    free(working_space);

    if(rank == 0) {
        /* This process is rank 0, therefore it's the root */

        /* record end time */
        end_time = MPI_Wtime();

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
        fprintf(outfile, "MPI %d process time: %f\n", number_of_processes, total_time);
        fclose(outfile);

        /* Print execution time to screen*/
        printf("MPI %d process time: %f \n", number_of_processes, total_time);


/**
 * TESTING!!
 */        
        for(int i = 0; i < array_size; i++) {
            if( start_array[i]  != test[i] ){
                printf("Validation Failure! at %d  E/O: %d  V: %d\n", i, start_array[i], test[i]);
            }
        }
        free(test);
/**
 * /TESTING!!
 */   

        /* Free allocated memory */
        free(start_array);
     
    } // if rank==0

    MPI_Finalize();
    exit(0);
}