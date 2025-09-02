#include "board.h"

int main() {
  Board *board =
      new_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
  print_board(board);
  print_board_info(board);
  return 0;
}
