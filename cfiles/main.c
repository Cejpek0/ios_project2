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
#include <stdatomic.h>
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
  FILE *file;
  remove("proj2.out");
  file = fopen ("proj2.out", "w");
  fclose(file);
  
  
  /***
   * @brief SHARED MEMORY
   */
  sem_t* semOxygen = mmap(NULL, sizeof(sem_t*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sem_t* semSync = mmap(NULL, sizeof(sem_t*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sem_t* semHydrogen = mmap(NULL, sizeof(sem_t*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sem_t* semMoleculeReady = mmap(NULL, sizeof(sem_t*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sem_t* semTwoHydrogensInQueue = mmap(NULL, sizeof(sem_t*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  sem_t* semMoleculeCreated = mmap(NULL, sizeof(sem_t*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  atomic_ulong* operation_count = mmap(NULL, sizeof(atomic_ulong), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  atomic_ulong* queue_hydrogen = mmap(NULL, sizeof(atomic_ulong), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  atomic_ulong* oxygen_consumed = mmap(NULL, sizeof(atomic_ulong), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  atomic_ulong* hydrogen_consumed = mmap(NULL, sizeof(atomic_ulong), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  atomic_ulong* molecule_count = mmap(NULL, sizeof(atomic_ulong), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  char* error = mmap(NULL, sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  *error = 0;

  /**
   * @brief INIT SEMAPHORES
   * 
   */
  sem_init(semOxygen, 1, 1);
  sem_init(semSync, 1, 1);
  sem_init(semHydrogen, 1, 0);
  sem_init(semMoleculeReady, 1, 0);
  sem_init(semTwoHydrogensInQueue, 1, 0);
  sem_init(semMoleculeCreated, 1, 0);


  /**
   * @brief FOR KYSLIK/OXYGEN
   */
  for(unsigned long i = 1; i <= NO && pid; ++i) {
    if((pid = fork()) == 0) {
      oxygen(i, TI, semOxygen, semHydrogen, semSync, semMoleculeReady, semTwoHydrogensInQueue, semMoleculeCreated, operation_count, molecule_count, NO, NH, oxygen_consumed, hydrogen_consumed, error);
    } else if (pid < 0) {
      sem_wait(semSync);
      (*operation_count)++;
      file = fopen ("proj2.out", "a");
      fprintf(file, "ERROR when creating new process. Exiting\n");
      fclose(file);
      sem_post(semSync);
      fprintf(stderr, "ERROR when creating new process. Exiting\n");
      *error = 1;
      sem_post(semOxygen);
      sem_post(semHydrogen);
      sem_post(semMoleculeReady);
      sem_post(semMoleculeReady);
      sem_post(semTwoHydrogensInQueue);
      break;
    } 
  }

  /**
   * @brief FOR VODIK/HYDROGEN
   */
  for(unsigned long i = 1; i <= NH && pid; ++i) {
    if((pid = fork()) == 0) {
      hydrogen(i, TI, semHydrogen, semSync, semMoleculeReady, semTwoHydrogensInQueue, semMoleculeCreated, operation_count, molecule_count, queue_hydrogen, NO, NH, oxygen_consumed, hydrogen_consumed, error);
    } else if (pid < 0) {
      sem_wait(semSync);
      (*operation_count)++;
      file = fopen ("proj2.out", "a");
      fprintf(file, "ERROR when creating new process. Exiting\n");
      fclose(file);
      sem_post(semSync);
      fprintf(stderr, "ERROR when creating new process. Exiting\n");
      *error = 1;
      sem_post(semOxygen);
      sem_post(semHydrogen);
      sem_post(semMoleculeReady);
      sem_post(semMoleculeReady);
      sem_post(semTwoHydrogensInQueue);
      break;
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
  sem_destroy(semSync);
  sem_destroy(semHydrogen);
  sem_destroy(semMoleculeReady);
  sem_destroy(semTwoHydrogensInQueue);
  sem_destroy(semMoleculeCreated);

  int err = munmap(queue_hydrogen, sizeof(atomic_ulong));
  if(err != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return 1;
  }
  err = munmap(hydrogen_consumed, sizeof(atomic_ulong));
  if(err != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return 1;
  }
  err = munmap(oxygen_consumed, sizeof(atomic_ulong));
  if(err != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return 1;
  }
  err = munmap(operation_count, sizeof(atomic_ulong));
  if(err != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return 1;
  }
  err = munmap(molecule_count, sizeof(atomic_ulong));
  if(err != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return 1;
  }
  err = munmap(error, sizeof(char));
  if(err != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return 1;
  }
  err = munmap(semOxygen, sizeof(sem_t*));
  if(err != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return 1;
  }
  err = munmap(semSync, sizeof(sem_t*));
  if(err != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return 1;
  }
  err = munmap(semHydrogen, sizeof(sem_t*));
  if(err != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return 1;
  }
  err = munmap(semMoleculeReady, sizeof(sem_t*));
  if(err != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return 1;
  }
  err = munmap(semTwoHydrogensInQueue, sizeof(sem_t*));
  if(err != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return 1;
  }
  err = munmap(semMoleculeCreated, sizeof(sem_t*));
  if(err != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return 1;
  }

  return EXIT_SUCCESS;
}
