#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 #include <unistd.h>
  #include <semaphore.h>
int oxygen (unsigned long id, unsigned long delay,
              sem_t* semOxygen, sem_t* semHydrogen, sem_t* semWrite, sem_t* semMoleculeReady,sem_t* semTwoHydrogensInQueue,
              unsigned long* operation_count,unsigned long* molecule_count,
              unsigned long max_oxygen, unsigned long max_hydrogen,
              unsigned long* oxygen_consumed, unsigned long* hydrogen_consumed, char* error){
    FILE* file;
    srand(getpid() + time(NULL)); // Random generator using tam and pid, because time alone is too slow

    // START + SLEEP
    sem_wait(semWrite);
    (*operation_count)++;
    printf("%lu: O %lu: started\n", *operation_count, id);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: O %lu: started\n", *operation_count, id);
    fclose(file);
    sem_post(semWrite);
    unsigned long wait = rand() % delay + 1;
    usleep(wait * 1000);

    // QUEUE
    sem_wait(semWrite);
    (*operation_count)++;
    printf("%lu: O %lu: going to queue\n", *operation_count, id);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: O %lu: going to queue\n", *operation_count, id);
    fclose(file);
    sem_post(semWrite);
    sem_wait(semOxygen); // Waiting in queue to start making molecule or to die
    if(*error) { // If error occurred in main process, die and tell it to everyone
        sem_post(semOxygen);
        sem_post(semHydrogen);
        sem_post(semMoleculeReady);
        sem_post(semMoleculeReady);
        sem_post(semTwoHydrogensInQueue);
        exit(EXIT_FAILURE);
    }
    if(*hydrogen_consumed >= max_hydrogen - 1) { // If there is not enough hydrogen for molecule available, die and tell other children to die
        sem_wait(semWrite);
        (*operation_count)++;
        printf("%lu: O %lu: not enough H\n", *operation_count, id);
        file = fopen ("proj2.out", "a");
        fprintf(file, "%lu: O %lu: not enough H\n", *operation_count, id);
        fclose(file);
        sem_post(semWrite);
        sem_post(semOxygen);
        exit(EXIT_SUCCESS);
    }
    sem_wait(semTwoHydrogensInQueue); // Wait for signal to start makig molecule from two hydrogens
    sem_wait(semTwoHydrogensInQueue); 
    if(*error) { // If error occurred in main process, die and tell it to everyone
        sem_post(semOxygen);
        sem_post(semHydrogen);
        sem_post(semMoleculeReady);
        sem_post(semMoleculeReady);
        sem_post(semTwoHydrogensInQueue);
        exit(EXIT_FAILURE);
    }
    (*molecule_count)++;
    unsigned long moleculeID = *molecule_count; // Start creating molecule and notify two hydrogens that the process began
    sem_wait(semWrite);
    (*operation_count)++;
    printf("%lu: O %lu: creating molecule %lu\n", *operation_count, id, *molecule_count);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: O %lu: creating molecule %lu\n", *operation_count, id, *molecule_count);
    fclose(file);
    sem_post(semWrite);
    sem_post(semHydrogen);
    sem_post(semHydrogen);
    wait = rand() % delay + 1;
    usleep(wait * 1000); // Sleep and then notify hydrogens that the process is done
    sem_wait(semWrite);
    (*operation_count)++;
    printf("%lu: O %lu: molecule %lu created\n", *operation_count, id, moleculeID);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: O %lu: molecule %lu created\n", *operation_count, id, moleculeID);
    fclose(file);
    sem_post(semWrite);
    sem_post(semMoleculeReady);
    sem_post(semMoleculeReady);
    (*oxygen_consumed)++;
    if(*oxygen_consumed == max_oxygen) { // If there is no other oxygen, wake up one hydrogen process, that will kill all it's siblings
      sem_post(semHydrogen);
      exit(EXIT_SUCCESS);
    }
    sem_post(semOxygen); // Tell next oxygen in queue to start making molecule
    exit(EXIT_SUCCESS);
}