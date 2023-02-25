
#include "defs.h"

/**
 * Chess Engine function to generate the position keys from the
 * provided board position structure.  This function generates a
 * uint64_t hash value containing the current board position from the
 * provided board position struct.
 * @param board A board position structure pointer used to generate
 *   the position keys.
 * @return The board position keys for the provided board.
 */
uint64_t ce_generate_position_key(const struct board_s *board) {
  uint64_t finalKey = 0ULL;
  int32_t sq = 0;

  // Hash the current pieces on the board into the key
  for (sq = 0; sq < NUM_BRD_SQ; ++sq) {
    int32_t piece = board->pieces[sq];
    if (piece != NO_SQ && piece != EMPTY && piece != OFFBOARD) {
      ASSERT(piece >= wP && piece <= bK);
      finalKey ^= tbl_piece_keys[piece][sq];
    }
  }

  // Hash the current side key
  if (board->side == WHITE) {
    finalKey ^= side_key;
  }

  // Hash the en-passent square into the key
  if (board->enPassent != NO_SQ) {
    ASSERT(board->enPassent >= 0 && board->enPassent <= NUM_BRD_SQ);
    finalKey ^= tbl_piece_keys[EMPTY][board->enPassent];
  }

  // Hash the castle permissions into the key
  ASSERT(board->castlePerms >= 0 && board->castlePerms <= 15);
  finalKey ^= tbl_castle_keys[board->castlePerms];

  return finalKey;
}
