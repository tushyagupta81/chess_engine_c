#include "board.h"
#include "memory.h"
#include "pieces.h"
#include "tests.h"
#include "types.h"
#include "unity.h"
#include <string.h>

void test_board(void) {
  Board *board =
      new_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
  TEST_ASSERT_EQUAL(strcmp("KQkq", board->castling) == 0, true);
  TEST_ASSERT_EQUAL(false, board->enpassant.valid);
  TEST_ASSERT_EQUAL(White, board->player);
  TEST_ASSERT_EQUAL(0, board->halfmoves);
  TEST_ASSERT_EQUAL(0, board->fullmoves);
  // Pawns
  for (int j = 0; j < 8; j++) {
    TEST_ASSERT_EQUAL(WhitePawn, *(Pieces *)get_at_2d(board->board, 6, j));
    TEST_ASSERT_EQUAL(BlackPawn, *(Pieces *)get_at_2d(board->board, 1, j));
  }

  // White back rank
  TEST_ASSERT_EQUAL(WhiteRook, *(Pieces *)get_at_2d(board->board, 7, 0));
  TEST_ASSERT_EQUAL(WhiteKnight, *(Pieces *)get_at_2d(board->board, 7, 1));
  TEST_ASSERT_EQUAL(WhiteBishop, *(Pieces *)get_at_2d(board->board, 7, 2));
  TEST_ASSERT_EQUAL(WhiteQueen, *(Pieces *)get_at_2d(board->board, 7, 3));
  TEST_ASSERT_EQUAL(WhiteKing, *(Pieces *)get_at_2d(board->board, 7, 4));
  TEST_ASSERT_EQUAL(WhiteBishop, *(Pieces *)get_at_2d(board->board, 7, 5));
  TEST_ASSERT_EQUAL(WhiteKnight, *(Pieces *)get_at_2d(board->board, 7, 6));
  TEST_ASSERT_EQUAL(WhiteRook, *(Pieces *)get_at_2d(board->board, 7, 7));

  // Black back rank
  TEST_ASSERT_EQUAL(BlackRook, *(Pieces *)get_at_2d(board->board, 0, 0));
  TEST_ASSERT_EQUAL(BlackKnight, *(Pieces *)get_at_2d(board->board, 0, 1));
  TEST_ASSERT_EQUAL(BlackBishop, *(Pieces *)get_at_2d(board->board, 0, 2));
  TEST_ASSERT_EQUAL(BlackQueen, *(Pieces *)get_at_2d(board->board, 0, 3));
  TEST_ASSERT_EQUAL(BlackKing, *(Pieces *)get_at_2d(board->board, 0, 4));
  TEST_ASSERT_EQUAL(BlackBishop, *(Pieces *)get_at_2d(board->board, 0, 5));
  TEST_ASSERT_EQUAL(BlackKnight, *(Pieces *)get_at_2d(board->board, 0, 6));
  TEST_ASSERT_EQUAL(BlackRook, *(Pieces *)get_at_2d(board->board, 0, 7));

  for (int i = 3; i < 6; i++) {
    for (int j = 0; j < 8; j++) {
      TEST_ASSERT_EQUAL(Blank, *(Pieces *)get_at_2d(board->board, i, j));
    }
  }
}

void test_moves(void) {
  Board *board = new_board("8/8/8/3r2r1/8/8/8/8 w KQkq - 0 0");
  Array *moves = new_array(ValuePosition);
  get_moves(moves, board, 3, 3);
  TEST_ASSERT_EQUAL(moves->curr_length, 12);
  board = new_board("8/8/8/3q4/8/8/8/8 w KQkq - 0 0");
  moves = new_array(ValuePosition);
  get_moves(moves, board, 3, 3);
  TEST_ASSERT_EQUAL(moves->curr_length, 27);
}
