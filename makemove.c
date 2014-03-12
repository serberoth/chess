
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

void ce_take_move(struct board_s *pos) {
  int move, at, to, captured, promoted;

  CHKBRD(pos);

  // decrament the history ply
  pos->historyPly--;
  pos->ply--;

  // get the move and squares
  move = pos->history[pos->historyPly].move;
  at = FROMSQ(move);
  to = TOSQ(move);

  // assert the validity of the mvoe
  ASSERT(ce_valid_square(at));
  ASSERT(ce_valid_square(to));

  // remove the en passent hash
  if (pos->enPassent != NO_SQ) {
    HASH_EP;
  }
  // clear the castling permissions from the hash
  HASH_CA;

  // reset the castle permissions, fifty move, and en passent square
  pos->castlePerms = pos->history[pos->historyPly].castlePerms;
  pos->fiftyMove = pos->history[pos->historyPly].fiftyMove;
  pos->enPassent = pos->history[pos->historyPly].enPassent;

  // reset the en passent hash
  if (pos->enPassent != NO_SQ) {
    HASH_EP;
  }
  // add the castle permissions to the hash
  HASH_CA;

  // change the current side
  pos->side ^= 1;
  HASH_SIDE;

  // add the en passent captured piece back
  if (move & MFLAGEP) {
    if (pos->side == WHITE) {
      _ce_add_piece(to - 10, pos, bP);
    } else if (pos->side == BLACK) {
      _ce_add_piece(to + 10, pos, wP);
    }
  } else if (move & MFLAGCA) {
    // move the castled piece back (the rook)
    switch (to) {
    case C1: _ce_move_piece(D1, A1, pos); break;
    case C8: _ce_move_piece(D8, A8, pos); break;
    case G1: _ce_move_piece(F1, H1, pos); break;
    case G8: _ce_move_piece(F8, H8, pos); break;
    default: ASSERT(FALSE); break;
    }
  }

  // move the piece back to the previous position
  _ce_move_piece(to, at, pos);

  // reset the king square
  if (IsKi(pos->pieces[at])) {
    pos->kingSq[pos->side] = at;
  }

  // add the captured piece back
  captured = CAPTURED(move);
  if (captured != EMPTY) {
    ASSERT(ce_valid_piece(captured));
    _ce_add_piece(to, pos, captured);
  }

  // remove the promoted piece and add the pawn back
  promoted = PROMOTED(move);
  if (promoted != EMPTY) {
    ASSERT(ce_valid_piece(promoted) && !IsPw(promoted));
    _ce_clear_piece(at, pos);
    _ce_add_piece(at, pos, (tbl_piece_col[promoted] == WHITE ? wP : bP));
  }

  CHKBRD(pos);
}

int ce_make_move(struct board_s *pos, int move) {
  int at, to, side, captured, prPce;

  ASSERT(ce_check_board(pos));

  at = FROMSQ(move);
  to = TOSQ(move);
  side = pos->side;

  ASSERT(ce_valid_square(at));
  ASSERT(ce_valid_square(to));
  ASSERT(ce_valid_side(side));
  ASSERT(ce_valid_piece(pos->pieces[at]));

  // store the position key in the current history
  pos->history[pos->historyPly].positionKey = pos->positionKey;
  // store the current move in the history
  pos->history[pos->historyPly].move = move;
  pos->history[pos->historyPly].fiftyMove = pos->fiftyMove;
  pos->history[pos->historyPly].enPassent = pos->enPassent;
  pos->history[pos->historyPly].castlePerms = pos->castlePerms;

  // clear the en passent captured piece
  if (move & MFLAGEP) {
    if (side == WHITE) {
      _ce_clear_piece(to - 10, pos);
    } else if (side == BLACK) {
      _ce_clear_piece(to + 10, pos);
    }
  } else if (move & MFLAGCA) {
    // move the castle piece (the rook)
    switch (to) {
    case C1: _ce_move_piece(A1, D1, pos); break;
    case C8: _ce_move_piece(A8, D8, pos); break;
    case G1: _ce_move_piece(H1, F1, pos); break;
    case G8: _ce_move_piece(H8, F8, pos); break;
    default: ASSERT(FALSE); break;
    }
  }

  if (pos->enPassent != NO_SQ) {
    // remove the en passent state from the hash
    HASH_EP;
  }
  // remove the castle permission from the hash
  HASH_CA;

  // update the castle permissions
  pos->castlePerms &= tbl_castle_perms[at];
  pos->castlePerms &= tbl_castle_perms[to];
  pos->enPassent = NO_SQ;

  // add the castle permissions to the hash
  HASH_CA;

  // increment the fifty move count
  pos->fiftyMove++;

  // check the fifty move rule and update
  captured = CAPTURED(move);
  if (captured != EMPTY) {
    ASSERT(ce_valid_piece(captured));
    _ce_clear_piece(to, pos);
    pos->fiftyMove = 0;
  }

  // update the ply and history ply
  pos->historyPly++;
  pos->ply++;

  // set the en passent square
  if (IsPw(pos->pieces[at])) {
    pos->fiftyMove = 0;
    if (move & MFLAGPS) {
      if (side == WHITE) {
        pos->enPassent = at + 10;
        ASSERT(tbl_ranks_board[pos->enPassent] == RANK_3);
      } else if (side == BLACK) {
        pos->enPassent = at - 10;
        ASSERT(tbl_ranks_board[pos->enPassent] == RANK_6);
      }
      HASH_EP;
    }
  }

  // move the piece on the board
  _ce_move_piece(at, to, pos);

  // promote the piece if necessary
  prPce = PROMOTED(move);
  if (prPce != EMPTY) {
    ASSERT(ce_valid_piece(prPce) && !IsPw(prPce));
    _ce_clear_piece(to, pos);
    _ce_add_piece(to, pos, prPce);
  }

  // update the king square
  if (IsKi(pos->pieces[to])) {
    pos->kingSq[pos->side] = to;
  }

  // change the current side
  pos->side ^= 1;
  HASH_SIDE;

  // check the board position
  CHKBRD(pos);

  // dissallow a move that leaves the king in check
  if (ce_is_square_attacked(pos->kingSq[side], pos->side, pos)) {
    ce_take_move(pos);
    return FALSE;
  }

  return TRUE;
}

