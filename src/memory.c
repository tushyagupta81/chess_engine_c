#include "memory.h"
#include "board.h"
#include "pieces.h"
#include "types.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Array *new_array(ValueTypes type) {
  Array *array = malloc(sizeof(Array));

  switch (type) {
  case ValuePiece:
    array->elem_size = sizeof(Pieces);
    array->values = malloc(array->elem_size * MIN_LEN);
    break;
  case ValueArray:
    array->elem_size = sizeof(Array *);
    array->values = malloc(array->elem_size * MIN_LEN);
    break;
  case ValuePosition:
    array->elem_size = sizeof(Position);
    array->values = malloc(array->elem_size * MIN_LEN);
    break;
  default:
    fprintf(stderr, "Unknow type");
    exit(1);
  }

  array->curr_length = 0;
  array->max_length = MIN_LEN;

  return array;
}

void *get_at(Array *array, uint16_t index) {
  if (index >= array->curr_length) {
    return NULL;
  }
  return array->values + index * array->elem_size;
}

void *get_at_2d(Array *array, uint16_t i, uint16_t j) {
  if (i >= array->curr_length) {
    return NULL;
  }
  Array *row = ((Array**)array->values)[i];
  if (j >= row->curr_length) {
    return NULL;
  }
  return row->values + j * row->elem_size;
}

void append(Array *array, void *token) {
  if (array->curr_length < array->max_length) {
    char *base = array->values;
    memcpy(base + array->curr_length * array->elem_size, token,
           array->elem_size);
    array->curr_length += 1;
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
    char *base = array->values;
    memcpy(base + index * array->elem_size, token, array->elem_size);
  } else {
    append(array, token);
  }
}

void set_at_2d(Array *array, void *token, uint16_t i, uint16_t j) {
  if (array->curr_length > i) {
    Array *row = ((Array **)array->values)[i];
    if (row->curr_length > j) {
      char *base = row->values;
      memcpy(base + j * row->elem_size, token, row->elem_size);
    } else {
      append(row, token);
    }
  }
}
