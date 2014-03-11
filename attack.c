
#include "defs.h"

// piece move distances in board array
const int knDir[8] = { -8, -19, -21, -12,  8,  19, 21, 12 };
const int rkDir[4] = { -1, -10,   1,  10 };
const int biDir[4] = { -9,  11,  11,   9 };
const int kiDir[8] = { -1, -10,   1,  10, -9, -11, 11, 9 };

int ce_is_square_attacked(const int sq, const int side, const struct board_s *pos) {
  int pce, index, t_sq, dir;

  // determine attacks from pawns
  if (side == WHITE) {
    if (pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP) {
      return TRUE;
    }
  } else {
    if (pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP) {
      return TRUE;
    }
  }

  return FALSE;
}

