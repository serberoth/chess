
#include "defs.h"

/*
int ce_nega_max(int depth) {
  int score = -INT_MAX;

  return ce_score_from_side() if depth < 1;

  for (move : pos) {
    ce_move_make(move, pos);
    curr_score = -ce_nega_max(depth - 1);
    ce_move_take(pos);
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
    ce_move_make(move, pos);
    curr_score = -ce_alpha_beta(depth - 1, -beta, -alpha);
    ce_move_take(pos);
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

/**
 * Chess Engine function to determine if the board position is a repetition of state
 * for things like the fifty move rule.
 * [INTERNAL]
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

/**
 * Chess Engine function to clear the search state of any previous search from the
 * provided board position and resetting the search using the provided search
 * parameters.
 * [INTERNAL]
 * @param pos A pointer to the current board position.
 * @param info A pointer to the current search parameters.
 */
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

  // info->startTime = sys_time_ms();
  info->stopped = 0;
  info->nodes = 0UL;

  info->failHigh = 0.0f;
  info->failHighFirst = 0.0f;
}

/**
 * Chess Engine function to chech the current time of for the current search.
 * This function will set the search stop field if the search time has expired.
 * [INTERNAL]
 * @param info A pointer to the current search parameters.
 */
static void _ce_check_time(struct search_info_s *info) {
  if (info->timeSet == TRUE && sys_time_ms() > info->stopTime) {
    info->stopped = TRUE;
  }

  sys_read_input(info);
}

/**
 * Chess Engine function to select a move from the provided move list.
 * This function selectes the top scoring move from the list and moves
 * that move into the move at the provided index.
 * [INTERNAL]
 * @param moveNum The index of the move to select from the move list.
 * @param list A pointer to the current move list
 */
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

/**
 * Chess Engine function that performs the quiescence search.
 * [INTERNAL]
 * @param alpha The alpha tree cutoff value.
 * @param beta The beta tree curoff value.
 * @param pos A pointer to the current board position.
 * @param info A pointer to the current search parameters.
 * @return This function returns the cutoff value for the search.
 */
