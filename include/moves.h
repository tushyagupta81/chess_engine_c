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

bool do_move(Board *board, char *move_string);
Player get_piece_color(Board *board, uint16_t i, uint16_t j);
bool checkmate(Board *board);
void pseudo_do_move(Board *board, Move *move);
void undo_move(Board *board, Move *move);
void print_move(Move *move);
void do_encoded_move(Board *board, Move *move);
bool check_check(Board*board, bool printErr);

#endif // !MOVES_H
