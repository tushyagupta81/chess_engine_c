#include "board.h"
#include "icons.h"
#include "memory.h"
#include "moves.h"
#include "pieces.h"
#include "types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool get_check_status(Board *board, Position *king) {
  Array *moves = new_array(ValuePosition);
  get_check_moves(moves, board, king->row, king->col);

  for (int i = 0; i < moves->curr_length; i++) {
    Position *pos = get_at(moves, i);
    Array *piece_moves = new_array(ValuePosition);
    get_moves(piece_moves, board, pos->row, pos->col);
    for (int j = 0; j < piece_moves->curr_length; j++) {
      if (cmp_pos(king, get_at(piece_moves, j))) {
        // NOTE: You have to deinit if u early quit
        deinit_array(piece_moves);
        deinit_array(moves);
        return true;
      }
    }
    deinit_array(piece_moves);
  }
  deinit_array(moves);
  return false;
}

void deinit_board(Board *board) {
  for (int i = 0; i < 8; i++) {
    Array *row = *(Array **)get_at(board->board, i);
    deinit_array(row); // free row->values
  }
  deinit_array(board->board); // free outer array->values
  free(board->castling);
  free(board);
}

Board *new_board(char *fen) {
  Board *board = malloc(sizeof(Board));
  if (board == NULL) {
    fprintf(stderr, "Failed to allocated memory to board\n");
    exit(1);
  }
  board->board = new_array(ValueArray);
  for (int i = 0; i < 8; i++) {
    Array *row = new_array(ValuePiece);
    for (int j = 0; j < 8; j++) {
      append(row, (Pieces[]){Blank});
    }
    append(board->board, &row);
  }

  bool whiteKingPresent = false, blackKingPresent = false;
  // Basic board Pieces
  int i = 0;
  int l = 0;
  int j = 0;
  while (fen[l] != ' ' && fen[l] != '\0') {
    switch (fen[l]) {
    case 'P':
      set_at_2d(board->board, (Pieces[]){WhitePawn}, i, j);
      break;
    case 'B':
      set_at_2d(board->board, (Pieces[]){WhiteBishop}, i, j);
      break;
    case 'N':
      set_at_2d(board->board, (Pieces[]){WhiteKnight}, i, j);
      break;
    case 'R':
      set_at_2d(board->board, (Pieces[]){WhiteRook}, i, j);
      break;
    case 'Q':
      set_at_2d(board->board, (Pieces[]){WhiteQueen}, i, j);
      break;
    case 'K':
      whiteKingPresent = true;
      board->whiteKing.row = i;
      board->whiteKing.col = j;
      set_at_2d(board->board, (Pieces[]){WhiteKing}, i, j);
      break;
    case 'p':
      set_at_2d(board->board, (Pieces[]){BlackPawn}, i, j);
      break;
    case 'b':
      set_at_2d(board->board, (Pieces[]){BlackBishop}, i, j);
      break;
    case 'n':
      set_at_2d(board->board, (Pieces[]){BlackKnight}, i, j);
      break;
    case 'r':
      set_at_2d(board->board, (Pieces[]){BlackRook}, i, j);
      break;
    case 'q':
      set_at_2d(board->board, (Pieces[]){BlackQueen}, i, j);
      break;
    case 'k':
      blackKingPresent = true;
      board->blackKing.row = i;
      board->blackKing.col = j;
      set_at_2d(board->board, (Pieces[]){BlackKing}, i, j);
      break;
    case '1' ... '8':
      for (int k = 0; k < fen[i] - l; k++) {
        set_at_2d(board->board, (Pieces[]){Blank}, i, j + k);
      }
      j += fen[l] - '1';
      break;
    case '/':
      i += 1;
      j = -1;
      break;
    default:
      fprintf(stderr, "Unknow symbol in fen string");
      exit(1);
    }
    j += 1;
    l += 1;
  }

  if (fen[l] == '\0') {
    fprintf(stderr, "Invalid FEN string. No player input\n");
    exit(1);
  }

  // Next move player
  board->player = fen[++l] == 'b' ? Black : White;

  // Castling rights
  l += 2;
  int start = l;
  while (fen[l] != ' ' && fen[l] != '\0') {
    l++;
  }
  if (fen[l] == '\0') {
    fprintf(stderr, "Invalid FEN string. No castling information\n");
    exit(1);
  }

  board->castling = malloc(sizeof(char) * 5);
  if (board->castling == NULL) {
    fprintf(stderr, "Failed to allocated memory to board->castling\n");
    exit(1);
  }
  memcpy(board->castling, fen + start, l - start);
  board->castling[l - start] = '\0';

  // En passant information
  l += 1;
  start = l;
  while (fen[l] != ' ' && fen[l] != '\0') {
    l++;
  }
  if (fen[l] == '\0') {
    fprintf(stderr, "Invalid FEN string. No enpassent information\n");
    exit(1);
  }

  char *enpassent_string = malloc(sizeof(char) * 4);
  if (enpassent_string == NULL) {
    fprintf(stderr, "Failed to allocated memory to enpassent_string\n");
    exit(1);
  }
  memcpy(enpassent_string, fen + start, l - start);
  if (l - start > 1) {
    board->enpassant.row = 7 - (enpassent_string[0] - 'a');
    board->enpassant.col = enpassent_string[1] - '1';
    board->enpassant.valid = true;
  } else {
    board->enpassant.row = 0;
    board->enpassant.col = 0;
    board->enpassant.valid = false;
  }
  free(enpassent_string);

  // Halfmoves
  l += 1;
  start = l;
  while (fen[l] != ' ' && fen[l] != '\0') {
    l++;
  }
  if (fen[l] == '\0') {
    fprintf(stderr, "Invalid FEN string. No halfmoves information\n");
    exit(1);
  }

  char temp[l - start + 1];
  memcpy(temp, fen + start, l - start);
  temp[l - start] = '\0';
  board->halfmoves = atoi(temp);

  // Fullmoves
  l += 1;
  start = l;
  while (fen[l] != '\0') {
    l++;
  }
  if (start == l) {
    fprintf(stderr, "Invalid FEN string. No fullmoves information\n");
    exit(1);
  }

  board->fullmoves = atoi(fen + start);

  if (whiteKingPresent && board->player == White) {
    board->check_move = get_check_status(board, &board->whiteKing);
  } else if (blackKingPresent && board->player == Black) {
    board->check_move = get_check_status(board, &board->blackKing);
  }else{
    board->check_move = false;
  }

  if (board->check_move) {
    board->checkmate = checkmate(board);
  } else {
    board->checkmate = false;
  }

  return board;
}

