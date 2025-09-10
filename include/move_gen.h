#ifndef MOVE_GEN_H
#define MOVE_GEN_H

#include "board.h"
#include "moves.h"
#include "pieces.h"

#define MAX_DEPTH 4

int alpha_beta(Board *board, bool maximizingPlayer, Player player, int alpha,
               int beta, int depth, Move *move_todo);

#endif // !MOVE_GEN_H
