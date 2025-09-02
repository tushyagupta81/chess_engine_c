#include "board.h"
#include "icons.h"
#include "memory.h"
#include "pieces.h"
#include "types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Board *new_board(char *fen) {
  Board *board = (Board *)malloc(sizeof(Board));
  board->board = new_array(ValueArray);
  for (int i = 0; i < 8; i++) {
    Array *row = new_array(ValuePiece);
    for (int j = 0; j < 8; j++) {
      append(row, (Pieces[]){Blank});
    }
    append(board->board, &row);
  }

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

  board->castling = (char *)malloc(sizeof(char) * 4);
  memcpy(board->castling, fen + start, l - start);

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

  board->enpassant = (char *)malloc(sizeof(char) * 2);
  memcpy(board->enpassant, fen + start, l - start);

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

  char *temp = malloc(sizeof(char) * (l - start));
  memcpy(temp, fen + start, l - start);
  board->halfmoves = atoi(temp);
  free(temp);

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

  return board;
}

void print_board(Board *board) {
  for (int i = 0; i < 8; i++) {
    // Array *row = get_at(board->board, i);
    for (int j = 0; j < 8; j++) {
      Pieces piece = *(Pieces *)get_at_2d(board->board, i, j);

      // Choose background (chessboard pattern)
      if ((i + j) % 2 == 0)
        printf("\x1b[48;5;49m"); // light square
      else
        printf("\x1b[48;5;130m"); // dark square

      // Print piece symbol
      printf("%s ", icons[piece]);

      // Reset colors
      printf("\x1b[0m");
    }
    printf("\n");
  }
}

void print_board_info(Board *board) {
  printf("%-10s - '%s'\n", "castling", board->castling);
  printf("%-10s - %b\n", "check_move", board->check_move);
  printf("%-10s - %d\n", "fullmoves", board->fullmoves);
  printf("%-10s - %d\n", "halfmoves", board->halfmoves);
  printf("%-10s - '%s'\n", "enpassant", board->enpassant);
  if (board->player == White) {
    printf("%-10s - 'White'\n", "player");
  } else {
    printf("%-10s - 'Black'\n", "player");
  }
}
