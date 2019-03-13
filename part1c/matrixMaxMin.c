/*Bag of tasks*/

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

int nextRow = 0;          /* shared bag variable */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

pthread_mutex_t lock;
int numWorkers;           /* number of workers */


typedef struct max{
  int max, i, j;
} max_struct;

typedef struct min{
  int min, i, j;
} min_struct;

typedef struct myResult{
  max_struct max_element;
  min_struct min_element;
  int partial_sum;
} myResult;


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
int size;  /* assume size is multiple of numWorkers */


void *Worker(void *arg);

/* read command line, initialize, and create threads */
int main(int argc, char **argv) {

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;



  int i, j;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex for the shared bags */
  pthread_mutex_init(&lock, NULL);


  /* initialize the matrix */
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
          matrix[i][j] = rand()%99;
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


  myResult *mRes;
  int total=0, max=0, min=NUMBER, max_x=0, max_y=0, min_x=0, min_y=0;


  start_time = read_timer();
  for (l = 0; l < numWorkers; l++){
    pthread_create(&workerid[l], &attr, Worker, (void *) l);

  }

  for (l = 0; l < numWorkers; l++){
    pthread_join(workerid[l], (void **)&mRes);
    total += mRes->partial_sum;
    if(mRes->max_element.max > max){
      max = mRes->max_element.max;
      max_x = mRes->max_element.i;
      max_y = mRes->max_element.j;
    }
    if(mRes->min_element.min < min){
      min = mRes->min_element.min;
      min_x = mRes->min_element.i;
      min_y = mRes->min_element.j;
    }
    free(mRes);
  }
  end_time = read_timer();


  printf("The total sum is %d\n", total);
  printf("The maximum is %d, and it is located at position (%d, %d)\n", max, max_x, max_y);
  printf("The minimum is %d, and it is located at position (%d, %d)\n", min, min_x, min_y);
  printf("The execution time is %g sec\n", end_time - start_time);

}
/* Each worker sums the values in one strip of the matrix.
After a barrier, worker(0) computes and prints the total */

void *Worker(void *arg) {
  long myid = (long) arg;
  int row =0;

  int total=0, j, last=size;
  int max, min, max_pos_row, max_pos_col, min_pos_row, min_pos_col;

  /* Compute sum, min, max as usual */
  max = matrix[row][0]; max_pos_row = row; max_pos_col = 0;
  min = matrix[row][0]; min_pos_row = row; min_pos_col = 0;

  myResult *result = malloc(sizeof(myResult));

#ifdef DEBUG
  printf("worker %ld (pthread id %lu) has started\n", myid, pthread_self());
#endif

  while(true){
    pthread_mutex_lock(&lock);
    row = nextRow;
    nextRow++;
    pthread_mutex_unlock(&lock);

    if(row >= size)
      break;

    for (j = 0; j < last; j++){
	total += matrix[row][j];
	if(matrix[row][j] > max){
	  max = matrix[row][j];
	  max_pos_row = row;
	  max_pos_col = j;
	}
	if(matrix[row][j] < min){
	  min = matrix[row][j];
	  min_pos_row = row;
	  min_pos_col = j;
	}
    }

  }

  result->partial_sum = total;
  result->max_element.max = max;
  result->max_element.i = max_pos_row;
  result->max_element.j = max_pos_col;
  result->min_element.min = min;
  result->min_element.i = min_pos_row;
  result->min_element.j = min_pos_col;
  return (void *)result;
}
