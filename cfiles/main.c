#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
  #include <sys/wait.h>
#include "../cfiles/functions.c"
#include "../cfiles/hydrogen.c"
#include "../cfiles/oxygen.c"
//#include <signal.h>

  /**
   * @brief SEMAFORY
   */
  

int main(int argc, char **arguments)
{
  int pid = 1;
  unsigned long NO;
  unsigned long NH;
  unsigned int TI;
  unsigned int TB;
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
  
  /***
   * @brief SHARED MEMORY
   */
  sem_t* semOxygen = mmap(NULL, sizeof(unsigned long), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sem_t* semHydrogen = mmap(NULL, sizeof(unsigned long), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sem_t* semMoleculeReady = mmap(NULL, sizeof(unsigned long), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sem_t* semTwoHydrogensInQueue = mmap(NULL, sizeof(unsigned long), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  unsigned long* operation_count = mmap(NULL, sizeof(unsigned long), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  unsigned long* queue_hydrogen = mmap(NULL, sizeof(unsigned long), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  unsigned long* oxygen_consumed = mmap(NULL, sizeof(unsigned long), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  unsigned long* hydrogen_consumed = mmap(NULL, sizeof(unsigned long), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  unsigned long* molecule_count = mmap(NULL, sizeof(unsigned long), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  /**
   * @brief INIT SEMAPHORES
   * 
   */
  sem_init(semOxygen, 1, 1);
  sem_init(semHydrogen, 1, 0);
  sem_init(semMoleculeReady, 1, 0);
  sem_init(semTwoHydrogensInQueue, 1, 0);

  /**
   * @brief FOR KYSLIK/OXYGEN
   */
  for(unsigned long i = 1; i <= NO && pid; ++i) {
    if((pid = fork()) == 0) {
      oxygen(i, TI, semOxygen, semMoleculeReady, semHydrogen, semTwoHydrogensInQueue, operation_count, molecule_count, NO, NH, oxygen_consumed, hydrogen_consumed);
    } 
  }

  /**
   * @brief FOR VODIK/HYDROGEN
   */
  for(unsigned long i = 1; i <= NH && pid; ++i) {
    if((pid = fork()) == 0) {
      hydrogen(i, TI, semHydrogen, semMoleculeReady, semTwoHydrogensInQueue, operation_count, molecule_count, queue_hydrogen, NO, NH, oxygen_consumed, hydrogen_consumed );
    }
  }

  /**
   * @brief If child then die, if Parent then wait until all subprocesses end
   */
  if(pid == 0) {
    exit(EXIT_SUCCESS);
  } else {
    while (wait(NULL) > 0);
  }

  /**
   * @brief CLEANUP
   */
  sem_destroy(semOxygen);
  sem_destroy(semHydrogen);
  sem_destroy(semMoleculeReady);
  sem_destroy(semTwoHydrogensInQueue);

  int err = munmap(queue_hydrogen, sizeof(unsigned long));
  if(err != 0){
      printf("UnMapping Failed\n");
      return 1;
  }
  err = munmap(hydrogen_consumed, sizeof(unsigned long));
  if(err != 0){
      printf("UnMapping Failed\n");
      return 1;
  }
  err = munmap(oxygen_consumed, sizeof(unsigned long));
  if(err != 0){
      printf("UnMapping Failed\n");
      return 1;
  }
  err = munmap(operation_count, sizeof(unsigned long));
  if(err != 0){
      printf("UnMapping Failed\n");
      return 1;
  }
  err = munmap(molecule_count, sizeof(unsigned long));
  if(err != 0){
      printf("UnMapping Failed\n");
      return 1;
  }

  //kill(pid, SIGKILL);
  return EXIT_SUCCESS;
}
