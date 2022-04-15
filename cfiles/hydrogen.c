#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 #include <unistd.h>
 #include <semaphore.h>
#include "../hfiles/hydrogen.h"
void hydrogen (unsigned long id, unsigned long time, unsigned long* queue_hydrogen,
                unsigned long* queue_oxygen, unsigned long max_oxygen, unsigned long max_hydrogen,
                unsigned long* oxygen_count, unsigned long* hydrogen_count){
    printf("%lu: H %lu: started\n", *queue_hydrogen, id);
    unsigned long x = rand() % time + 1;
    usleep(x * 1000);
    printf("%lu: H %lu: going to queue\n", *queue_hydrogen, id);
    (*hydrogen_count)++;
    (*queue_hydrogen)++;
}