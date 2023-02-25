
#include "defs.h"

/**
 * Chess Engine validation function that validates that the provided
 * square is a valid square on the chess board.
 * @param sq The chess board square to validate.
 * @return Boolean value indicating the validity of the square.
 */
bool ce_valid_square(const int32_t sq) {
  return tbl_files_board[sq] == OFFBOARD ? false : true;
}

/**
 * Chess Engine validation function that validates the provided side
 * value is either of WHITE or BLACK.
 * @param side The side value to validate.
 * @param Boolean value indicating the validity of the side value.
 */
bool ce_valid_side(const int32_t side) {
  return (side == WHITE || side == BLACK) ? true: false;
}

/**
 * Chess Engine validation function that validates the provided
 * file rank on the board.
 * @param fr The file rank of the board to validte.
 * @return Boolean value indicating the validity of the file rank value.
 */
bool ce_valid_file_rank(const int32_t fr) {
  return (fr >= FILE_A && fr <= FILE_H) ? true : false;
}

/**
 * Chess Engine validation function that validates the provided
 * piece including the empty/no piece board representation value.
 * @param pce The numerical value of the piece to validate.
 * @return Boolean value indicating the validity of the piece value.
 */
bool ce_valid_piece_empty(const int32_t pce) {
  return (pce >= EMPTY && pce <= bK) ? true : false;
}

/**
 * Chess Engine validation function that validates the provided
 * piece value.
 * @param pce The numerical value of the piece to validate.
 * @return Boolean value indicating the validity of the piece value.
 */
bool ce_valid_piece(const int32_t pce) {
  return (pce >= wP && pce <= bK) ? true : false;
}
