#ifndef BOARD_H
#define BOARD_H

#include "memory.h"
#include "pieces.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  Array *board;
  bool check_move;
  Player player;
  char* castling;
  char* enpassant;
  uint16_t halfmoves;
  uint16_t fullmoves;
} Board;

Board *new_board(char *fen);

#endif // !BOARD_H
