#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/*References
https://cis.temple.edu/~giorgio/old/cis307s97/readings/pbuffer.html
*/

//GLOBAL VARIABLES AND FUNCTION DECLARATIONS

int n_cats = 0,n_dogs=0,n_birds=0;
int startTime = 0;
volatile unsigned long cats_play=0, dogs_play=0,birds_play=0;
volatile int cats_cur=0, dogs_cur=0,birds_cur=0;

pthread_mutex_t thread_mutex;
pthread_cond_t cat_cond;
pthread_cond_t dog_cond;
pthread_cond_t bird_cond;

int IsNum(char num[]);
void Cat_enter(void);
void Cat_exit(void);
void Dog_enter(void);
void Dog_exit(void);
void Bird_enter(void);
void Bird_exit(void);
void Play(void);

typedef struct thread_data_t {
  int tid;
  char* type;
} thread_data_t;

//GLOBAL VARIABLES AND FUNCTION DECLARATIONS

void mfence (void) {
  asm volatile ("mfence" : : : "memory");
}

void Play(void) {
  //Assertion to check if all the rules are being followed
  int i=0;
  for (i=0; i<10; i++) {
    assert(cats_cur >= 0 && cats_cur <= n_cats);
    assert(dogs_cur >= 0 && dogs_cur <= n_dogs);
    assert(birds_cur >= 0 && birds_cur <= n_birds);
    assert(cats_cur == 0 || dogs_cur == 0);
    assert(cats_cur == 0 || birds_cur == 0);
   }
}

void Cat_enter(void) {
  //lock
  if (pthread_mutex_lock(&thread_mutex) != 0) {
    printf("Cats: Wasn't able to acquire the lock!\n");
    return;
  }
  //Cat shouldn't enter when there are dogs or birds in the playground. It should release the process and wait until cat_cond is satisfied.
  while (dogs_cur>0 || birds_cur>0) 
    pthread_cond_wait(&cat_cond, &thread_mutex);
  //Increment number of current cats by 1
  cats_cur++;  
  //Everything should be executed in the same order
  ////mfence();
  //Increment count for total number of cats that have played in the playground
  cats_play++;
  //mfence();
  //Do the assertion
  Play(); 
  //mfence();
  //Make th ecat leave the playground
  Cat_exit();
  //mfence();
  //Release the lock
  if (pthread_mutex_unlock(&thread_mutex) != 0) {
    printf("Cats: Wasn't able to release the lock!\n");
    exit(1);
  }
}

void Cat_exit(void) {
  //Decrement the number of cats currently in the playground
  cats_cur--;
  if(cats_cur==0)
  {  
    //If there are no more cats on the ground on the playground, let dogs and birds know that it's safe for them to enter by broadcasting their conditions
    pthread_cond_broadcast(&dog_cond);
    pthread_cond_broadcast(&bird_cond);
  }
}


void Dog_enter(void) {
  //Common comments are mentioned in cat functions
    if (pthread_mutex_lock(&thread_mutex) != 0) {
    printf("Dogs: Wasn't able to acquire the lock!\n");
    return;
  }
  //Dogs can only enter when there are no cats, so wait till dog_cond is true
  while (cats_cur>0) 
    pthread_cond_wait(&dog_cond, &thread_mutex);
  dogs_cur++; 
  //mfence();
  dogs_play++; 
  //mfence();
  Play(); 
  //mfence();
  Dog_exit();
  //mfence();
    if (pthread_mutex_unlock(&thread_mutex) != 0) {
    printf("Dogs: Wasn't able to release the lock!\n");
    exit(1);
  }
}

void Dog_exit(void) {
  //Common comments are mentioned in cat functions
  dogs_cur--;
  //If there are no more dogs AND NO MORE BIRDS, THEN let cats know it's safe to enter
  if(dogs_cur==0 && birds_cur==0)
    pthread_cond_broadcast(&cat_cond);
}

void Bird_enter(void) {
  //Common comments are mentioned in cat functions
  if (pthread_mutex_lock(&thread_mutex) != 0) {
    printf("Birds: Wasn't able to acquire the lock!\n");
    return;
  }
  //Birds can only enter when there are no cats, so wait till bird_cond is true
  while (cats_cur>0) {
    pthread_cond_wait(&bird_cond, &thread_mutex);}
  birds_cur++; 
  //mfence();
  birds_play++; 
  //mfence();
  Play(); 
  //mfence();
  Bird_exit();
  //mfence();
  if (pthread_mutex_unlock(&thread_mutex) != 0) {
    printf("Birds: Wasn't able to release the lock!\n");
    exit(1);
  }
}

void Bird_exit(void) {
  //Common comments are mentioned in cat functions
  birds_cur--;
  //If there are no more birds AND NO MORE DOGS, THEN let cats know it's safe to enter
  if(dogs_cur==0 && birds_cur==0)
    pthread_cond_broadcast(&cat_cond);
}


void *criticalSection(void *arg) {
  thread_data_t *data = (thread_data_t *)arg;  
  //Check if time is up
  while(time(NULL)-startTime<=10)
  {
    //printf("%ju \n",clock()/CLOCKS_PER_SEC);
    //Call respective enter function according to which animal is trying to enter
    if(strcmp(data->type,"cat")==0)
        Cat_enter();
    else if(strcmp(data->type,"dog")==0)
       Dog_enter();
    else if(strcmp(data->type,"bird")==0)
        Bird_enter();
  }
  pthread_exit(NULL);
}

