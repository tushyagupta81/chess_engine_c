#include "board.h"
#include "moves.h"
#include "pieces.h"
#include "tests.h"
#include "unity.h"

void test_moves_notation(void) {
  Board *board =
      new_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  TEST_ASSERT_EQUAL(1, board->fullmoves);
  do_move(board, "b1c1");
  do_move(board, "g8f8");
  do_move(board, "c1d1");
  do_move(board, "f8e8");
  do_move(board, "a1c1");
  TEST_ASSERT_EQUAL(WhitePawn, GET_PIECE(4, 0));
  TEST_ASSERT_EQUAL(WhiteRook, GET_PIECE(5, 0));
  TEST_ASSERT_EQUAL(3, board->fullmoves);
  TEST_ASSERT_EQUAL(1, board->halfmoves);
}
