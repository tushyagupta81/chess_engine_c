#include "board.h"
#include "move_gen.h"
#include "moves.h"
#include "tests.h"
#include <limits.h>
#include <stdlib.h>

void test_alpha_beta(void) {
  Board *board =
      new_board("3r4/6pP/k1P1n3/2P5/2pq1NK1/7p/1R3B1N/3B4 w - - 0 1");
  Move *move = malloc(sizeof(Move));
  for (int i = 0; i < 5; i++) {
    // print_board(board);
    // print_board_info(board);
    alpha_beta(board, true, board->player, INT_MIN,
               INT_MAX, 0, move);
    do_encoded_move(board, move);
  }
  free(move);
  // print_board(board);
  deinit_board(board);
}
