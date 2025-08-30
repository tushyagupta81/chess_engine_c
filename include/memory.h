#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include <stdint.h>

#define MIN_LEN 8

typedef struct {
  void *values;
  int curr_length;
  int max_length;
  int elem_size;
} Array;

Array *new_array(ValueTypes type);
void append(Array *array, void *token);
void pop(Array *array);
void set_at(Array *array, void *token, uint16_t index);
void *get_at(Array *array, uint16_t index);
void *get_at_2d(Array *array, uint16_t i, uint16_t j);
void set_at_2d(Array *array, void *token, uint16_t i, uint16_t j);

#endif // !MEMORY_H
