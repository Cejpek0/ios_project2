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

int main(int argc, char **arguments)
{
  int pid = 0;
  unsigned long NO;
  unsigned long NH;
  unsigned int TI;
  unsigned int TB;
  if( ! checkArguments(argc, arguments, &NO, &NH, &TI, &TB)){
    fprintf (stderr, "Spatne argumenty programu\n");
    return EXIT_FAILURE;
  }
  FILE *file;
  remove("proj2.out");
  file = fopen ("proj2.out", "w");
  fclose(file);
  
  
  /***
   * @brief SHARED MEMORY
   */
  // Semaphre for synchronization of printing to file and incrementing of shared variables -> mutex lock
  sem_t* semSync = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  // Semaphore for releasing oxygen out of queue
  sem_t* semOxygen = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  // Semaphrore for releasing hydrogen out fo queue
  sem_t* semHydrogen = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  // Semaphore for checking if there is two hydrogens ready in queue
  sem_t* semTwoHydrogensInQueue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  // Semaphore that waits until hydrogens printed out creating
  sem_t* semMoleculeHydrogenCreating = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  // Semaphore for telling hydrogens that oxygen created molecule
  sem_t* semMoleculeOxygenCreated = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  // Semaphore for telling oxygen that hydrogens also created molecule
  sem_t* semMoleculeHydrogenCreated = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  
  // Counter of lines/operations
  atomic_ulong* operation_count = mmap(NULL, sizeof(atomic_ulong), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  // Counter of consumed oxygens
  atomic_ulong* oxygen_consumed = mmap(NULL, sizeof(atomic_ulong), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  // Counter of consumed hydrogens
  atomic_ulong* hydrogen_consumed = mmap(NULL, sizeof(atomic_ulong), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  // Counter of molecules created
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
  sem_init(semMoleculeOxygenCreated, 1, 0);
  sem_init(semTwoHydrogensInQueue, 1, 0);
  sem_init(semMoleculeHydrogenCreated, 1, 0);
  sem_init(semMoleculeHydrogenCreating, 1, 0);

  /**
   * @brief FOR KYSLIK/OXYGEN
   */
  for(unsigned long i = 1; i <= NO; ++i) {
    if((pid = fork()) == 0) {
      oxygen(i, TI, TB, semOxygen, semHydrogen, semSync, semMoleculeOxygenCreated, semTwoHydrogensInQueue, semMoleculeHydrogenCreated, semMoleculeHydrogenCreating, operation_count, molecule_count, NO, NH, oxygen_consumed, hydrogen_consumed, error);
      break;
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
      sem_post(semMoleculeOxygenCreated);
      sem_post(semMoleculeOxygenCreated);
      sem_post(semTwoHydrogensInQueue);
      break;
    } 
  }

  /**
   * @brief FOR VODIK/HYDROGEN
   */
  for(unsigned long i = 1; i <= NH; ++i) {
    if((pid = fork()) == 0) {
      hydrogen(i, TI, semHydrogen, semSync, semMoleculeOxygenCreated, semTwoHydrogensInQueue, semMoleculeHydrogenCreated, semMoleculeHydrogenCreating, operation_count, molecule_count, NO, NH, oxygen_consumed, hydrogen_consumed, error);
      break;
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
      sem_post(semMoleculeOxygenCreated);
      sem_post(semMoleculeOxygenCreated);
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
  sem_destroy(semMoleculeOxygenCreated);
  sem_destroy(semTwoHydrogensInQueue);
  sem_destroy(semMoleculeHydrogenCreated);

  if(munmap(hydrogen_consumed, sizeof(atomic_ulong)) != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return EXIT_FAILURE;
  }
  if(munmap(oxygen_consumed, sizeof(atomic_ulong)) != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return EXIT_FAILURE;
  }
  if(munmap(operation_count, sizeof(atomic_ulong)) != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return EXIT_FAILURE;
  }
  if(munmap(molecule_count, sizeof(atomic_ulong)) != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return EXIT_FAILURE;
  }
  if(munmap(error, sizeof(char)) != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return EXIT_FAILURE;
  }
  if(munmap(semOxygen, sizeof(sem_t)) != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return EXIT_FAILURE;
  }
  if(munmap(semSync, sizeof(sem_t)) != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return EXIT_FAILURE;
  }
  if(munmap(semHydrogen, sizeof(sem_t)) != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return EXIT_FAILURE;
  }
  if(munmap(semMoleculeOxygenCreated, sizeof(sem_t)) != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return EXIT_FAILURE;
  }
  if(munmap(semTwoHydrogensInQueue, sizeof(sem_t)) != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return EXIT_FAILURE;
  }
  if(munmap(semMoleculeHydrogenCreated, sizeof(sem_t)) != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return EXIT_FAILURE;
  }
  if(munmap(semMoleculeHydrogenCreating, sizeof(sem_t)) != 0){
      fprintf(stderr, "UnMapping Failed\n");
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
