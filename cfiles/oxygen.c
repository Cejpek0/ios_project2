#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdatomic.h>

// Master process that is controlling whole molecule creation
void oxygen (unsigned long id, unsigned long delay, unsigned long delayMolecule,
              sem_t* semOxygen, sem_t* semHydrogen, sem_t* semSync, sem_t* semMoleculeOxygenCreated,sem_t* semTwoHydrogensInQueue,
              sem_t* semMoleculeHydrogenCreated, sem_t* semMoleculeHydrogenCreating, atomic_ulong* operation_count,atomic_ulong* molecule_count,
              unsigned long max_oxygen, unsigned long max_hydrogen,
              atomic_ulong* oxygen_consumed, atomic_ulong* hydrogen_consumed, char* error){
    FILE* file;
    srand(time(NULL) ^ (getpid() << 16)); // Random generator using time and pid, because time alone is too slow
    unsigned long wait = delay > 0 ? rand() % delay + 1 : 0;

    // START + SLEEP
    sem_wait(semSync);
    atomic_fetch_add(operation_count, 1);
    // printf("%lu: O %lu: started\n", *operation_count, id);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: O %lu: started\n", *operation_count, id);
    fclose(file);
    sem_post(semSync);
    
    usleep(wait * 1000);

    // QUEUE
    sem_wait(semSync);
    atomic_fetch_add(operation_count, 1);
    // printf("%lu: O %lu: going to queue\n", *operation_count, id);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: O %lu: going to queue\n", *operation_count, id);
    fclose(file);
    sem_post(semSync);

    // Waiting in queue to start making molecule, or to die
    sem_wait(semOxygen); 
    
    if(*error) { // If error occurred in main process, die and tell it to everyone
        sem_post(semOxygen);
        sem_post(semHydrogen);
        sem_post(semMoleculeOxygenCreated);
        sem_post(semMoleculeOxygenCreated);
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

    // Wait for signal from 2 hydrogens, to start makig molecule 
    sem_wait(semTwoHydrogensInQueue);
    sem_wait(semTwoHydrogensInQueue);

    if(*error) { // If error occurred in main process, die and tell it to everyone
        sem_post(semOxygen);
        sem_post(semHydrogen);
        sem_post(semMoleculeOxygenCreated);
        sem_post(semMoleculeOxygenCreated);
        sem_post(semTwoHydrogensInQueue);
        exit(EXIT_FAILURE);
    }

    sem_wait(semSync);
    atomic_fetch_add(molecule_count, 1);
    sem_post(semSync);

    // Start creating molecule
    unsigned long moleculeID = *molecule_count; 
    sem_wait(semSync);
    atomic_fetch_add(operation_count, 1);
    // printf("%lu: O %lu: creating molecule %lu\n", *operation_count, id, *molecule_count);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: O %lu: creating molecule %lu\n", *operation_count, id, *molecule_count);
    fclose(file);
    sem_post(semSync);

    // Notify two hydrogens that the creation began 
    sem_post(semHydrogen); 
    sem_post(semHydrogen);
    wait = delayMolecule > 0 ? rand() % delayMolecule + 1 : 0;
    usleep(wait * 1000); // Sleep

    // Wait for two hydrogens completing printing out "Creating molecule"
    sem_wait(semMoleculeHydrogenCreating);
    sem_wait(semMoleculeHydrogenCreating);

    sem_wait(semSync);
    atomic_fetch_add(operation_count, 1);
    // printf("%lu: O %lu: molecule %lu created\n", *operation_count, id, moleculeID);
    file = fopen ("proj2.out", "a");
    fprintf(file, "%lu: O %lu: molecule %lu created\n", *operation_count, id, moleculeID);
    fclose(file);
    sem_post(semSync);

    //Notify two hydrogens that molecule is created
    sem_post(semMoleculeOxygenCreated);
    sem_post(semMoleculeOxygenCreated);

    sem_wait(semSync);
    atomic_fetch_add(oxygen_consumed, 1);
    sem_post(semSync);

    // Wait for two hydrogens to finish printing out "Molecule created"
    sem_wait(semMoleculeHydrogenCreated);
    sem_wait(semMoleculeHydrogenCreated);

    if(*oxygen_consumed == max_oxygen) { // If there is no other oxygen, wake up one hydrogen process, that will kill all it's siblings
      sem_post(semHydrogen);
    }
    sem_post(semOxygen); // Tell next oxygen in queue to start making molecule
    exit(EXIT_SUCCESS);
}