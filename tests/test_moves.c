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

void test_enpassent(void) {
  Board *board =
      new_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  TEST_ASSERT_EQUAL(1, board->fullmoves);
  do_move(board, "b1d1");
  TEST_ASSERT_EQUAL(board->enpassant.valid, true);
  TEST_ASSERT_EQUAL(5, board->enpassant.row);
  TEST_ASSERT_EQUAL(0, board->enpassant.col);
  do_move(board, "g1e1");
  TEST_ASSERT_EQUAL(board->enpassant.valid, true);
  TEST_ASSERT_EQUAL(2, board->enpassant.row);
  TEST_ASSERT_EQUAL(0, board->enpassant.col);
  do_move(board, "b2d2");
  TEST_ASSERT_EQUAL(board->enpassant.valid, true);
  TEST_ASSERT_EQUAL(5, board->enpassant.row);
  TEST_ASSERT_EQUAL(1, board->enpassant.col);
  do_move(board, "g2f2");
  TEST_ASSERT_EQUAL(board->enpassant.valid, false);
  do_move(board, "d2e2");
  TEST_ASSERT_EQUAL(board->enpassant.valid, false);
  do_move(board, "g3e3");
  TEST_ASSERT_EQUAL(board->enpassant.valid, true);
  do_move(board, "e2f3");
  TEST_ASSERT_EQUAL(board->enpassant.valid, false);
  TEST_ASSERT_EQUAL(Blank, *(Pieces *)get_at_2d(board->board, 3, 2));
}
