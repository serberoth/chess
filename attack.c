
#include "defs.h"

// piece move distances in board array
const int knDir[8] = { -8, -19, -21, -12,  8,  19, 21, 12 };
const int rkDir[4] = { -1, -10,   1,  10 };
const int biDir[4] = { -9, -11,  11,   9 };
const int kiDir[8] = { -1, -10,   1,  10, -9, -11, 11, 9 };

int ce_is_square_attacked(const int sq, const int side, const struct board_s *pos) {
  int pce, index, t_sq, dir;

  // determine attacks from pawns
  if (side == WHITE) {
    if (pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP) {
      return TRUE;
    }
  } else if (side == BLACK) {
    if (pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP) {
      return TRUE;
    }
  }

  // determine attacks for knights
  for (index = 0; index < 8; ++index) {
    pce = pos->pieces[sq + knDir[index]];
    if (IsKn(pce) && tbl_piece_col[pce] == side) {
      return TRUE;
    }
  }

  // determine attacks for rooks and queen
  for (index = 0; index < 4; ++index) {
    dir = rkDir[index];
    t_sq = sq + dir;
    pce = pos->pieces[t_sq];
    while (pce != OFFBOARD) {
      if (pce != EMPTY) {
        if (IsRQ(pce) && tbl_piece_col[pce] == side) {
          return TRUE;
        }
        break;
      }
      t_sq += dir;
      pce = pos->pieces[t_sq];
    }
  }

  // determine attacks for bishops and queen
  for (index = 0; index < 4; ++index) {
    dir = biDir[index];
    t_sq = sq + dir;
    pce = pos->pieces[t_sq];
    while (pce != OFFBOARD) {
      if (pce != EMPTY) {
        if (IsBQ(pce) && tbl_piece_col[pce] == side) {
          return TRUE;
        }
        break;
      }
      t_sq += dir;
      pce = pos->pieces[t_sq];
    }
  }

  // determine attacks for king
  for (index = 0; index < 8; ++index) {
    pce = pos->pieces[sq + kiDir[index]];
    if (IsKi(pce) && tbl_piece_col[pce] == side) {
      return TRUE;
    }
  }

  return FALSE;
}

