#ifndef BOARD_H
#define BOARD_H

#include "memory.h"
#include "pieces.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint16_t row;
  uint16_t col;
} Position;

typedef struct {
  bool valid;
  uint16_t row;
  uint16_t col;
} Enpassent;

typedef struct {
  Array *board;
  bool check_move;
  Player player;
  char *castling;
  Enpassent enpassant;
  uint16_t halfmoves;
  uint16_t fullmoves;
} Board;

#define INBOUNDS(r, c) ((r) < 8 && (c) < 8)
#define GET_PIECE(r, c) *(Pieces *)get_at_2d(board->board, r, c)

Board *new_board(char *fen);
Array *get_moves(Array *moves, Board *board, uint16_t i, uint16_t j);
void print_board(Board *board);
void print_board_info(Board *board);
bool cmp_pos(Position *a, Position *b);

#endif // !BOARD_H
