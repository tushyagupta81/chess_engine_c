#ifndef PIECES_H
#define PIECES_H

#include <stdint.h>
typedef enum {
  Blank,
  WhitePawn,
  WhiteRook,
  WhiteBishop,
  WhiteKnight,
  WhiteQueen,
  WhiteKing,
  BlackPawn,
  BlackRook,
  BlackBishop,
  BlackKnight,
  BlackQueen,
  BlackKing,
} Pieces;

typedef enum {
  White,
  Black,
  None,
  OutOfBounds,
} Player;

Player get_opponent(Player player);

#endif // !PIECES_H
