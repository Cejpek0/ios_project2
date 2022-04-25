#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 #include <unistd.h>
 #include <semaphore.h>
 #include <stdatomic.h>
int hydrogen (unsigned long id, unsigned long delay,
                sem_t* semHydrogen, sem_t* semSync, sem_t* semMoleculeReady, sem_t* semTwoHydrogensInQueue,
                sem_t* semMoleculeCreated, atomic_ulong* operation_count, atomic_ulong* molecule_count,
                atomic_ulong* queue_hydrogen, unsigned long max_oxygen, unsigned long max_hydrogen,
                atomic_ulong* oxygen_consumed, atomic_ulong* hydrogen_consumed, char* error){
    FILE* file;
    srand(time(NULL) ^ (getpid() << 16)); // Random generator using tam and pid, because time alone is too slow
    sem_wait(semSync);
    atomic_fetch_add(operation_count, 1);
    // printf("%lu: H %lu: started\n", *operation_count, id);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: H %lu: started\n", *operation_count, id);
    fclose(file);
    sem_post(semSync);

    //QUEUE
    unsigned long wait = rand() % delay + 1;
    usleep(wait * 1000);
    if(*oxygen_consumed == max_oxygen) { // If all oxygens are consumed, die and wake up other hydrogen, so it can die
        sem_wait(semSync);
        atomic_fetch_add(operation_count, 1);
        // printf("%lu: H %lu: not enough O or H\n", *operation_count, id);
        file = fopen ("proj2.out", "a");
        fprintf(file, "%lu: H %lu: not enough O or H\n", *operation_count, id);
        fclose(file);
        sem_post(semSync);
        sem_post(semHydrogen);
    }
    sem_wait(semSync);
    atomic_fetch_add(operation_count, 1);
    // printf("%lu: H %lu: going to queue\n", *operation_count, id);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: H %lu: going to queue\n", *operation_count, id);
    fclose(file);
    sem_post(semSync);
    sem_wait(semSync);
    atomic_fetch_add(queue_hydrogen, 1);
    sem_post(semSync);
    sem_post(semTwoHydrogensInQueue);
    sem_wait(semHydrogen); // Wait till its your turn to create molecule or die
    if(*error) { // If error occurred in main process, die and tell it to everyone
        sem_post(semHydrogen);
        sem_post(semMoleculeReady);
        sem_post(semMoleculeReady);
        sem_post(semTwoHydrogensInQueue);
        exit(EXIT_FAILURE);
    }
    if(*oxygen_consumed == max_oxygen || *hydrogen_consumed >= max_hydrogen - 1) { // If there is one hydrogen remaining, die. If there is no oxygen remaining, die.
        sem_wait(semSync);
        atomic_fetch_add(operation_count, 1);
        // printf("%lu: H %lu: not enough O or H\n", *operation_count, id);
        file = fopen ("proj2.out", "a");
        fprintf(file, "%lu: H %lu: not enough O or H\n", *operation_count, id);
        fclose(file);
        sem_post(semSync);
        sem_post(semHydrogen); // and wake up next hydrogen, so it can die
        exit(EXIT_SUCCESS);
    }
    unsigned long moleculeID = *molecule_count;
    sem_wait(semSync);
    atomic_fetch_add(operation_count, 1);
    // printf("%lu: H %lu: creating molecule %lu\n", *operation_count, id, *molecule_count);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: H %lu: creating molecule %lu\n", *operation_count, id, *molecule_count);
    fclose(file);
    sem_post(semSync);
    sem_wait(semMoleculeReady);// Wait until oxygen is done with creating molecule
    if(*error) { // If error occurred in main process, die and tell it to everyone
        sem_post(semHydrogen);
        sem_post(semMoleculeReady);
        sem_post(semMoleculeReady);
        sem_post(semTwoHydrogensInQueue);
        exit(EXIT_FAILURE);
    }
    sem_wait(semSync);
    atomic_fetch_add(hydrogen_consumed, 1);
    sem_post(semSync);   
    sem_wait(semSync);
    atomic_fetch_add(operation_count, 1);
    // printf("%lu: H %lu: molecule %lu created\n", *operation_count, id, moleculeID);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: H %lu: molecule %lu created\n", *operation_count, id, moleculeID);
    fclose(file);
    sem_post(semSync);
    sem_post(semMoleculeCreated);
    if(*hydrogen_consumed >= max_hydrogen - 1) { // If there is one hydrogen remaining, die.
        sem_post(semHydrogen);
    }
    exit(EXIT_SUCCESS);
}