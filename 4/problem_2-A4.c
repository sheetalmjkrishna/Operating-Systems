#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>

//GLOBAL VARIABLES

volatile bool *Entering;
volatile int *Number;
int threadCount = 0;
volatile int stopTime;
volatile int in_cs=0;
volatile int me_check=0;

typedef struct thread_data_t {
  int tid;
} thread_data_t;

//GLOBAL VARIABLES

int maxToken(void)
{
  int maxVal = 0;
  int i=0;
  for(i=0;i<threadCount;i++)
  {
    if(Number[i]>maxVal)
      maxVal = Number[i];
  } 
  return maxVal;
}

void lock(int id) {
  Entering[id] = true; 
  Number[id] = maxToken()+1;
  Entering[id] = false;  
  int newId=0;
  for (newId = 0; newId < threadCount; newId++) {
      while(Entering[newId])
      {
	sched_yield();
      }
      while(Number[newId]!=0 && (Number[newId] < Number[id] || (Number[newId] == Number[id] && newId<id)))
      {
        sched_yield();
      }
    }
}   
void unlock(int id) {      
   Number[id] = 0;
} 
void *criticalSection(void *arg) {
  thread_data_t *data = (thread_data_t *)arg; 
  int tid = data->tid;
  long int count=0;
  //printf("Thread %d is in Critical Section.\n", tid);
  while(clock()<stopTime)
  {
    lock(tid);  
    ++count;      
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
    unlock(tid);
  }
  printf("Thread %d entered the Critical Section %ld times.\n",tid,count);
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
  Entering = malloc(threadCount * sizeof(*Entering));
  Number = malloc(threadCount * sizeof(*Number));
  for (i = 0; i < threadCount; i++) {
    Entering[i]=false;
    Number[i] = 0;
  }
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
  return EXIT_SUCCESS;
}
