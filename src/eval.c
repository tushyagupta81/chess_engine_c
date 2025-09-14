#include "eval.h"
#include "board.h"
#include "moves.h"
#include "piece_square.h"
#include "pieces.h"
#include <stdbool.h>
#include <stdint.h>

int get_piece_value(Pieces piece, uint16_t i, uint16_t j) {
  // TODO: see https://www.chessprogramming.org/CPW-Engine_eval
  int white_index = i * 8 + j;
  int black_index = 64 - (i * 8 + j + 1);
  switch (piece) {
  case Blank:
    return 0;
  case WhitePawn:
    return 100 + pawn_piece_sqaure[white_index];
  case WhiteRook:
    return 500 + rook_piece_sqaure[white_index];
  case WhiteBishop:
    return 330 + bishop_piece_sqaure[white_index];
  case WhiteKnight:
    return 320 + knight_piece_sqaure[white_index];
  case WhiteQueen:
    return 900 + queen_piece_sqaure[white_index];
  case WhiteKing:
    return 20000 + king_piece_sqaure[white_index];
  case BlackPawn:
    return 100 + pawn_piece_sqaure[black_index];
  case BlackRook:
    return 500 + rook_piece_sqaure[black_index];
  case BlackBishop:
    return 330 + bishop_piece_sqaure[black_index];
  case BlackKnight:
    return 320 + knight_piece_sqaure[black_index];
  case BlackQueen:
    return 900 + queen_piece_sqaure[black_index];
  case BlackKing:
    return 20000 + king_piece_sqaure[black_index];
  }
  return 0;
}

int get_board_score(Board *board, Player color) {
  int res = 0;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (get_piece_color(board, i, j) == color) {
        res += get_piece_value(GET_PIECE(i, j), i, j);
      }
    }
  }
  return res;
}
