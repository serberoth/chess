
#include "defs.h"

int ce_valid_square(const int sq) {
  return tbl_files_board[sq] == OFFBOARD ? FALSE : TRUE;
}

int ce_valid_side(const int side) {
  return (side == WHITE || side == BLACK) ? TRUE: FALSE;
}

int ce_valid_file_rank(const int fr) {
  return (fr >= FILE_A && fr <= FILE_H) ? TRUE : FALSE;
}

int ce_valid_piece_empty(const int pce) {
  return (pce >= EMPTY && pce <= bK) ? TRUE : FALSE;
}

int ce_valid_piece(const int pce) {
  return (pce >= wP && pce <= bK) ? TRUE : FALSE;
}

