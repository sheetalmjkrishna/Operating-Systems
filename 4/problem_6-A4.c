#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>

//GLOBAL VARIABLES

int threadCount = 0;
volatile int stopTime;
volatile int in_cs=0;
volatile int me_check=0;
volatile int total_random=0;
volatile int total_inside=0;
pthread_mutex_t thread_mutex;

typedef struct thread_data_t {
  int tid;
} thread_data_t;

//GLOBAL VARIABLES
void *criticalSection(void *arg) {
  thread_data_t *data = (thread_data_t *)arg; 
  int tid = data->tid;
  while(clock()<stopTime)
  {
    pthread_mutex_lock(&thread_mutex);
    float x=((float)rand()/RAND_MAX)*2-1;
    float y=((float)rand()/RAND_MAX)*2-1;
    float dist_from_center_sq = x*x+y*y;
    total_random++;
    total_inside+=(dist_from_center_sq<=1)?1:0;
    me_check=tid;      
    assert(in_cs==0);
    in_cs++;
    assert(in_cs==1);
    in_cs++;
    assert(in_cs==2);
    in_cs++;
    assert(in_cs==3);
    in_cs=0;
    if(me_check!=tid)
        printf("Mutual Exclusion has not been maintained!\n");
    pthread_mutex_unlock(&thread_mutex);
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  int result = 0,i=0,time=0;
  if(argc!=3)
  {  
    fprintf(stderr, "You've missed entering thread count or time!\n");
    return EXIT_FAILURE;
  }
  threadCount =  atoi(argv[1]);
  time = atoi(argv[2]);
  stopTime = clock() + (time * CLOCKS_PER_SEC);
  pthread_t thr[threadCount];
  thread_data_t thread_data[threadCount];
  for (i = 0; i < threadCount; i++) {
    thread_data[i].tid = i;
    if ((result = pthread_create(&thr[i], NULL, criticalSection, &thread_data[i]))) {
      fprintf(stderr, "Thread Creation failed because of: %d\n", result);
      return EXIT_FAILURE;
    }
  }
  for (i = 0; i < threadCount; ++i) {
    pthread_join(thr[i], NULL);
  }
  float pi=4*(total_inside)/(float)(total_random);
  printf("Total number of points generated within the square = %d\nTotal number of points that fell within the circle =%d\nThe approximate value of PI calculated using the Monte Carlo method = %f \n",total_random,total_inside,pi);
  return EXIT_SUCCESS;
}
