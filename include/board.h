#ifndef BOARD_H
#define BOARD_H

#include "memory.h"
#include "pieces.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  Array *board;
  bool check_move;
  Player player;
} Board;

Board *new_board(char *fen);

#endif // !BOARD_H
