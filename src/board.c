#include "board.h"
#include "icons.h"
#include "memory.h"
#include "moves.h"
#include "pieces.h"
#include "types.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool get_check_status(Board *board, Position *king) {
  Array *moves = new_array(ValuePosition);
  get_check_moves(moves, board, king->row, king->col);

  Array *piece_moves = new_array(ValuePosition);
  for (int i = 0; i < moves->curr_length; i++) {
    Position *pos = get_at(moves, i);
    piece_moves->curr_length = 0;
    get_moves(piece_moves, board, pos->row, pos->col);
    for (int j = 0; j < piece_moves->curr_length; j++) {
      if (cmp_pos(king, get_at(piece_moves, j))) {
        // NOTE: You have to deinit if u early quit
        deinit_array(piece_moves);
        deinit_array(moves);
        return true;
      }
    }
  }
  deinit_array(piece_moves);
  deinit_array(moves);
  return false;
}

bool board_to_fen(Board *board, char *fen) {
  if (!board)
    return false;

  if (!fen) {
    fprintf(stderr, "Failed to allocate memory for FEN string\n");
    exit(1);
  }
  fen[0] = '\0';
  int pos = 0;

  // Board pieces
  for (int i = 0; i < BOARD_ROW; i++) {
    int empty = 0;
    for (int j = 0; j < BOARD_COL; j++) {
      Pieces p = *(Pieces *)get_at(board->board, i * BOARD_ROW + j);
      char c = 0;
      switch (p) {
      case WhitePawn:
        c = 'P';
        break;
      case WhiteKnight:
        c = 'N';
        break;
      case WhiteBishop:
        c = 'B';
        break;
      case WhiteRook:
        c = 'R';
        break;
      case WhiteQueen:
        c = 'Q';
        break;
      case WhiteKing:
        c = 'K';
        break;
      case BlackPawn:
        c = 'p';
        break;
      case BlackKnight:
        c = 'n';
        break;
      case BlackBishop:
        c = 'b';
        break;
      case BlackRook:
        c = 'r';
        break;
      case BlackQueen:
        c = 'q';
        break;
      case BlackKing:
        c = 'k';
        break;
      case Blank:
        empty++;
        continue;
      }
      if (empty > 0) {
        pos += sprintf(fen + pos, "%d", empty);
        empty = 0;
      }
      fen[pos++] = c;
    }
    if (empty > 0) {
      pos += sprintf(fen + pos, "%d", empty);
    }
    if (i != BOARD_ROW - 1) {
      fen[pos++] = '/';
    }
  }

  // Current player
  fen[pos++] = ' ';
  fen[pos++] = (board->player == White) ? 'w' : 'b';

  // Castling rights
  fen[pos++] = ' ';
  int castling_added = 0;
  if (board->castling.WhiteKingSide)
    fen[pos++] = 'K', castling_added = 1;
  if (board->castling.WhiteQueenSide)
    fen[pos++] = 'Q', castling_added = 1;
  if (board->castling.BlackKingSide)
    fen[pos++] = 'k', castling_added = 1;
  if (board->castling.BlackQueenSide)
    fen[pos++] = 'q', castling_added = 1;
  if (!castling_added)
    fen[pos++] = '-';

  // En passant
  fen[pos++] = ' ';
  if (board->enpassant.valid) {
    fen[pos++] = 'a' + board->enpassant.col;
    fen[pos++] = '8' - board->enpassant.row;
  } else {
    fen[pos++] = '-';
  }

  // Halfmove clock
  fen[pos++] = ' ';
  pos += sprintf(fen + pos, "%d", board->halfmoves);

  // Fullmove number
  fen[pos++] = ' ';
  pos += sprintf(fen + pos, "%d", board->fullmoves);

  fen[pos] = '\0';
  return true;
}

void deinit_board(Board *board) {
  // for (int i = 0; i < 8; i++) {
  //   Array *row = *(Array **)get_at(board->board, i);
  //   deinit_array(row); // free row->values
  // }
  deinit_array(board->board); // free outer array->values
  // free(board->castling);
  free(board);
}

