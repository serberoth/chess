
#include "defs.h"

U64 ce_generate_position_key(const struct board_s *board) {
  U64 finalKey = 0ULL;
  int sq = 0;

  // hash the current pieces on the board into the key
  for (sq = 0; sq <= NUM_BRD_SQ; ++sq) {
    int piece = board->pieces[sq];
    if (piece != NO_SQ && piece != EMPTY) {
      ASSERT(piece >= wP && piece <= bK);
      finalKey ^= tbl_piece_keys[piece][sq];
    }
  }

  // has the current side key
  if (board->side == WHITE) {
    finalKey ^= side_key;
  }

  // hash the en-passent square into the key
  if (board->enPassent != NO_SQ) {
    ASSERT(board->enPassent >= 0 && board->enPassent <= NUM_BRD_SQ);
    finalKey ^= tbl_piece_keys[EMPTY][board->enPassent];
  }

  // hash the castle permissions into the key
  ASSERT(board->castlePerms >= 0 && board->castlePerms <= 15);
  finalKey ^= tbl_castle_keys[board->castlePerms];

  return finalKey;
}

