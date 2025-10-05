#include "board.h"
#include "moves.h"
#include "pieces.h"
#include "tests.h"
#include "unity.h"

void test_moves_notation(void) {
  Board *board =
      new_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  TEST_ASSERT_EQUAL(1, board->fullmoves);
  do_move(board, "a2a3");
  do_move(board, "h7h6");
  do_move(board, "a3a4");
  do_move(board, "h6h5");
  do_move(board, "a4a5");
  do_move(board, "h5h4");
  do_move(board, "a1a4");
  TEST_ASSERT_EQUAL_MESSAGE(WhitePawn, GET_PIECE(3, 0), "Expecte WhitePawn a5");
  TEST_ASSERT_EQUAL_MESSAGE(WhiteRook, GET_PIECE(4, 0),
                            "Expected WhiteRook a4");
  TEST_ASSERT_EQUAL(4, board->fullmoves);
  TEST_ASSERT_EQUAL(1, board->halfmoves);
  deinit_board(board);
}

void test_enpassent(void) {
  Board *board =
      new_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  TEST_ASSERT_EQUAL(1, board->fullmoves);
  do_move(board, "a2a4");
  TEST_ASSERT_EQUAL(board->enpassant.valid, true);
  TEST_ASSERT_EQUAL(5, board->enpassant.row);
  TEST_ASSERT_EQUAL(0, board->enpassant.col);
  do_move(board, "a7a5");
  TEST_ASSERT_EQUAL(board->enpassant.valid, true);
  TEST_ASSERT_EQUAL(2, board->enpassant.row);
  TEST_ASSERT_EQUAL(0, board->enpassant.col);
  do_move(board, "b2b4");
  TEST_ASSERT_EQUAL(board->enpassant.valid, true);
  TEST_ASSERT_EQUAL(5, board->enpassant.row);
  TEST_ASSERT_EQUAL(1, board->enpassant.col);
  do_move(board, "b7b6");
  TEST_ASSERT_EQUAL(board->enpassant.valid, false);
  do_move(board, "b4b5");
  TEST_ASSERT_EQUAL(board->enpassant.valid, false);
  do_move(board, "c7c5");
  TEST_ASSERT_EQUAL(board->enpassant.valid, true);
  do_move(board, "b5c6");
  TEST_ASSERT_EQUAL(board->enpassant.valid, false);
  TEST_ASSERT_EQUAL(Blank, *(Pieces *)get_at(board->board, 3 * BOARD_COL + 1));
  deinit_board(board);
}

void test_check(void) {
  Board *board = new_board("4k3/8/8/8/8/8/8/4R3 b KQkq - 0 1");
  TEST_ASSERT_EQUAL(true, board->check_move);
  deinit_board(board);
  board = new_board("k7/8/8/8/8/8/8/7B b KQkq - 0 1");
  TEST_ASSERT_EQUAL(true, board->check_move);
  deinit_board(board);
  board = new_board("k6R/8/8/8/8/8/8/8 b KQkq - 0 1");
  TEST_ASSERT_EQUAL(true, board->check_move);
  deinit_board(board);
  board = new_board("k7/8/8/8/8/8/8/7Q b KQkq - 0 1");
  TEST_ASSERT_EQUAL(true, board->check_move);
  deinit_board(board);
  board = new_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  TEST_ASSERT_EQUAL(false, board->check_move);
  deinit_board(board);
}
