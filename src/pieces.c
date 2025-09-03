#include "pieces.h"
#include "board.h"
#include "memory.h"
#include "moves.h"
#include "types.h"
#include <stdint.h>

#define ADD_MOVES(r, c)                                                        \
  do {                                                                         \
    if (INBOUNDS(r, c)) {                                                      \
      Position pos = {r, c};                                                   \
      append(moves, &pos);                                                     \
    }                                                                          \
  } while (0)

void move_straight(Board *board, Array *moves, uint16_t i, uint16_t j,
                   Player enemy_color) {
  uint16_t old_i = i, old_j = j;
  for (int k = -1; k < 2; k += 2) {
    for (int m = 0; m < 2; m++) {
      i = old_i;
      j = old_j;
      while (get_piece_color(board, i + (m % 2 == 0 ? 0 : k),
                             j + (m % 2 == 0 ? k : 0)) == None) {
        ADD_MOVES(i + (m % 2 == 0 ? 0 : k), j + (m % 2 == 0 ? k : 0));
        i += (m % 2 == 0 ? 0 : k);
        j += (m % 2 == 0 ? k : 0);
      }
      if (get_piece_color(board, i + (m % 2 == 0 ? 0 : k),
                          j + (m % 2 == 0 ? k : 0)) == enemy_color) {
        ADD_MOVES(i + (m % 2 == 0 ? 0 : k), j + (m % 2 == 0 ? k : 0));
      }
    }
  }
}

void move_diagonal(Board *board, Array *moves, uint16_t i, uint16_t j,
                   Player enemy_color) {
  uint16_t old_i = i, old_j = j;
  for (int k = -1; k < 2; k += 2) {
    for (int m = -1; m < 2; m += 2) {
      i = old_i;
      j = old_j;
      while (get_piece_color(board, i + k, j + m) == None) {
        ADD_MOVES(i + k, j + m);
        i += k;
        j += m;
      }
      if (get_piece_color(board, i + k, j + m) == enemy_color) {
        ADD_MOVES(i + k, j + m);
      }
    }
  }
}

Array *get_moves(Board *board, uint16_t i, uint16_t j) {
  Pieces piece = GET_PIECE(i, j);
  Array *moves = new_array(ValuePosition);
  switch (piece) {
  case WhitePawn:
    for (int col = -1; col < 2; col++) {
      if (col == 0) {
        if (get_piece_color(board, i - 1, j + col) == None) {
          ADD_MOVES(i - 1, j + col);
        }
      } else if (get_piece_color(board, i - 1, j + col) == Black) {
        ADD_MOVES(i - 1, j + col);
      } else if (board->enpassant.valid && board->enpassant.row == i - 1 &&
                 board->enpassant.col == j + col) {
        ADD_MOVES(i - 1, j + col);
      }
    }
    if (i == 6) {
      ADD_MOVES(i - 2, j);
    }
    break;
  case BlackPawn:
    for (int col = -1; col < 2; col++) {
      if (col == 0) {
        if (get_piece_color(board, i + 1, j + col) == None) {
          ADD_MOVES(i + 1, j + col);
        }
      } else if (get_piece_color(board, i + 1, j + col) == White) {
        ADD_MOVES(i + 1, j + col);
      } else if (board->enpassant.valid && board->enpassant.row == i + 1 &&
                 board->enpassant.col == j + col) {
        ADD_MOVES(i + 1, j + col);
      }
    }
    if (i == 1) {
      ADD_MOVES(i + 2, j);
    }
    break;
  case WhiteRook:
    move_straight(board, moves, i, j, Black);
    break;
  case BlackRook:
    move_straight(board, moves, i, j, White);
    break;
  case WhiteBishop:
    move_diagonal(board, moves, i, j, Black);
    break;
  case BlackBishop:
    move_diagonal(board, moves, i, j, White);
    break;
  case WhiteQueen:
    move_straight(board, moves, i, j, Black);
    move_diagonal(board, moves, i, j, Black);
    break;
  case BlackQueen:
    move_straight(board, moves, i, j, White);
    move_diagonal(board, moves, i, j, White);
    break;
  case WhiteKnight: {
    int dr[8] = {2, 2, -2, -2, 1, 1, -1, -1};
    int dc[8] = {1, -1, 1, -1, 2, -2, 2, -2};
    for (int k = 0; k < 8; k++) {
      Player piece_color = get_piece_color(board, i + dr[k], j + dc[k]);
      if (piece_color != White && piece_color != OutOfBounds) {
        ADD_MOVES(i + dr[k], j + dc[k]);
      }
    }
    break;
  }
  case BlackKnight: {
    int dr[8] = {2, 2, -2, -2, 1, 1, -1, -1};
    int dc[8] = {1, -1, 1, -1, 2, -2, 2, -2};
    for (int k = 0; k < 8; k++) {
      Player piece_color = get_piece_color(board, i + dr[k], j + dc[k]);
      if (piece_color != Black && piece_color != OutOfBounds) {
        ADD_MOVES(i + dr[k], j + dc[k]);
      }
    }
    break;
  }
  case WhiteKing: {
    int dr[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int dc[8] = {0, 1, 1, 1, 0, -1, -1, -1};
    for (int k = 0; k < 8; k++) {
      Player piece_color = get_piece_color(board, i + dr[k], j + dc[k]);
      if (piece_color != White && piece_color != OutOfBounds) {
        ADD_MOVES(i + dr[k], j + dc[k]);
      }
    }
    break;
  }
  case BlackKing: {
    int dr[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int dc[8] = {0, 1, 1, 1, 0, -1, -1, -1};
    for (int k = 0; k < 8; k++) {
      Player piece_color = get_piece_color(board, i + dr[k], j + dc[k]);
      if (piece_color != Black && piece_color != OutOfBounds) {
        ADD_MOVES(i + dr[k], j + dc[k]);
      }
    }
    break;
  }
  case Blank:
    break;
  }
  return moves;
}
