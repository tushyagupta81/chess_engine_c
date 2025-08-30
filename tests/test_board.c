#include "board.h"
#include "memory.h"
#include "pieces.h"
#include "tests.h"
#include "unity.h"
#include <stdio.h>

void test_board(void) {
  Board *board = new_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
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
