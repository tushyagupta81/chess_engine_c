#include "moves.h"
#include "board.h"
#include "memory.h"
#include "pieces.h"
#include "types.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_move(Move *move) {
  printf("Start: %d, %d\n", move->start.row, move->start.col);
  printf("End:   %d, %d\n", move->end.row, move->end.col);
}

void pseudo_do_move(Board *board, Move *move) {
  set_at_2d(board->board, (Pieces[]){move->start_piece}, move->end.row,
            move->end.col);
  set_at_2d(board->board, (Pieces[]){Blank}, move->start.row, move->start.col);
  if (move->start_piece == WhiteKing) {
    board->whiteKing.row = move->end.row;
    board->whiteKing.col = move->end.col;
  } else if (move->start_piece == BlackKing) {
    board->blackKing.row = move->end.row;
    board->blackKing.col = move->end.col;
  }
}

void undo_move(Board *board, Move *move) {
  if (move->start_piece == WhiteKing) {
    board->whiteKing.row = move->start.row;
    board->whiteKing.col = move->start.col;
  } else if (move->start_piece == BlackKing) {
    board->blackKing.row = move->start.row;
    board->blackKing.col = move->start.col;
  }
  set_at_2d(board->board, &move->end_piece, move->end.row, move->end.col);
  set_at_2d(board->board, &move->start_piece, move->start.row, move->start.col);
}

bool checkmate(Board *board) {
  Array *positions = new_array(ValuePosition);
  Array *moves = new_array(ValueMoves);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (board->player == Black) {
        if (get_piece_color(board, i, j) == Black) {
          get_moves(positions, board, i, j);
        }
      } else {
        if (get_piece_color(board, i, j) == White) {
          get_moves(positions, board, i, j);
        }
      }
      for (int k = 0; k < positions->curr_length; k++) {
        Position p = *(Position *)get_at(positions, k);
        append(moves, (Move[]){{
                          (Position){i, j},
                          GET_PIECE(i, j),
                          p,
                          GET_PIECE(p.row, p.col),
                      }});
      }
      while (positions->curr_length != 0) {
        pop(positions);
      }
    }
  }
  for (int i = 0; i < moves->curr_length; i++) {
    Move m = *(Move *)get_at(moves, i);
    pseudo_do_move(board, &m);
    if (board->player == Black) {
      if (get_check_status(board, &board->blackKing) == true) {
        undo_move(board, &m);
      } else {
        undo_move(board, &m);
        deinit_array(moves);
        deinit_array(positions);
        return false;
      }
    } else {
      if (get_check_status(board, &board->whiteKing) == true) {
        undo_move(board, &m);
      } else {
        undo_move(board, &m);
        deinit_array(moves);
        deinit_array(positions);
        return false;
      }
    }
  }
  deinit_array(moves);
  deinit_array(positions);
  return true;
}

bool check_bounds(Move *move) {
  if (move->start.row < 8 && move->start.col < 8 && move->end.row < 8 &&
      move->end.col < 8) {
    return true;
  }
  return false;
}

bool string_alnum(char *move_string) {
  int i = 0;
  while (move_string[i] != '\0') {
    if (!isalnum(move_string[i])) {
      return false;
    }
    i++;
  }
  return true;
}

bool decode_move(char *move_string, Move *move) {
  int len = strlen(move_string);
  if (len != 4 && len != 5) {
    fprintf(stderr, "Move has to be of len 4 or 5\n");
    return false;
  }

  bool alnum = string_alnum(move_string);
  if (!alnum) {
    fprintf(stderr, "Invalid char in move\n");
    return false;
  }

  char start_row = move_string[0];
  char start_col = move_string[1];
  char end_row = move_string[2];
  char end_col = move_string[3];

  move->start.row = 7 - (start_row - 'a');
  move->start.col = start_col - '1';
  move->end.row = 7 - (end_row - 'a');
  move->end.col = end_col - '1';

  bool bounds = check_bounds(move);
  if (!bounds) {
    fprintf(stderr, "Move is out of bounds\n");
    print_move(move);
    return false;
  }

  return true;
}

