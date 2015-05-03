
#include "defs.h"

/**
 * Chess Engine validation function that validates that the provided
 * square is a valid square on the chess board.
 * @param sq The chess board square to validate.
 * @return Boolean value indicating the validity of the square.
 */
int ce_valid_square(const int sq) {
  return tbl_files_board[sq] == OFFBOARD ? FALSE : TRUE;
}

/**
 * Chess Engine validation function that validates the provided side
 * value is either of WHITE or BLACK.
 * @param side The side value to validate.
 * @param Boolean value indicating the validity of the side value.
 */
int ce_valid_side(const int side) {
  return (side == WHITE || side == BLACK) ? TRUE: FALSE;
}

/**
 * Chess Engine validation function that validates the provided
 * file rank on the board.
 * @param fr The file rank of the board to validte.
 * @return Boolean value indicating the validity of the file rank value.
 */
int ce_valid_file_rank(const int fr) {
  return (fr >= FILE_A && fr <= FILE_H) ? TRUE : FALSE;
}

/**
 * Chess Engine validation function that validates the provided
 * piece including the empty/no piece board representation value.
 * @param pce The numerical value of the piece to validate.
 * @return Boolean value indicating the validity of the piece value.
 */
int ce_valid_piece_empty(const int pce) {
  return (pce >= EMPTY && pce <= bK) ? TRUE : FALSE;
}

/**
 * Chess Engine validation function that validates the provided
 * piece value.
 * @param pce The numerical value of the piece to validate.
 * @return Boolean value indicating the validity of the piece value.
 */
int ce_valid_piece(const int pce) {
  return (pce >= wP && pce <= bK) ? TRUE : FALSE;
}