// This method attempts to compensate for the horizon effect when searching moves
static int _ce_quiescence(int alpha, int beta, struct board_s *pos, struct search_info_s *info) {
  struct move_list_s list;
  int moveNum = 0;
  int legal = 0;
  int oldAlpha = alpha;
  int bestMove = NOMOVE;
  int score = -INFINITY;
  int pvMove = 0;

  CHKBRD(pos);

  if ((info->nodes & 2047) == 0) {
    _ce_check_time(info);
  }

  info->nodes++;

  if (_ce_is_repetition(pos) || pos->fiftyMove >= 100) {
    return 0;
  }

  if (pos->ply > MAX_DEPTH - 1) {
    return ce_eval_position(pos);
  }

  score = ce_eval_position(pos);

  if (score >= beta) {
    return beta;
  }

  if (score > alpha) {
    alpha = score;
  }

  ce_generate_capture_moves(pos, &list);

  pvMove = ce_pvtable_probe(pos);

  for (moveNum = 0; moveNum < list.count; ++moveNum) {
    _ce_select_move(moveNum, &list);

    if (!ce_move_make(pos, list.moves[moveNum].move)) {
      continue;
    }

    ++legal;
    score = -_ce_quiescence(-beta, -alpha, pos, info);
    ce_move_take(pos);

    if (info->stopped == TRUE) {
      return 0;
    }

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
   
  if (alpha != oldAlpha) {
    ce_pvtable_store(pos, bestMove);
  }

  return alpha;
}

/**
 * Chess Engine function that performs alhpa-beta tree search.
 * [INTERNAL]
 * @param alpha The alpha cutoff value.
 * @param beta The beta cutoff value.
 * @param depth The current search depth.
 * @param pos A pointer to the current board position.
 * @param info A pointer to the current search parameters.
 * @param do_null An integer flag value indicating ??? [This parameter is currently unused]
 * @return This function returns the cutoff value for the search.
 */
static int _ce_alpha_beta(int alpha, int beta, int depth, struct board_s *pos, struct search_info_s *info, int do_null) {
  struct move_list_s list;
  int moveNum = 0;
  int legal = 0;
  int oldAlpha = alpha;
  int bestMove = NOMOVE;
  int score = -INFINITY;
  int pvMove = NOMOVE;

  CHKBRD(pos);

  if (depth == 0) {
    info->nodes++;
    // return ce_eval_position(pos);
    return _ce_quiescence(alpha, beta, pos, info);
  }

  if ((info->nodes & 2047) == 0) {
    _ce_check_time(info);
  }

  info->nodes++;

  if (_ce_is_repetition(pos) || pos->fiftyMove >= 100) {
    return 0;
  }

  if (pos->ply > MAX_DEPTH - 1) {
    return ce_eval_position(pos);
  }

  ce_generate_all_moves(pos, &list);

  pvMove = ce_pvtable_probe(pos);

  // follow the principal variation line
  if (pvMove != NOMOVE) {
    for (moveNum = 0; moveNum < list.count; ++moveNum) {
      if (list.moves[moveNum].move == pvMove) {
        list.moves[moveNum].score = 2000000;
        break;
      }
    }
  }

  // negamax implementation of alpha-beta search
  for (moveNum = 0; moveNum < list.count; ++moveNum) {
    _ce_select_move(moveNum, &list);

    if (!ce_move_make(pos, list.moves[moveNum].move)) {
      continue;
    }

    legal++;
    score = -_ce_alpha_beta(-beta, -alpha, depth - 1, pos, info, TRUE);

    ce_move_take(pos);

    if (info->stopped == TRUE) {
      return 0;
    }

    if (score > alpha) {
      if (score >= beta) {
        if (legal == 1) {
          info->failHighFirst++;
        }
        info->failHigh++;

        // keep a list of the moves that terminated the search
        if (!(list.moves[moveNum].move & MFLAGCAP)) {
          pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
          pos->searchKillers[0][pos->ply] = list.moves[moveNum].move;
        }

        return beta;
      }

      alpha = score;
      bestMove = list.moves[moveNum].move;

      // keep a list of the moves that terminated the search
      if (!(list.moves[moveNum].move & MFLAGCAP)) {
        pos->searchHistory[pos->pieces[FROMSQ(bestMove)]][TOSQ(bestMove)] += depth;
      }   
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

/**
 * Chess Engine function to search a given board position for the best possible move.
 * This function outputs several pieces of information during the search process in
 * the UCI chess engine protocol.
 * @param pos A pointer to the current board position.
 * @param info A pointer to the current search parameters.
 */
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

    if (info->stopped == TRUE) {
      break;
    }

    pvMoves = ce_pvtable_get_line(currentDepth, pos);
    bestMove = pos->pvarray[0];

    if (info->gameMode == MODE_UCI) {
      printf("info score cp %d depth %d nodes %ld time %d ", bestScore, currentDepth, info->nodes, sys_time_ms() - info->startTime);
      // printf("Depth %d score: %d move: %s nodes: %ld ", currentDepth, bestScore, ce_print_move(MV(bestMove)), info->nodes);
    } else if (info->gameMode == MODE_XBOARD && info->postThinking == TRUE) {
      printf("%d %d %d %ld ", currentDepth, bestScore, (sys_time_ms() - info->startTime) / 10, info->nodes);
    } else if (info->postThinking == TRUE) {
      printf("score:%d depth:%d nodes:%ld time%d(ms) ", bestScore, currentDepth, info->nodes, sys_time_ms() - info->startTime);
    }

    if (info->gameMode == MODE_UCI || info->postThinking == TRUE) {
      pvMoves = ce_pvtable_get_line(currentDepth, pos);
      printf("pv");
      for (pvNum = 0; pvNum < pvMoves; ++pvNum) {
        printf(" %s", ce_print_move(MV(pos->pvarray[pvNum])));
      }
      printf("\n");
#ifdef DEBUG
      // TODO: This does not handle division by zero
      printf("Ordering: %.2f (%.2f / %.2f)\n", (info->failHighFirst / info->failHigh), info->failHighFirst, info->failHigh);
#endif
    }
  }

  if (info->gameMode == MODE_UCI) {
    // UI Protocol: (UCI Protocol)
    // info score cp 13 depth 1 nodes 13 time 15 pv f1b5
#ifdef DEBUG
    printf("Best Move: %s\n", ce_print_move(MV(bestMove)));
#endif
    printf("bestmove %s\n", ce_print_move(MV(bestMove)));
  } else if (info->gameMode == MODE_XBOARD) {
    printf("mode %s\n", ce_print_move(MV(bestMove)));
    ce_move_make(pos, bestMove);
  } else {
    printf("\n\n***!! Move: %s !!***\n\n", ce_print_move(MV(bestMove)));
    ce_move_make(pos, bestMove);
    ce_print_board(pos);
  }
}