Board *new_board(const char *fen) {
  Board *board = malloc(sizeof(Board));
  if (board == NULL) {
    fprintf(stderr, "Failed to allocated memory to board\n");
    exit(1);
  }
  board->board = new_array(ValuePiece);
  for (int i = 0; i < BOARD_ROW * BOARD_COL; i++) {
    append(board->board, (Pieces[]){Blank});
  }
  // for (int i = 0; i < 8; i++) {
  //   Array *row = new_array(ValuePiece);
  //   for (int j = 0; j < 8; j++) {
  //     append(row, (Pieces[]){Blank});
  //   }
  //   append(board->board, &row);
  // }

  bool whiteKingPresent = false, blackKingPresent = false;
  // Basic board Pieces
  int i = 0;
  int l = 0;
  int j = 0;
  while (fen[l] != ' ' && fen[l] != '\0') {
    switch (fen[l]) {
    case 'P':
      set_at(board->board, (Pieces[]){WhitePawn}, i * BOARD_ROW + j);
      break;
    case 'B':
      set_at(board->board, (Pieces[]){WhiteBishop}, i * BOARD_ROW + j);
      break;
    case 'N':
      set_at(board->board, (Pieces[]){WhiteKnight}, i * BOARD_ROW + j);
      break;
    case 'R':
      set_at(board->board, (Pieces[]){WhiteRook}, i * BOARD_ROW + j);
      break;
    case 'Q':
      set_at(board->board, (Pieces[]){WhiteQueen}, i * BOARD_ROW + j);
      break;
    case 'K':
      whiteKingPresent = true;
      board->whiteKing.row = i;
      board->whiteKing.col = j;
      set_at(board->board, (Pieces[]){WhiteKing}, i * BOARD_ROW + j);
      break;
    case 'p':
      set_at(board->board, (Pieces[]){BlackPawn}, i * BOARD_ROW + j);
      break;
    case 'b':
      set_at(board->board, (Pieces[]){BlackBishop}, i * BOARD_ROW + j);
      break;
    case 'n':
      set_at(board->board, (Pieces[]){BlackKnight}, i * BOARD_ROW + j);
      break;
    case 'r':
      set_at(board->board, (Pieces[]){BlackRook}, i * BOARD_ROW + j);
      break;
    case 'q':
      set_at(board->board, (Pieces[]){BlackQueen}, i * BOARD_ROW + j);
      break;
    case 'k':
      blackKingPresent = true;
      board->blackKing.row = i;
      board->blackKing.col = j;
      set_at(board->board, (Pieces[]){BlackKing}, i * BOARD_ROW + j);
      break;
    case '1' ... '8':
      for (int k = 0; k < fen[i] - l; k++) {
        set_at(board->board, (Pieces[]){Blank}, i * BOARD_COL + j + k);
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

  board->castling.BlackKingSide = false;
  board->castling.BlackQueenSide = false;
  board->castling.WhiteKingSide = false;
  board->castling.WhiteQueenSide = false;

  for (int i = start; i <= l; i++) {
    if (fen[i] == 'k') {
      board->castling.BlackKingSide = true;
    } else if (fen[i] == 'K') {
      board->castling.WhiteKingSide = true;
    } else if (fen[i] == 'q') {
      board->castling.BlackQueenSide = true;
    } else if (fen[i] == 'Q') {
      board->castling.WhiteQueenSide = true;
    }
  }
  // board->castling = malloc(sizeof(char) * 5);
  // if (board->castling == NULL) {
  //   fprintf(stderr, "Failed to allocated memory to board->castling\n");
  //   exit(1);
  // }
  // memcpy(board->castling, fen + start, l - start);
  // board->castling[l - start] = '\0';

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
    board->enpassant.row = 7 - (enpassent_string[1] - '1');
    board->enpassant.col = enpassent_string[0] - 'a';
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
  } else {
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
    printf("%c ", 'A' + i);
  }
  printf("\n");
  for (int i = 0; i < 8; i++) {
    // Array *row = get_at(board->board, i);
    printf("%d ", 8 - i);
    for (int j = 0; j < 8; j++) {
      Pieces piece = *(Pieces *)get_at(board->board, i * BOARD_COL + j);

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
    printf(" %d", 8 - i);
    printf("\n");
  }
  printf("  ");
  for (int i = 0; i < 8; i++) {
    printf("%c ", 'A' + i);
  }
  printf("\n");
}

void castling_to_string(Board *board, char *out) {
  int pos = 0;
  if (board->castling.WhiteKingSide)
    out[pos++] = 'K';
  if (board->castling.WhiteQueenSide)
    out[pos++] = 'Q';
  if (board->castling.BlackKingSide)
    out[pos++] = 'k';
  if (board->castling.BlackQueenSide)
    out[pos++] = 'q';
  if (pos == 0)
    out[pos++] = '-';
  out[pos] = '\0'; // Null-terminate the string
}

void print_board_info(Board *board) {
  char castling_str[5]; // Max 4 chars + null terminator
  castling_to_string(board, castling_str);
  printf("%-10s - '%s'\n", "castling", castling_str);
  printf("%-10s - %s\n", "check move", board->check_move ? "True" : "False");
  printf("%-10s - %s\n", "checkmate", board->checkmate ? "True" : "False");
  printf("%-10s - %d\n", "fullmoves", board->fullmoves);
  printf("%-10s - %d\n", "halfmoves", board->halfmoves);
  printf("%-10s - %d %d %s\n", "enpassant", board->enpassant.row,
         board->enpassant.col, board->enpassant.valid ? "Valid" : "Invalid");
  if (board->player == White) {
    printf("%-10s - White\n", "player");
  } else {
    printf("%-10s - Black\n", "player");
  }
}

bool cmp_pos(Position *a, Position *b) {
  if (a->row == b->row && a->col == b->col) {
    return true;
  }
  return false;
}
