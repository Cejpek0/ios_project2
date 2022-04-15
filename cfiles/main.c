#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/mman.h>
#include "../hfiles/functions.h"
#include "../hfiles/hydrogen.h"
#include "../hfiles/oxygen.h"
//#include <signal.h>

int main(int argc, char **arguments)
{
  unsigned long NO;
  unsigned long NH;
  unsigned int TI;
  unsigned int TB;
  unsigned long h2oCounter = 0;
  unsigned long hydrogenCounter = 0;
  unsigned long oxygenCounter = 0;
  (void)h2oCounter;
  (void)hydrogenCounter;
  (void) oxygenCounter;
  if( ! checkArguments(argc, arguments, &NO, &NH, &TI, &TB)){
    fprintf (stderr, "Spatne argumenty programu\n");
    return 1;
  }
  /*FILE *fp;
  remove("proj2.out");
  fp = fopen ("proj2.out", "w");
  if(fp == NULL) {
    return 0;
  }



  fclose (fp);*/
  unsigned long* queue_oxygen = mmap(NULL, sizeof(unsigned long), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  unsigned long* queue_hydrogen = mmap(NULL, sizeof(unsigned long), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  unsigned long* oxygen_count = mmap(NULL, sizeof(unsigned long), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  unsigned long* hydrogen_count = mmap(NULL, sizeof(unsigned long), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  int pid;
  srand(time(NULL)); 
  for(unsigned long i = 1; i <= NO && pid; ++i) {
    if((pid = fork())) {
      oxygen(i, TI, queue_oxygen, queue_hydrogen, NO, NH, oxygen_count, hydrogen_count);
    }
  }
  for(unsigned long i = 1; i <= NH && pid; ++i) {
    if((pid = fork())) {
      hydrogen(i, TI, queue_hydrogen, queue_oxygen, NO, NH, oxygen_count, hydrogen_count);
    }
  }
  int err = munmap(queue_oxygen, sizeof(unsigned long));
  if(err != 0){
      printf("UnMapping Failed\n");
      return 1;
  }
  err = munmap(queue_hydrogen, sizeof(unsigned long));
  if(err != 0){
      printf("UnMapping Failed\n");
      return 1;
  }
  err = munmap(hydrogen_count, sizeof(unsigned long));
  if(err != 0){
      printf("UnMapping Failed\n");
      return 1;
  }
  err = munmap(oxygen_count, sizeof(unsigned long));
  if(err != 0){
      printf("UnMapping Failed\n");
      return 1;
  }

  //kill(pid, SIGKILL);
  return 0;
}
