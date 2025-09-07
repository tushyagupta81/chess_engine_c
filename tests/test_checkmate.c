#include "board.h"
#include "moves.h"
#include "tests.h"
#include "unity.h"

void test_checkmate(void){
  Board* board = new_board("rnbqkbnr/pppppppp/8/8/2B5/5Q2/PPPPPPPP/RNBQKBNR w KQkq - 0 3");
  TEST_ASSERT_EQUAL(false, board->checkmate);
  do_move(board, "c6g6");
  TEST_ASSERT_EQUAL(true, board->checkmate);
  deinit_board(board);
  board = new_board("6k1/5ppp/8/8/8/8/8/R6K w KQkq - 0 15");
  TEST_ASSERT_EQUAL(false, board->checkmate);
  do_move(board, "a1h1");
  TEST_ASSERT_EQUAL(true, board->checkmate);
  deinit_board(board);
}
