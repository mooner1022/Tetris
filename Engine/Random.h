//
// Created by moonm on 2022-11-29.
//

#ifndef TETRIS_RANDOM_H
#define TETRIS_RANDOM_H

#include <stdlib.h>
#include <stdbool.h>

void rand_init();

int range_rand(int min, int max);
bool bool_rand();
void* arr_rand(void* array, size_t mem_size, size_t arr_size);

#endif
