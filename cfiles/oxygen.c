#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdatomic.h>
int oxygen (unsigned long id, unsigned long delay,
              sem_t* semOxygen, sem_t* semHydrogen, sem_t* semSync, sem_t* semMoleculeReady,sem_t* semTwoHydrogensInQueue,
              sem_t* semMoleculeCreated, atomic_ulong* operation_count,atomic_ulong* molecule_count,
              unsigned long max_oxygen, unsigned long max_hydrogen,
              atomic_ulong* oxygen_consumed, atomic_ulong* hydrogen_consumed, char* error){
    FILE* file;
    srand(time(NULL) ^ (getpid() << 16)); // Random generator using tam and pid, because time alone is too slow

    // START + SLEEP
    sem_wait(semSync);
    atomic_fetch_add(operation_count, 1);
    // printf("%lu: O %lu: started\n", *operation_count, id);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: O %lu: started\n", *operation_count, id);
    fclose(file);
    sem_post(semSync);
    unsigned long wait = rand() % delay + 1;
    usleep(wait * 1000);

    // QUEUE
    sem_wait(semSync);
    atomic_fetch_add(operation_count, 1);
    // printf("%lu: O %lu: going to queue\n", *operation_count, id);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: O %lu: going to queue\n", *operation_count, id);
    fclose(file);
    sem_post(semSync);
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
        sem_wait(semSync);
        atomic_fetch_add(operation_count, 1);
        // printf("%lu: O %lu: not enough H\n", *operation_count, id);
        file = fopen ("proj2.out", "a");
        fprintf(file, "%lu: O %lu: not enough H\n", *operation_count, id);
        fclose(file);
        sem_post(semSync);
        sem_post(semHydrogen);
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
    sem_wait(semSync);
    atomic_fetch_add(molecule_count, 1);
    sem_post(semSync);
    unsigned long moleculeID = *molecule_count; // Start creating molecule and notify two hydrogens that the process began
    sem_wait(semSync);
    atomic_fetch_add(operation_count, 1);
    // printf("%lu: O %lu: creating molecule %lu\n", *operation_count, id, *molecule_count);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: O %lu: creating molecule %lu\n", *operation_count, id, *molecule_count);
    fclose(file);
    sem_post(semSync);
    sem_post(semHydrogen);
    sem_post(semHydrogen);
    wait = rand() % delay + 1;
    usleep(wait * 1000); // Sleep and then notify hydrogens that the process is done
    sem_wait(semSync);
    atomic_fetch_add(operation_count, 1);
    // printf("%lu: O %lu: molecule %lu created\n", *operation_count, id, moleculeID);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: O %lu: molecule %lu created\n", *operation_count, id, moleculeID);
    fclose(file);
    sem_post(semSync);
    sem_post(semMoleculeReady);
    sem_post(semMoleculeReady);
    sem_wait(semSync);
    atomic_fetch_add(oxygen_consumed, 1);
    sem_post(semSync);
    if(*oxygen_consumed == max_oxygen) { // If there is no other oxygen, wake up one hydrogen process, that will kill all it's siblings
      sem_post(semHydrogen);
    }
    sem_wait(semMoleculeCreated);
    sem_wait(semMoleculeCreated);
    sem_post(semOxygen); // Tell next oxygen in queue to start making molecule
    exit(EXIT_SUCCESS);
}