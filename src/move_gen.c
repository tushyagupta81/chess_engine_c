#include "move_gen.h"
#include "board.h"
#include "eval.h"
#include "memory.h"
#include "moves.h"
#include "nn_model.h"
#include "pieces.h"
#include "types.h"
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int max(int a, int b) {
  if (a > b) {
    return a;
  }
  return b;
}

int min(int a, int b) {
  if (a < b) {
    return a;
  }
  return b;
}

int alpha_beta(Board *board, bool maximizingPlayer, Player player, int alpha,
               int beta, int depth, Move *move_todo) {
  if (board->halfmoves > 100) {
    return 0;
  }

  if (depth >= MAX_DEPTH) {
    if (model_is_valid() == -1) {
      return get_board_score(board, get_opponent(player));
    }
    char fen[256];
    if (!board_to_fen(board, fen)) {
      return 0;
    }
    float nn_value = predict_fen(fen);
    // Convert float to int for your alpha-beta return type
    return (int)(nn_value * INT_MAX);
  }

  Array *moves = new_array(ValueMoves);
  Array *positions = new_array(ValuePosition);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      positions->curr_length = 0;
      if (get_piece_color(board, i, j) == player) {
        get_moves(positions, board, i, j);
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
    }
  }
  deinit_array(positions);

  if (moves->curr_length == 0) {
    if (check_check(board, false)) {
      return maximizingPlayer ? INT_MAX - MAX_DEPTH + depth
                              : INT_MIN + MAX_DEPTH - depth;
    }
    // Stalemate
    return 0;
  }

  int best;
  if (maximizingPlayer) {
    best = INT_MIN;
  } else {
    best = INT_MAX;
  }

  int best_index = 0;

  for (int i = 0; i < moves->curr_length; i++) {
    Move *m = (Move *)get_at(moves, i);
    pseudo_do_move(board, m);

    if (check_check(board, false)) {
      undo_move(board, m);
      continue;
    }

    bool old_check_status = board->check_move;
    Castle old_castling = board->castling;
    Enpassent old_enpassent = board->enpassant;
    uint16_t old_halfmoves = board->halfmoves;
    uint16_t old_fullmoves = board->fullmoves;
    bool old_checkmate_status = board->checkmate;

    if ((m->start_piece == WhitePawn || m->start_piece == BlackPawn) &&
        (board->enpassant.valid)) {
      if (board->enpassant.row == m->end.row &&
          board->enpassant.col == m->end.col) {
        if (m->start_piece == WhitePawn) {
          set_at(board->board, (Pieces[]){Blank},
                 (m->end.row + 1) * BOARD_ROW + m->end.col);
        } else {
          set_at(board->board, (Pieces[]){Blank},
                 (m->end.row - 1) * BOARD_ROW + m->end.col);
        }
      }
    }

    if ((m->start_piece == WhitePawn || m->start_piece == BlackPawn) &&
        (abs(m->start.row - m->end.row) == 2)) {
      if (m->end.row < m->start.row) {
        board->enpassant.row = m->end.row + 1;
        board->enpassant.col = m->end.col;
        board->enpassant.valid = true;
      } else {
        board->enpassant.row = m->end.row - 1;
        board->enpassant.col = m->end.col;
        board->enpassant.valid = true;
      }
    } else {
      board->enpassant.valid = false;
    }

    if (m->start_piece == WhitePawn || m->start_piece == BlackPawn) {
      board->halfmoves = 0;
    } else if (get_piece_color(board, m->end.row, m->end.col) == White ||
               get_piece_color(board, m->end.row, m->end.col) == Black) {
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

    int v = 0;
    if (board->checkmate) {
      v = maximizingPlayer ? INT_MAX - MAX_DEPTH + depth
                           : INT_MIN + MAX_DEPTH - depth;
    } else {
      v = alpha_beta(board, !maximizingPlayer, get_opponent(player), alpha,
                     beta, depth + 1, move_todo);
    }

    board->player = player;
    board->check_move = old_check_status;
    board->castling = old_castling;
    board->enpassant = old_enpassent;
    board->halfmoves = old_halfmoves;
    board->fullmoves = old_fullmoves;
    board->checkmate = old_checkmate_status;

    undo_move(board, m);
    if (maximizingPlayer) {
      if (v > best) {
        best = v;
        best_index = i;
      }
      alpha = max(alpha, v);
    } else {
      if (v < best) {
        best = v;
        best_index = i;
      }
      beta = min(beta, v);
    }
    if (alpha >= beta) {
      break;
    }
  }

  if (depth == 0) {
    memcpy(move_todo, get_at(moves, best_index), sizeof(Move));
  }

  deinit_array(moves);
  return best;
}
