
#include "defs.h"

void ce_reset_board(struct board_s *pos) {
  int index;

  for (index = 0; index < NUM_BRD_SQ; ++index) {
    pos->pieces[index] = OFFBOARD;
  }

  for (index = 0; index < 64; ++index) {
    pos->pieces[SQ120(index)] = EMPTY;
  }

  for (index = 0; index < 3; ++index) {
    pos->bigPieces[index] = 0;
    pos->majPieces[index] = 0;
    pos->minPieces[index] = 0;
    pos->pawns[index] = 0;
  }

  for (index = 0; index < 13; ++index) {
    pos->pieceNum[index] = 0;
  }

  pos->kingSq[WHITE] = pos->kingSq[BLACK] = NO_SQ;

  pos->side = BOTH;
  pos->enPassent = NO_SQ;
  pos->fiftyMove = 0;

  pos->ply = 0;
  pos->historyPly = 0;

  pos->castlePerms = 0;

  pos->positionKey = 0ULL;
}

