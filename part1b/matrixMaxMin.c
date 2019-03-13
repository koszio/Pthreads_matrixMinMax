
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 20   /* maximum number of workers */
#define NUMBER 99
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


int matrix[MAXSIZE][MAXSIZE]; /* matrix */
volatile int total, max, min=NUMBER;
volatile int maxRow, maxCol, minRow, minCol;

//pthread_mutex_t barrier;  /* mutex lock for the barrier */
//pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */
//int numArrived = 0;       /* number who have arrived */

/* a reusable counter barrier */
/*void Barrier() {
  pthread_mutex_lock(&barrier);
  numArrived++;
  if (numArrived == numWorkers) {
    numArrived = 0;
    pthread_cond_broadcast(&go);
  } else
    pthread_cond_wait(&go, &barrier);
  pthread_mutex_unlock(&barrier);
}*/

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */


void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /*Redundant but let them be*/
  /* initialize mutex and condition variable */
  //pthread_mutex_init(&barrier, NULL);
  //pthread_cond_init(&go, NULL);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  stripSize = size/numWorkers;

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
          matrix[i][j] = rand()%NUMBER;
	  }
  }

  /* print the matrix */
#ifdef DEBUG
  for (i = 0; i < size; i++) {
	  printf("[ ");
	  for (j = 0; j < size; j++) {
	    printf(" %d", matrix[i][j]);
	  }
	  printf(" ]\n");
  }
#endif

  /* do the parallel work: create the workers */
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++)
    pthread_create(&workerid[l], &attr, Worker, (void *) l);

  for (l = 0; l < numWorkers; l++)
      pthread_join(workerid[l], NULL);
    /* get end time */
   end_time = read_timer();
   /* print results */
   printf("The total is %d\nthe max value is %d at position %d:%d\n and the min value is %d at position %d:%d \n", total, max, maxRow, maxCol, min, minRow, minCol);
   printf("The execution time is %g sec\n", end_time - start_time);

  pthread_exit(NULL);

}


void *Worker(void *arg) {
  long myid = (long) arg;
  int i, j, first, last;


#ifdef DEBUG
  printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

  /* determine first and last rows of my strip */
/*Remember stripsize is defined as  q*m/t    or  size/numberThreads or workers in our case.
Also, q is the current thread*/

  first = myid*stripSize;
  last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);
//printf("The first is: %d\n", matrix[first][0]);

//  min = matrix[first][0];
//  max =matrix[first][0];

  //printf("I am here %d\n",min );
    for (i = first; i <= last; i++)
        for (j = 0; j < size; j++){
//COMMENT: Depending where the mutex is located we can practically have a sequencial program or a concurrent one.
//For example if the lock is placed before the for loops each consequent thread waits until the previous thread is done. WHich means that they do nothing. Consequently the
//program does run with more threads but each thread waits for the next one, thus no concurrent utilization
//IF however, the lock is placed inside the for loops then all threads are done with their for loops but they right one after the other one. Therefore it is performed concurrently.

          pthread_mutex_lock(&lock);
                total+=matrix[i][j];
                if(matrix[i][j]>max){
                  //     pthread_mutex_lock(&lock);

                  max = matrix[i][j];
                  maxRow=i;
                  maxCol=j;
                  //pthread_mutex_unlock(&lock);
                }
                //printf("I am here2 %d\n",max );
                if(matrix[i][j]<min){
                  //pthread_mutex_lock(&lock);
                  min = matrix[i][j];
                  minRow=i;
                  minCol=j;
                  pthread_mutex_unlock(&lock);
//                  pthread_mutex_unlock(&lock);
                }

              //  printf("I am here2 %d\n",min );
      }

  }
