#ifndef MOVES_H
#define MOVES_H

#include "board.h"
#include "pieces.h"

typedef struct {
  Position start;
  Pieces start_piece;
  Position end;
  Pieces end_piece;
} Move;

void do_move(Board *board, char *move_string);
Player get_piece_color(Board *board, uint16_t i, uint16_t j);

#endif // !MOVES_H
