
#include "defs.h"

// piece move distances in board array
const int32_t knDir[8] = { -8, -19, -21, -12,  8,  19, 21, 12 };
const int32_t rkDir[4] = { -1, -10,   1,  10 };
const int32_t biDir[4] = { -9, -11,  11,   9 };
const int32_t kiDir[8] = { -1, -10,   1,  10, -9, -11, 11, 9 };

/**
 * Chess Engine function that determines if the provided square is being attacked by the
 * provided side on the provided board position.
 * @param sq The square to check for attacks.
 * @param side The side for which to check attacks against.
 * @param pos A pointer to the current board position.
 * @return Boolean status indicating if the square is being attacked.
 */
bool ce_is_square_attacked(const int32_t sq, const int32_t side, const struct board_s *pos) {
  int32_t pce, t_sq, dir;

  ASSERT(ce_valid_square(sq));
  ASSERT(ce_valid_side(side));
  CHKBRD(pos);

  // Determine attacks from pawns
  if (side == WHITE) {
    if (pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP) {
      return true;
    }
  } else if (side == BLACK) {
    if (pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP) {
      return true;
    }
  }

  // Determine attacks for knights
  for (size_t index = 0; index < 8; ++index) {
    pce = pos->pieces[sq + knDir[index]];
    if (IsKn(pce) && tbl_piece_col[pce] == side) {
      return true;
    }
  }

  // Determine attacks for rooks and queen
  for (size_t index = 0; index < 4; ++index) {
    dir = rkDir[index];
    t_sq = sq + dir;
    pce = pos->pieces[t_sq];
    while (pce != OFFBOARD) {
      if (pce != EMPTY) {
        if (IsRQ(pce) && tbl_piece_col[pce] == side) {
          return true;
        }
        break;
      }
      t_sq += dir;
      pce = pos->pieces[t_sq];
    }
  }

  // Determine attacks for bishops and queen
  for (size_t index = 0; index < 4; ++index) {
    dir = biDir[index];
    t_sq = sq + dir;
    pce = pos->pieces[t_sq];
    while (pce != OFFBOARD) {
      if (pce != EMPTY) {
        if (IsBQ(pce) && tbl_piece_col[pce] == side) {
          return true;
        }
        break;
      }
      t_sq += dir;
      pce = pos->pieces[t_sq];
    }
  }

  // Determine attacks for king
  for (size_t index = 0; index < 8; ++index) {
    pce = pos->pieces[sq + kiDir[index]];
    if (IsKi(pce) && tbl_piece_col[pce] == side) {
      return true;
    }
  }

  return false;
}
