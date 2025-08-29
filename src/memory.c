#include "memory.h"
#include "pieces.h"
#include "types.h"
#include <stdlib.h>
#include <string.h>

Array *new_array(ValueTypes type) {
  Array *array = (Array *)malloc(sizeof(Array));

  switch (type) {
  case ValuePiece:
    array->elem_size = sizeof(Pieces);
    array->values = (Pieces *)malloc(array->elem_size * MIN_LEN);
    break;
  default:
    array->elem_size = sizeof(Pieces);
    array->values = (Pieces *)malloc(array->elem_size * MIN_LEN);
  }

  array->curr_length = 0;
  array->max_length = MIN_LEN;

  return array;
}

void append(Array *array, void *token) {
  if (array->curr_length < array->max_length) {
    char *base = (char *)array->values;
    memcpy(base + array->curr_length * array->elem_size, token,
           array->elem_size);
    array->curr_length+=1;
  } else {
    array->values =
        realloc(array->values, array->elem_size * array->max_length * 2);
    array->max_length *= 2;
    append(array, token);
  }
}

void pop(Array *array) {
  if (array->curr_length > 0) {
    array->curr_length -= 1;
  }
}

void set_at(Array *array, void *token, uint16_t index) {
  if (array->curr_length > index) {
    char *base = (char *)array->values;
    memcpy(base + index * array->elem_size, token,
           array->elem_size);
  } else {
    append(array, token);
  }
}
