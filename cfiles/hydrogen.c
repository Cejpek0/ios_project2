#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 #include <unistd.h>
 #include <semaphore.h>
int hydrogen (unsigned long id, unsigned long delay,
                sem_t* semHydrogen, sem_t* semMoleculeReady,sem_t* semTwoHydrogenInQueue,
                unsigned long* operation_count, unsigned long* molecule_count,
                unsigned long* queue_hydrogen, unsigned long max_oxygen, unsigned long max_hydrogen,
                unsigned long* oxygen_consumed, unsigned long* hydrogen_consumed){
                    int error;
    (*operation_count)++;
    srand(getpid());
    printf("%lu: H %lu: started\n", *operation_count, id);
    //QUEUE
    unsigned long x = rand() % delay + 1;
    usleep(x * 1000);     
    (*operation_count)++;
    if(*oxygen_consumed == max_oxygen) { // If all oxygens are consumed, die and wake up other hydrogen, so it can die
        printf("%lu: H %lu: not enough O or H\n", *operation_count, id);
        error = sem_post(semHydrogen);
        if(error == -1) {
            printf("FUCK");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    printf("%lu: H %lu: going to queue\n", *operation_count, id);
    (*queue_hydrogen)++;
    
    error = sem_post(semTwoHydrogenInQueue);
    if(error == -1) {
        printf("FUCK");
        return EXIT_FAILURE;
    }
    sem_wait(semHydrogen); // Wait till its your turn to create molecule or die
    if(*oxygen_consumed == max_oxygen || *hydrogen_consumed >= max_hydrogen - 1) { // If there is one hydrogen remaining, die. If there is no oxygen remaining, die.
        (*operation_count)++;
        printf("%lu: H %lu: not enough O or H\n", *operation_count, id);
        error = sem_post(semHydrogen); // and wake up next hydrogen, so it can die
        if(error == -1) {
            printf("FUCK");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    unsigned long moleculeID = *molecule_count;
    (*operation_count)++;
    printf("%lu: H %lu: creating molecule %lu\n", *operation_count, id, *molecule_count);
    sem_wait(semMoleculeReady);// Wait until oxygen is done with creating molecule
    (*hydrogen_consumed)++;
    (*operation_count)++;
    printf("%lu: H %lu: molecule %lu created\n", *operation_count, id, moleculeID);
    if(*hydrogen_consumed >= max_hydrogen - 1) { // If there is one hydrogen remaining, die.
        error = sem_post(semHydrogen);// and wake up next, hydrogen, so he can realize there is no place for him in this world and he should just die
        if(error == -1) {
            printf("FUCK");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}