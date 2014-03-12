
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

