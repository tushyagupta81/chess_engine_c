#include "board.h"
#include "memory.h"
#include "moves.h"
#include <stdio.h>
#include <string.h>

int main() {
  Board *board =
      new_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
  print_board(board);
  print_board_info(board);
  char input[5 + 1];
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
    do_move(board, input);
    print_board(board);
    print_board_info(board);
  }
  deinit_board(board);
  return 0;
}
