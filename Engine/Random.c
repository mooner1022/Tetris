//
// Created by moonm on 2022-11-29.
//

#include <time.h>
#include "Random.h"

void rand_init() {
    srand((unsigned int)(time(NULL)));
}

int range_rand(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

bool bool_rand() {
    return range_rand(0, 1);
}

/*
void* arr_rand(void* array, size_t mem_size, size_t arr_size) {
    int index = range_rand(0, (int) arr_size);
    return (array + (mem_size * index));
}
 */