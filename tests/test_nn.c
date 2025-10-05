#include "board.h"
#include "moves.h"
#include "nn_model.h"
#include "tests.h"
#include "unity.h"

void test_nn(void) {
  Board *board =
      new_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  predict_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  deinit_board(board);
}
