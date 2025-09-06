#include "board.h"

int main() {
  Board *board =
      new_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
  print_board(board);
  print_board_info(board);
  deinit_board(board);
  return 0;
}
