#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <math.h>
#include <time.h>



int main(int argc, char* argv[])
{
    int niter = 1000000;
    int myid;                       //holds process's rank id
    double x,y;                     //x,y value for the random coordinate
    int i;                          //loop counter
    int count=0;                //Count holds all the number of how many good coordinates
    double z;                       //Used to check if x^2+y^2<=1
    double pi;                      //holds approx value of pi
    int reducedcount;                   //total number of "good" points from all nodes
    int reducedniter;                   //total number of ALL points from all nodes
    int ranknum = 0;
    int numthreads = 1;
    MPI_Init(&argc, &argv);                 //Start MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);           //get rank of node's process
    MPI_Comm_size(MPI_COMM_WORLD, &ranknum);


    double bTime, aTime, tTime;         //NEW Declaring doubles variables to time program
    bTime = omp_get_wtime();            //NEW Starting the timer


    int identity = 0;

    if(myid != 0)                       //Do the following on all except the master node
    {
        //Start OpenMP code: 16 threads/node
#pragma omp parallel firstprivate(x, y, z, i) reduction(+:count) num_threads(numthreads)
        {
            srandom((int)time(NULL) ^ omp_get_thread_num());    //Give random() a seed value
            for (i=0; i<niter; ++i)              //main loop
            {
                x = (double)random()/RAND_MAX;      //gets a random x coordinate
                y = (double)random()/RAND_MAX;      //gets a random y coordinate
                z = sqrt((x*x)+(y*y));          //Checks to see if number is inside unit circle
                if (z<=1)
                {
                    ++count;            //if it is, consider it a valid random point
                }
            }


            identity++;
        }


    }


    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(&count,
               &reducedcount,
               1,
               MPI_INT,
               MPI_SUM,
               0,
               MPI_COMM_WORLD);
    reducedniter = numthreads*niter*(ranknum-1);


    aTime = omp_get_wtime();            //NEW Ending the time
    tTime = aTime - bTime;              //NEW Finding the total time each loop of the program took


    printf("Loop %i took %f seconds\n", myid, tTime);


    if (myid == 0)                      //if root process/master node
    {
        //p = 4(m/n)
        pi = ((double)reducedcount/(double)reducedniter)*4.0;
        //Print the calculated value of pi
        printf("Pi: %f\n%i\n%d\n", pi, reducedcount, reducedniter);
    }






    MPI_Finalize();                     //Close the MPI instance
    return 0;
}

