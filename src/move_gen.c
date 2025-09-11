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

  int i;
  if (maximizingPlayer) {
    int best = INT_MIN;
    int best_index = 0;

    for (i = 0; i < moves->curr_length; i++) {
      Move *m = (Move *)get_at(moves, i);
      pseudo_do_move(board, m);

      if (check_check(board, false)) {
        undo_move(board, m);
        continue;
      }

      bool old_check_status = board->check_move;
      bool old_checkmate_status = board->checkmate;

      if (board->player == White) {
        board->check_move = get_check_status(board, &board->blackKing);
      } else if (board->player == Black) {
        board->check_move = get_check_status(board, &board->whiteKing);
      }

      board->player = get_opponent(player);

      if (board->check_move == true) {
        board->checkmate = checkmate(board);
      }

      if (board->checkmate) {
        board->player = player;
        board->check_move = old_check_status;
        board->checkmate = old_checkmate_status;
        undo_move(board, m);
        return 1e5;
      }

      int v = alpha_beta(board, false, get_opponent(player), alpha, beta,
                         depth + 1, move_todo);

      board->player = player;
      board->check_move = old_check_status;
      board->checkmate = old_checkmate_status;
      undo_move(board, m);
      if (v > best) {
        best = v;
        best_index = i;
      }
      alpha = max(alpha, v);
      if (alpha >= beta) {
        break;
      }
    }
    if (depth == 0) {
      memcpy(move_todo, get_at(moves, best_index), sizeof(Move));
    }
    deinit_array(moves);
    return best;
  } else {
    int best = INT_MAX;
    int best_index = 0;
    for (i = 0; i < moves->curr_length; i++) {
      Move *m = (Move *)get_at(moves, i);
      pseudo_do_move(board, m);

      if (check_check(board, false)) {
        undo_move(board, m);
        continue;
      }

      bool old_check_status = board->check_move;
      bool old_checkmate_status = board->checkmate;

      if (board->player == White) {
        board->check_move = get_check_status(board, &board->blackKing);
      } else if (board->player == Black) {
        board->check_move = get_check_status(board, &board->whiteKing);
      }

      board->player = get_opponent(player);

      if (board->check_move == true) {
        board->checkmate = checkmate(board);
      }

      if (board->checkmate) {
        board->player = player;
        board->check_move = old_check_status;
        board->checkmate = old_checkmate_status;
        undo_move(board, m);
        return 0;
      }

      int v = alpha_beta(board, true, get_opponent(player), alpha, beta,
                         depth + 1, move_todo);

      board->player = player;
      board->check_move = old_check_status;
      board->checkmate = old_checkmate_status;

      undo_move(board, m);
      if (v < best) {
        best = v;
        best_index = i;
      }
      beta = min(beta, v);
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
}
