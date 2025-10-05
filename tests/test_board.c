#include "board.h"
#include "memory.h"
#include "pieces.h"
#include "tests.h"
#include "types.h"
#include "unity.h"

void test_board(void) {
  Board *board =
      new_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
  TEST_ASSERT_EQUAL(true, board->castling.BlackKingSide);
  TEST_ASSERT_EQUAL(true, board->castling.BlackQueenSide);
  TEST_ASSERT_EQUAL(true, board->castling.WhiteKingSide);
  TEST_ASSERT_EQUAL(true, board->castling.WhiteQueenSide);
  TEST_ASSERT_EQUAL(false, board->enpassant.valid);
  TEST_ASSERT_EQUAL(White, board->player);
  TEST_ASSERT_EQUAL(0, board->halfmoves);
  TEST_ASSERT_EQUAL(0, board->fullmoves);
  // Pawns
  for (int j = 0; j < 8; j++) {
    TEST_ASSERT_EQUAL(WhitePawn,
                      *(Pieces *)get_at(board->board, 6 * BOARD_COL + j));
    TEST_ASSERT_EQUAL(BlackPawn,
                      *(Pieces *)get_at(board->board, 1 * BOARD_COL + j));
  }

  // White back rank
  TEST_ASSERT_EQUAL(WhiteRook,
                    *(Pieces *)get_at(board->board, 7 * BOARD_COL + 0));
  TEST_ASSERT_EQUAL(WhiteKnight,
                    *(Pieces *)get_at(board->board, 7 * BOARD_COL + 1));
  TEST_ASSERT_EQUAL(WhiteBishop,
                    *(Pieces *)get_at(board->board, 7 * BOARD_COL + 2));
  TEST_ASSERT_EQUAL(WhiteQueen,
                    *(Pieces *)get_at(board->board, 7 * BOARD_COL + 3));
  TEST_ASSERT_EQUAL(WhiteKing,
                    *(Pieces *)get_at(board->board, 7 * BOARD_COL + 4));
  TEST_ASSERT_EQUAL(WhiteBishop,
                    *(Pieces *)get_at(board->board, 7 * BOARD_COL + 5));
  TEST_ASSERT_EQUAL(WhiteKnight,
                    *(Pieces *)get_at(board->board, 7 * BOARD_COL + 6));
  TEST_ASSERT_EQUAL(WhiteRook,
                    *(Pieces *)get_at(board->board, 7 * BOARD_COL + 7));

  // Black back rank
  TEST_ASSERT_EQUAL(BlackRook,
                    *(Pieces *)get_at(board->board, 0 * BOARD_COL + 0));
  TEST_ASSERT_EQUAL(BlackKnight,
                    *(Pieces *)get_at(board->board, 0 * BOARD_COL + 1));
  TEST_ASSERT_EQUAL(BlackBishop,
                    *(Pieces *)get_at(board->board, 0 * BOARD_COL + 2));
  TEST_ASSERT_EQUAL(BlackQueen,
                    *(Pieces *)get_at(board->board, 0 * BOARD_COL + 3));
  TEST_ASSERT_EQUAL(BlackKing,
                    *(Pieces *)get_at(board->board, 0 * BOARD_COL + 4));
  TEST_ASSERT_EQUAL(BlackBishop,
                    *(Pieces *)get_at(board->board, 0 * BOARD_COL + 5));
  TEST_ASSERT_EQUAL(BlackKnight,
                    *(Pieces *)get_at(board->board, 0 * BOARD_COL + 6));
  TEST_ASSERT_EQUAL(BlackRook,
                    *(Pieces *)get_at(board->board, 0 * BOARD_COL + 7));

  for (int i = 3; i < 6; i++) {
    for (int j = 0; j < 8; j++) {
      TEST_ASSERT_EQUAL(Blank,
                        *(Pieces *)get_at(board->board, i * BOARD_COL + j));
    }
  }
  deinit_board(board);
}

void test_moves(void) {
  Board *board = new_board("8/8/8/3r2r1/8/8/8/8 w KQkq - 0 0");
  Array *moves = new_array(ValuePosition);
  get_moves(moves, board, 3, 3);
  TEST_ASSERT_EQUAL(moves->curr_length, 12);
  deinit_array(moves);
  deinit_board(board);
  board = new_board("8/8/8/3q4/8/8/8/8 w KQkq - 0 0");
  moves = new_array(ValuePosition);
  get_moves(moves, board, 3, 3);
  TEST_ASSERT_EQUAL(moves->curr_length, 27);
  deinit_array(moves);
  deinit_board(board);
}
