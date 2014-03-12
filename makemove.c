
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

