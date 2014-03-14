
#include "defs.h"

/*
int ce_nega_max(int depth) {
  int score = -INT_MAX;

  return ce_score_from_side() if depth < 1;

  for (move : pos) {
    ce_make_move(move, pos);
    curr_score = -ce_nega_max(depth - 1);
    ce_take_move(pos);
    if (curr_score > score) {
      score = curr_score;
    }
  }
  return score;
}
 */

/*
int ce_alpha_beta(int depth, int alpha, int beta) {
  int score = -INT_MAX;

  return ce_score_from_side() if depth < 1;

  for (move : pos) {
    ce_make_move(move, pos);
    curr_score = -ce_alpha_beta(depth - 1, -beta, -alpha);
    ce_take_move(pos);
    return beta if curr_score >= beta;
    return alpha if curr_score > alpha;
  }

  return alpha;
}
 */

/*
int ce_search(int depth) {
  int curr_depth = 1;
  ce_iterative_deepening(depth) {
    ce_alpha_beta(depth);
    curr_depth++;
  }
}
 */

int ce_is_repetition(const struct board_s *pos) {
  int index = 0;

  // start from the last capture or pawn move
  for (index = pos->historyPly - pos->fiftyMove; index < pos->historyPly - 1; ++index) {
    ASSERT(index >= 0 && index <= MAX_GAME_MOVES);

    if (pos->positionKey == pos->history[index].positionKey) {
      return TRUE;
    }
  }

  return FALSE;
}

void ce_search_position(struct board_s *pos) {
}