void print_board(Board *board) {
  printf("  ");
  for (int i = 0; i < 8; i++) {
    printf("%d ", i + 1);
  }
  printf("\n");
  for (int i = 0; i < 8; i++) {
    // Array *row = get_at(board->board, i);
    printf("%c ", 'H' - i);
    for (int j = 0; j < 8; j++) {
      Pieces piece = *(Pieces *)get_at_2d(board->board, i, j);

      // Choose background (chessboard pattern)
      if ((i + j) % 2 == 0)
        printf("\x1b[48;5;36m"); // light square
      else
        printf("\x1b[48;5;94m"); // dark square

      // Print piece symbol
      printf("%s ", icons[piece]);

      // Reset colors
      printf("\x1b[0m");
    }
    printf(" %c", 'H' - i);
    printf("\n");
  }
  printf("  ");
  for (int i = 0; i < 8; i++) {
    printf("%d ", i + 1);
  }
  printf("\n");
}

void print_board_info(Board *board) {
  printf("%-10s - '%s'\n", "castling", board->castling);
  printf("%-10s - %b\n", "check_move", board->check_move);
  printf("%-10s - %b\n", "checkmate", board->checkmate);
  printf("%-10s - %d\n", "fullmoves", board->fullmoves);
  printf("%-10s - %d\n", "halfmoves", board->halfmoves);
  printf("%-10s - %d %d\n", "enpassant", board->enpassant.row,
         board->enpassant.col);
  if (board->player == White) {
    printf("%-10s - 'White'\n", "player");
  } else {
    printf("%-10s - 'Black'\n", "player");
  }
}

bool cmp_pos(Position *a, Position *b) {
  if (a->row == b->row && a->col == b->col) {
    return true;
  }
  return false;
}
