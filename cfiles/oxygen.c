#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 #include <unistd.h>
  #include <semaphore.h>
void oxygen (unsigned long id, unsigned long delay, unsigned long* queue_oxygen,
                unsigned long* queue_hydrogen, unsigned long max_oxygen, unsigned long max_hydrogen,
                unsigned long* oxygen_count, unsigned long* hydrogen_count, unsigned long* operation_count,
                sem_t* semOxygen, unsigned long* molecule_count, sem_t* semMolecule, sem_t* semHydrogen){
    srand(getpid()+time(NULL));
    (*operation_count)++;
    printf("%lu: O %lu: started\n", *operation_count, id);
    unsigned long x = rand() % delay + 1;
    usleep(x * 1000);
    (*oxygen_count)++;
    (*operation_count)++;
    printf("%lu: O %lu: going to queue\n", *operation_count, id);
    (*queue_oxygen)++;
    sem_wait(semOxygen);
    sem_wait(semMolecule);
    (*operation_count)++;
    (*molecule_count)++;
    unsigned long moleculeID = *molecule_count;
    printf("%lu: O %lu: creating molecule %lu\n", *operation_count, id, *molecule_count);
    sem_post(semHydrogen);
    sem_post(semHydrogen);
    x = rand() % delay + 1;
    usleep(x * 1000);
    sem_post(semMolecule);
    sem_post(semMolecule);
    (*operation_count)++;
    printf("%lu: O %lu: molecule %lu created\n", *operation_count, id, moleculeID);
    sem_post(semOxygen);
    return;
    (void)hydrogen_count;
    (void)max_hydrogen;
    (void)max_oxygen;
    (void)queue_hydrogen;
}