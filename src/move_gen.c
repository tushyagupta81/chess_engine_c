#include "move_gen.h"
#include "board.h"
#include "memory.h"
#include "moves.h"
#include "pieces.h"
#include "types.h"
#include <limits.h>
#include <stdbool.h>
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
  if (depth >= MAX_DEPTH) {
    return get_board_score(board, player);
  }

  Array *moves = new_array(ValueMoves);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      Array *positions = new_array(ValuePosition);
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
      deinit_array(positions);
    }
  }

  int i;
  if (maximizingPlayer) {
    int best = INT_MIN;

    for (i = 0; i < moves->curr_length; i++) {
      Move *m = (Move *)get_at(moves, i);
      pseudo_do_move(board, m);
      bool self_check;
      if (board->player == White) {
        self_check = get_check_status(board, &board->whiteKing);
      } else {
        self_check = get_check_status(board, &board->blackKing);
      }
      if (self_check) {
        undo_move(board, m);
        continue;
      }
      board->player = get_opponent(player);

      int v = alpha_beta(board, false, get_opponent(player), alpha, beta,
                         depth + 1, move_todo);

      board->player = player;
      undo_move(board, m);
      best = max(best, v);
      alpha = max(alpha, v);
      if (alpha >= beta) {
        break;
      }
    }
    if (depth == 0) {
      if (i == moves->curr_length) {
        i--;
      }
      memcpy(move_todo, get_at(moves, i), sizeof(Move));
    }
    deinit_array(moves);
    return best;
  } else {
    int best = INT_MAX;
    for (i = 0; i < moves->curr_length; i++) {
      Move *m = (Move *)get_at(moves, i);
      pseudo_do_move(board, m);
      bool self_check;
      if (board->player == White) {
        self_check = get_check_status(board, &board->whiteKing);
      } else {
        self_check = get_check_status(board, &board->blackKing);
      }
      if (self_check) {
        undo_move(board, m);
        continue;
      }

      board->player = get_opponent(player);
      int v = alpha_beta(board, true, get_opponent(player), alpha, beta,
                         depth + 1, move_todo);
      board->player = get_opponent(player);

      undo_move(board, m);
      best = min(best, v);
      beta = min(beta, v);
      if (alpha >= beta) {
        break;
      }
    }
    if (depth == 0) {
      if (i == moves->curr_length) {
        i--;
      }
      memcpy(move_todo, get_at(moves, i), sizeof(Move));
    }
    deinit_array(moves);
    return best;
  }
}
