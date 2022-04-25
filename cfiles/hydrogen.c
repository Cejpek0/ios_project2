#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 #include <unistd.h>
 #include <semaphore.h>
 #include <stdatomic.h>

// Slave process, which is controlled by oxygen
void hydrogen (unsigned long id, unsigned long delay,
                sem_t* semHydrogen, sem_t* semSync, sem_t* semMoleculeOxygenCreated, sem_t* semTwoHydrogensInQueue,
                sem_t* semMoleculeHydrogenCreated, sem_t* semMoleculeHydrogenCreating, atomic_ulong* operation_count, atomic_ulong* molecule_count,
                unsigned long max_oxygen, unsigned long max_hydrogen,
                atomic_ulong* oxygen_consumed, atomic_ulong* hydrogen_consumed, char* error){
    FILE* file;
    srand(time(NULL) ^ (getpid() << 16)); // Random generator using time and pid, because time alone is too slow
    unsigned long wait = delay > 0 ? rand() % delay + 1 : 0;
    // STARTED
    sem_wait(semSync);
    atomic_fetch_add(operation_count, 1);
    // printf("%lu: H %lu: started\n", *operation_count, id);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: H %lu: started\n", *operation_count, id);
    fclose(file);
    sem_post(semSync);

    // QUEUE
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
    sem_post(semTwoHydrogensInQueue);
    sem_wait(semHydrogen); // Wait till its your turn to create molecule or die

    if(*error) { // If error occurred in main process, die and tell it to everyone
        sem_post(semHydrogen);
        sem_post(semMoleculeOxygenCreated);
        sem_post(semMoleculeOxygenCreated);
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

    // Notify oxygen that printing out "Creating molecule" is completed
    sem_post(semMoleculeHydrogenCreating);

    // Wait until oxygen is done with creating molecule
    sem_wait(semMoleculeOxygenCreated);
    
    if(*error) { // If error occurred in main process, die and tell it to everyone
        sem_post(semHydrogen);
        sem_post(semMoleculeOxygenCreated);
        sem_post(semMoleculeOxygenCreated);
        sem_post(semTwoHydrogensInQueue);
        exit(EXIT_FAILURE);
    }

    sem_wait(semSync);
    atomic_fetch_add(hydrogen_consumed, 1);
    atomic_fetch_add(operation_count, 1);
    // printf("%lu: H %lu: molecule %lu created\n", *operation_count, id, moleculeID);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: H %lu: molecule %lu created\n", *operation_count, id, moleculeID);
    fclose(file);
    sem_post(semSync);

    // Notify oxygen that this hydrogen completed creating molecule
    sem_post(semMoleculeHydrogenCreated); 
    if(*hydrogen_consumed >= max_hydrogen - 1) { // If there is one hydrogen remaining, die.
        sem_post(semHydrogen);
    }
    exit(EXIT_SUCCESS);
}