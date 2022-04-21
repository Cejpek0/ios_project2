#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 #include <unistd.h>
  #include <semaphore.h>
int oxygen (unsigned long id, unsigned long delay,
              sem_t* semOxygen, sem_t* semMoleculeReady, sem_t* semHydrogen, sem_t* semTwoHydrogensInQueue,
              unsigned long* operation_count,unsigned long* molecule_count,
              unsigned long max_oxygen, unsigned long max_hydrogen,
              unsigned long* oxygen_consumed, unsigned long* hydrogen_consumed){
    int error;
    srand(getpid()+time(NULL));
    // START + SLEEP
    (*operation_count)++;
    printf("%lu: O %lu: started\n", *operation_count, id);
    unsigned long x = rand() % delay + 1;
    usleep(x * 1000);
    // QUEUE
    (*operation_count)++;
    printf("%lu: O %lu: going to queue\n", *operation_count, id);
    sem_wait(semOxygen); // Waiting in queue to start making molecule or to die
    if(*hydrogen_consumed >= max_hydrogen - 1) { // If there is not enough hydrogen for molecule available, die and tell other children to die
        (*operation_count)++;
        printf("%lu: O %lu: not enough H\n", *operation_count, id);
        error = sem_post(semOxygen);
        if(error == -1) {
            printf("FUCK");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    sem_wait(semTwoHydrogensInQueue); // Wait for signal to start makig molecule from two hydrogens
    sem_wait(semTwoHydrogensInQueue); 
    (*operation_count)++;
    (*molecule_count)++;
    unsigned long moleculeID = *molecule_count; // Start creating molecule and notify two hydrogens that the process began
    printf("%lu: O %lu: creating molecule %lu\n", *operation_count, id, *molecule_count);
    error = sem_post(semHydrogen);
    if(error == -1) {
            printf("FUCK");
            return EXIT_FAILURE;
        }
    error = sem_post(semHydrogen);
    if(error == -1) {
            printf("FUCK");
            return EXIT_FAILURE;
        }
    x = rand() % delay + 1;
    usleep(x * 1000); // Sleep and then notify hydrogens that the process is done
    printf("%lu: O %lu: molecule %lu created\n", *operation_count, id, moleculeID);
    error = sem_post(semMoleculeReady);
    if(error == -1) {
            printf("FUCK");
            return EXIT_FAILURE;
        }
    error = sem_post(semMoleculeReady);
    if(error == -1) {
            printf("FUCK");
            return EXIT_FAILURE;
        }
    (*operation_count)++;
    (*oxygen_consumed)++;
    if(*oxygen_consumed == max_oxygen) { // If there is no other oxygen, enable one hydrogen process, that will kill all it's siblings
      error = sem_post(semHydrogen);
      if(error == -1) {
            printf("FUCK");
            return EXIT_FAILURE;
        }
      return EXIT_SUCCESS;
    }
    error = sem_post(semOxygen); // Tell next oxygen in queue to start making molecule
    if(error == -1) {
            printf("FUCK");
            return EXIT_FAILURE;
        }
    return EXIT_SUCCESS;
}