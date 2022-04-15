#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 #include <unistd.h>

#include "../hfiles/oxygen.h"
void oxygen (unsigned long id, unsigned long time, unsigned long* queue_oxygen,
                unsigned long* queue_hydrogen, unsigned long max_oxygen, unsigned long max_hydrogen,
                unsigned long* oxygen_count, unsigned long* hydrogen_count){
    printf("%lu: O %lu: started\n", *queue_oxygen, id);
    unsigned long x = rand() % time + 1;
    usleep(x * 1000);
    printf("%lu: O %lu: going to queue\n", *queue_oxygen, id);
    (*queue_oxygen)++;
}