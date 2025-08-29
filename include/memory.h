#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include <stdint.h>

#define MIN_LEN 64

typedef struct {
  void *values;
  int curr_length;
  int max_length;
  int elem_size;
} Array;

Array* new_array(ValueTypes type);
void append(Array *array, void *token);
void pop(Array *array);
void set_at(Array *array, void* token, uint16_t index);

#endif // !MEMORY_H