int IsNum(char num[]){
   //To check if input parameters are numbers
   int i;
   if(num[0]=='-'){
      return 0;
   }
   for(i=0;i<(int)strlen(num);i++){
      if(!isdigit(num[i]))
         return 0;
   }
   return 1;
}

int main(int argc, char *argv[]) {
  int result = 0,i;
  if(argc<4)
  {  
    fprintf(stderr, "You've missed entering the count for one of the animals!\n");
    return EXIT_FAILURE;
  }
  if(argc>4)
  {  
    fprintf(stderr, "You've entered too many numbers!\n");
    return EXIT_FAILURE;
  }
  //If the arg is a number, THEN convert to int
  if(IsNum(argv[1]))
     n_cats =  atoi(argv[1]);
  else{
    fprintf(stderr, "Cats: You've entered a non-integer or negative number!\n");
    return EXIT_FAILURE;
  }
  if(IsNum(argv[2]))
     n_dogs =  atoi(argv[2]);
  else{
    fprintf(stderr, "Dogs: You've entered a non-integer or negative number!\n");
    return EXIT_FAILURE;
  }
  if(IsNum(argv[3]))
     n_birds =  atoi(argv[3]);
  else{
    fprintf(stderr, "Birds: You've entered a non-integer or negative number!\n");
    return EXIT_FAILURE;
  }
  if(n_cats>99||n_cats<0||n_dogs>99||n_dogs<0||n_birds>99||n_birds<0){
    fprintf(stderr, "Count of animals should be between 0 and 99!\n");
    return EXIT_FAILURE;
  }
  //Create thread and data arrays
  pthread_t thr_cats[n_cats];
  pthread_t thr_dogs[n_dogs];
  pthread_t thr_birds[n_birds];
  thread_data_t thread_data_cats[n_cats];
  thread_data_t thread_data_dogs[n_dogs];
  thread_data_t thread_data_birds[n_birds];

  //If youu're unable to initialize the mutex lock
  if(pthread_mutex_init(&thread_mutex, NULL) != 0) {
    perror("pthread_mutex_init failed!");
    exit(0);
  }

  //If you're unable to initialze any of the condition variables of the animals to 0
  if(pthread_cond_init(&cat_cond, NULL)!= 0){
    perror("Cat: pthread_cond_init failed!");
    exit(0);
  }
   if(pthread_cond_init(&dog_cond, NULL)!= 0){
    perror("Dog: pthread_cond_init failed!");
    exit(0);
  }
   if(pthread_cond_init(&bird_cond, NULL)!= 0){
    perror("Bird: pthread_cond_init failed!");
    exit(0);
  }  

  //Create individual threads for each animal of each kind and call the common critical section function for each which will take care of calling the enter/exit funtions
  printf("Play-time begins! :) \n");
  startTime = time(NULL);
  for (i=0; i < n_cats; i++) {
    thread_data_cats[i].tid = i;
    thread_data_cats[i].type = malloc(4);
    strcpy(thread_data_cats[i].type,"cat");
    if ((result = pthread_create(&thr_cats[i], NULL, criticalSection, &thread_data_cats[i]))) {
      fprintf(stderr, "Cats- Thread Creation failed because of error: %d (It's possible that you are low on memory. You could close the browser, folders, clear the trash etc. and try again)\n", result);
      return EXIT_FAILURE;
    }
  }
  for (i=0; i < n_dogs; i++) {
    thread_data_dogs[i].tid = i;
    thread_data_dogs[i].type = malloc(4);
    strcpy(thread_data_dogs[i].type,"dog");
    if ((result = pthread_create(&thr_dogs[i], NULL, criticalSection, &thread_data_dogs[i]))) {
      fprintf(stderr, "Dogs- Thread Creation failed because of error: %d (It's possible that you are low on memory. You could close the browser, folders, clear the trash etc. and try again)\n", result);
      return EXIT_FAILURE;
    }
  }
  for (i=0; i < n_birds; i++) {
    thread_data_birds[i].tid = i;
    thread_data_birds[i].type = malloc(5);
    strcpy(thread_data_birds[i].type,"bird");
    if ((result = pthread_create(&thr_birds[i], NULL, criticalSection, &thread_data_birds[i]))) {
      fprintf(stderr, "Birds- Thread Creation failed because of error: %d (It's possible that you are low on memory. You could close the browser, folders, clear the trash etc. and try again)\n", result);
      return EXIT_FAILURE;
    }
  }
  
  
  //Wait for and join all the threads that have exited
  for (i=0; i < n_cats; i++) {
	pthread_join(thr_cats[i], NULL);
  }
  for (i=0; i < n_dogs; i++) {
	pthread_join(thr_dogs[i], NULL);
  }
  for (i=0; i < n_birds; i++) {
        pthread_join(thr_birds[i], NULL);
  }
  //Print results
  printf("Play-time ends! :( \n\n");
  printf("Number of Cats that entered the Playground = %lu \nNumber of Dogs that entered the Playground = %lu \nNumber of Birds that entered the Playground = %lu \n\n",cats_play,dogs_play,birds_play);
  return EXIT_SUCCESS;
}
