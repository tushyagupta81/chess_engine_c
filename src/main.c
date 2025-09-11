#include "board.h"
#include "memory.h"
#include "move_gen.h"
#include "moves.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  Board *board =
      new_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
  // new_board("6k1/5ppp/8/8/8/8/8/R6K w KQkq - 0 15");
  print_board(board);
  print_board_info(board);
  char input[5 + 1];
  Move *move = malloc(sizeof(Move));
  while (1) {
    printf("> ");
    fflush(stdout);

    if (!fgets(input, sizeof(input), stdin)) {
      break;
    }
    input[strcspn(input, "\n")] = '\0';

    if (strlen(input) == sizeof(input) - 1) {
      flush_stdin();
    }

    if (strcmp(input, "exit") == 0) {
      break;
    }

    printf("\n");
    if (!do_move(board, input)) {
      continue;
    }
    print_board(board);
    print_board_info(board);
    if (board->checkmate == true) {
      printf("\n=== Game over ===\n");
      if (board->player == White) {
        printf("Black wins\n");
      } else {
        printf("White wins\n");
      }
      break;
    }
    alpha_beta(board, true, board->player, INT_MIN, INT_MAX, 0, move);
    do_encoded_move(board, move);
    print_board(board);
    print_board_info(board);
  }
  deinit_board(board);
  return 0;
}
