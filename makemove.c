
#include "defs.h"

/*
1) make (move)
2) get the from (at), to, cap from the move
3) store the current position in the pos->history array
4) move the current piece from (at) -> to
5) if a capture was made, remove the captured piece from the piece list
6) update the fifty move rule, see if pawn was moved
7) promotions
8) en passent captures
9) set en passent square if move was pawn start
10) for all pieces added, moved, removed, update all position counters, and piece lists
11) maintain position key (hash)
12) update the castle permissions
13) change side, increment ply, and historyPly
 */

#define HASH_PCE(pce, sq)		(pos->positionKey ^= (tbl_piece_keys[(pce)][(sq)]))
#define HASH_CA				(pos->positionKey ^= (tbl_castle_keys[(pos->castlePerms)]))
#define HASH_SIDE			(pos->positionKey ^= (side_key))
#define HASH_EP				(pos->positionKey ^= (tbl_piece_keys[(EMPTY)][(pos->enPassent)]))

const int tbl_castle_perms[120] = {
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
  15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
  15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
  15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

static void _ce_clear_piece(const int sq, struct board_s *pos) {
  int pce, col, index, t_pceNum;

  ASSERT(ce_valid_square(sq));

  pce = pos->pieces[sq];
  col = tbl_piece_col[pce];
  index = 0;
  t_pceNum = -1;

  HASH_PCE(pce, sq);

  // remove the piece from the board
  pos->pieces[sq] = EMPTY;
  pos->material[col] -= tbl_piece_val[pce];

  // remove the piece from the appropriate material list
  if (tbl_piece_big[pce]) {
    pos->bigPieces[col]--;
    if (tbl_piece_maj[pce]) {
      pos->majPieces[col]--;
    } else if (tbl_piece_min[pce]) {
      pos->minPieces[col]--;
    }
  } else {
    CLRBIT(pos->pawns[col], SQ64(sq));
    CLRBIT(pos->pawns[BOTH], SQ64(sq));
  }

  // remove the piece from the piece list
  for (index = 0; index < pos->pieceNum[pce]; ++index) {
    if (pos->pieceList[pce][index] == sq) {
      t_pceNum = index;
      break;
    }
  }

  ASSERT(t_pceNum != -1);

  // remove the piece from the piece list and replace with the next piece
  pos->pieceNum[pce]--;
  pos->pieceList[pce][t_pceNum] = pos->pieceList[pce][pos->pieceNum[pce]];
}

static void _ce_add_piece(const int sq, struct board_s *pos, const int pce) {
  int col;

  ASSERT(ce_valid_piece(pce));
  ASSERT(ce_valid_square(sq));

  col = tbl_piece_col[pce];

  HASH_PCE(pce, sq);

  pos->pieces[sq] = pce;

  if (tbl_piece_big[pce]) {
    pos->bigPieces[col]++;
    if (tbl_piece_maj[pce]) {
      pos->majPieces[col]++;
    } else if (tbl_piece_min[pce]) {
      pos->minPieces[col]++;
    }
  } else {
    SETBIT(pos->pawns[col], SQ64(sq));
    SETBIT(pos->pawns[BOTH], SQ64(sq));
  }

  pos->material[col] += tbl_piece_val[pce];
  pos->pieceList[pce][pos->pieceNum[pce]++] = sq;
}

static void _ce_move_piece(const int at, const int to, struct board_s *pos) {
  int index = 0, pce, col;
#ifdef DEBUG
  int t_pieceNum = FALSE;
#endif

  ASSERT(ce_valid_square(at));
  ASSERT(ce_valid_square(to));

  pce = pos->pieces[at];
  col = tbl_piece_col[pce];

  HASH_PCE(pce, at);
  pos->pieces[at] = EMPTY;

  HASH_PCE(pce, to);
  pos->pieces[to] = pce;

  if (!tbl_piece_big[pce]) {
    CLRBIT(pos->pawns[col], SQ64(at));
    CLRBIT(pos->pawns[BOTH], SQ64(at));
    SETBIT(pos->pawns[col], SQ64(to));
    SETBIT(pos->pawns[BOTH], SQ64(to));
  }

  for (index = 0; index < pos->pieceNum[pce]; ++index) {
    if (pos->pieceList[pce][index] == at) {
      pos->pieceList[pce][index] = to;
#ifdef DEBUG
      t_pieceNum = TRUE;
#endif
      break;
    }
  }

  ASSERT(t_pieceNum);
}

