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

typedef struct thread_data_t {
  int tid;
} thread_data_t;

struct spin_lock_t {
   volatile int being_served; 
  volatile int started_waiting; 
};
struct spin_lock_t slock;

//GLOBAL VARIABLES

static inline int atomic_xadd(volatile int *ptr) {
  register int val __asm__("eax") = 1;
  asm volatile("lock xaddl %0,%1"
  : "+r" (val)
  : "m" (*ptr)
  : "memory"
  );  
  return val;
}

void spin_lock(struct spin_lock_t *s) {
  int started_waiting = atomic_xadd(&s->started_waiting);
  while(started_waiting != s->being_served){};
}

void spin_unlock(struct spin_lock_t *s) {
  atomic_xadd(&s->being_served);
}

void *criticalSection(void *arg) {
  thread_data_t *data = (thread_data_t *)arg; 
  int tid = data->tid;
  long int count=0;
  //printf("Thread %d is in Critical Section.\n", tid);
  while(clock()<stopTime)
  {
    spin_lock(&slock);  
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
    spin_unlock(&slock);
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
  slock.being_served = 0;
  slock.started_waiting = 0;
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
