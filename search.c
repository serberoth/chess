
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

static int _ce_is_repetition(const struct board_s *pos) {
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

static void _ce_checkup() {
  // check if time up, or interrupt from ui
}

static void _ce_clear_for_search(struct board_s *pos, struct search_info_s *info) {
  int index = 0, index2 = 0;

  for (index = 0; index < 13; ++index) {
    for (index2 = 0; index2 < NUM_BRD_SQ; ++index2) {
      pos->searchHistory[index][index2] = 0;
    }
  }

  for (index = 0; index < 2; ++index) {
    for (index2 = 0; index2 < MAX_DEPTH; ++index2) {
      pos->searchKillers[index][index2] = 0;
    }
  }

  ce_pvtable_clear(&pos->pvtable);
  pos->ply = 0;

  info->startTime = sys_time_ms();
  info->stopped = 0;
  info->nodes = 0UL;

  info->failHigh = 0.0f;
  info->failHighFirst = 0.0f;
}

static void _ce_select_move(int moveNum, struct move_list_s *list) {
  struct move_s temp = { 0 };
  int index = 0;
  int bestScore = 0;
  int bestNum = moveNum;

  for (index = moveNum; index < list->count; ++index) {
    if (list->moves[index].score > bestScore) {
      bestScore = list->moves[index].score;
      bestNum = index;
    }   
  }

  temp = list->moves[moveNum];
  list->moves[moveNum] = list->moves[bestNum];
  list->moves[bestNum] = temp;
}

static int _ce_alpha_beta(int alpha, int beta, int depth, struct board_s *pos, struct search_info_s *info, int do_null) {
  struct move_list_s list;
  int moveNum = 0;
  int legal = 0;
  int oldAlpha = alpha;
  int bestMove = NOMOVE;
  int score = -INFINITY;

  CHKBRD(pos);

  if (depth == 0) {
    info->nodes++;
    return ce_eval_position(pos);
  }

  info->nodes++;

  if (_ce_is_repetition(pos) || pos->fiftyMove >= 100) {
    return 0;
  }

  if (pos->ply > MAX_DEPTH - 1) {
    return ce_eval_position(pos);
  }

  ce_generate_all_moves(pos, &list);

  // negamax implementation of alpha-beta search
  for (moveNum = 0; moveNum < list.count; ++moveNum) {
    _ce_select_move(moveNum, &list);

    if (!ce_make_move(pos, list.moves[moveNum].move)) {
      continue;
    }

    legal++;
    score = -_ce_alpha_beta(-beta, -alpha, depth - 1, pos, info, TRUE);

    ce_take_move(pos);

    if (score > alpha) {
      if (score >= beta) {
        if (legal == 1) {
          info->failHighFirst++;
        }
        info->failHigh++;
        return beta;
      }

      alpha = score;
      bestMove = list.moves[moveNum].move;
    }
  }

  if (legal == 0) {
    // determine mate in num moves or stalemate
    if (ce_is_square_attacked(pos->kingSq[pos->side], pos->side ^ 1, pos)) {
      return -MATE + pos->ply;
    } else {
      return 0;
    }
  }

  if (alpha != oldAlpha) {
    ce_pvtable_store(pos, bestMove);
  }

  return alpha;
}

static int _ce_quiescence(int alpha, int beta, struct board_s *pos, struct search_info_s *info) {
  return 0;
}

void ce_search_position(struct board_s *pos, struct search_info_s *info) {
  // iterative deepening
  int bestMove = NOMOVE;
  int bestScore = -INFINITY;
  int currentDepth = 0;
  int pvMoves = 0;
  int pvNum = 0;

  _ce_clear_for_search(pos, info);

  for (currentDepth = 1; currentDepth <= info->depth; ++currentDepth) {
    bestScore = _ce_alpha_beta(-INFINITY, INFINITY, currentDepth, pos, info, TRUE);
    pvMoves = ce_pvtable_get_line(currentDepth, pos);
    bestMove = pos->pvarray[0];

    printf("Depth %d score: %d move: %s nodes: %ld ", currentDepth, bestScore, ce_print_move(MV(bestMove)), info->nodes);

    pvMoves = ce_pvtable_get_line(currentDepth, pos);
    printf("pv");
    for (pvNum = 0; pvNum < pvMoves; ++pvNum) {
      printf(" %s", ce_print_move(MV(pos->pvarray[pvNum])));
    }
    printf("\n");
    // TODO: This does not handle division by zero
    printf("Ordering: %.2f (%.2f / %.2f)\n", (info->failHighFirst / info->failHigh), info->failHighFirst, info->failHigh);
  }
}