Player get_piece_color(Board *board, uint16_t i, uint16_t j) {
  if (!INBOUNDS(i, j)) {
    return OutOfBounds;
  }
  Pieces piece = GET_PIECE(i, j);
  if (piece == Blank) {
    return None;
  }
  if (piece < 7) {
    return White;
  }
  return Black;
}

void do_move(Board *board, char *move_string) {
  Move move;
  bool valid = decode_move(move_string, &move);
  if (!valid) {
    fprintf(stderr, "Invalid move: '%s'\n", move_string);
    return;
  }

  Player start_color = get_piece_color(board, move.start.row, move.start.col);
  if (start_color != board->player) {
    fprintf(stderr, "You can only use your own pieces\n");
    fprintf(stderr, "Invalid move: '%s'\n", move_string);
    return;
  }

  Player end_color = get_piece_color(board, move.end.row, move.end.col);
  if (start_color == end_color) {
    fprintf(stderr, "Same colors cannot attack each other\n");
    fprintf(stderr, "Invalid move: '%s'\n", move_string);
    return;
  }

  // print_move(&move);
  move.start_piece = GET_PIECE(move.start.row, move.start.col);
  move.end_piece = GET_PIECE(move.end.row, move.end.col);

  Array *moves = new_array(ValuePosition);
  get_moves(moves, board, move.start.row, move.start.col);
  bool found_move = false;
  for (int i = 0; i < moves->curr_length; i++) {
    Position *a = get_at(moves, i);
    if (cmp_pos(a, &(move.end))) {
      found_move = true;
      break;
    }
  }
  deinit_array(moves);

  if (!found_move) {
    fprintf(stderr, "Move not possible\n");
    fprintf(stderr, "Invalid move: '%s'\n", move_string);
    return;
  }

  set_at_2d(board->board, (Pieces[]){move.start_piece}, move.end.row,
            move.end.col);
  set_at_2d(board->board, (Pieces[]){Blank}, move.start.row, move.start.col);
  if (move.start_piece == WhiteKing) {
    board->whiteKing.row = move.end.row;
    board->whiteKing.col = move.end.col;
  } else if (move.start_piece == BlackKing) {
    board->blackKing.row = move.end.row;
    board->blackKing.col = move.end.col;
  }

  if (board->check_move == true) {
    bool now_check;
    if (board->player == White) {
      now_check = get_check_status(board, &board->whiteKing);
    } else {
      now_check = get_check_status(board, &board->blackKing);
    }
    if (now_check == true) {
      fprintf(stderr, "Your move does not solve the check\n");
      undo_move(board, &move);
      return;
    }
  }

  if ((move.start_piece == WhitePawn || move.start_piece == BlackPawn) &&
      (board->enpassant.valid)) {
    if (board->enpassant.row == move.end.row &&
        board->enpassant.col == move.end.col) {
      if (move.start_piece == WhitePawn) {
        set_at_2d(board->board, (Pieces[]){Blank}, move.end.row + 1,
                  move.end.col);
      } else {
        set_at_2d(board->board, (Pieces[]){Blank}, move.end.row - 1,
                  move.end.col);
      }
    }
  }

  if ((move.start_piece == WhitePawn || move.start_piece == BlackPawn) &&
      (abs(move.start.row - move.end.row) == 2)) {
    if (move.end.row < move.start.row) {
      board->enpassant.row = move.end.row + 1;
      board->enpassant.col = move.end.col;
      board->enpassant.valid = true;
    } else {
      board->enpassant.row = move.end.row - 1;
      board->enpassant.col = move.end.col;
      board->enpassant.valid = true;
    }
  } else {
    board->enpassant.valid = false;
  }

  if (move.start_piece == WhitePawn || move.start_piece == BlackPawn) {
    board->halfmoves = 0;
  } else if (end_color == White || end_color == Black) {
    board->halfmoves = 0;
  } else {
    board->halfmoves++;
  }

  if (board->player == White) {
    board->check_move = get_check_status(board, &board->blackKing);
  } else if (board->player == Black) {
    board->check_move = get_check_status(board, &board->whiteKing);
  }

  if (board->player == White) {
    board->player = Black;
  } else {
    board->player = White;
    board->fullmoves++;
  }

  if (board->check_move == true) {
    board->checkmate = checkmate(board);
  }
}
