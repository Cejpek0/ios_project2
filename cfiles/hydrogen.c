#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 #include <unistd.h>
 #include <semaphore.h>
void hydrogen (unsigned long id, unsigned long delay, unsigned long* queue_hydrogen,
                unsigned long* queue_oxygen, unsigned long max_oxygen, unsigned long max_hydrogen,
                unsigned long* oxygen_count, unsigned long* hydrogen_count, unsigned long* operation_count,
                sem_t* semHydrogen, unsigned long* molecule_count, sem_t* semMolecule){
    (*operation_count)++;
    srand(getpid());
    printf("%lu: H %lu: started\n", *operation_count, id);
    unsigned long x = rand() % delay + 1;
    usleep(x * 1000);
    (*hydrogen_count)++;
    (*operation_count)++;
    printf("%lu: H %lu: going to queue\n", *operation_count, id);
    sem_wait(semHydrogen);
    (*operation_count)++;
    unsigned long moleculeID = *molecule_count;
    printf("%lu: H %lu: creating molecule %lu\n", *operation_count, id, *molecule_count);
    sem_wait(semMolecule);
    (*operation_count)++;
    printf("%lu: H %lu: molecule %lu created\n", *operation_count, id, moleculeID);
    return;
    (*queue_hydrogen)++;
    (void)oxygen_count;
    (void)max_hydrogen;
    (void)max_oxygen;
    (void)queue_oxygen;
}