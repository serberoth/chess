
#include "defs.h"

// static long leafNodes;

/**
 * Chess Engine performance test function that evaluates the provided board position
 * at the provided depth.
 * [INTERNAL]
 * @param depth The current move tree evaluation depth.
 * @param pos A pointer to the current board position.
 * @return The number of evaluated leaf nodes.
 */
static unsigned long _ce_perft(int depth, struct board_s *pos) {
  struct move_list_s list = { 0 };
  unsigned long leafNodes = 0UL;
  int index;

  CHKBRD(pos);

  if (depth == 0) {
    return 1UL;
    // leafNodes++;
    // return;
  }

  ce_generate_all_moves(pos, &list);

  for (index = 0; index < list.count; ++index) {
    if (!ce_move_make(pos, list.moves[index].move)) {
      continue;
    }

    leafNodes += _ce_perft(depth - 1,  pos);
    ce_move_take(pos);
  }

  return leafNodes;
}

/**
 * Chess Engine performance test function that evaluates the provided board position
 * to the provided depth.
 * @param depth The maximum move tree evaluation depth.
 * @param pos A pointer to the current board position.
 * @return The number of evaluated leaf nodes.
 */
unsigned long ce_perf_test(int depth, struct board_s *pos) {
  struct move_list_s list = { 0 };
  unsigned long leafNodes = 0UL;
  unsigned long cumm_nodes, last_nodes;
  int index;
  int start;

  CHKBRD(pos);

  ce_print_board(pos);

  printf("Starting test to depth: %d\n", depth);
  start = sys_time_ms();

  ce_generate_all_moves(pos, &list);

  for (index = 0; index < list.count; ++index) {
    union move_u move = list.moves[index].fields;
    if (!ce_move_make(pos, move.val)) {
      continue;
    }

    cumm_nodes = leafNodes;
    leafNodes += _ce_perft(depth - 1, pos);
    ce_move_take(pos);
    last_nodes = leafNodes - cumm_nodes;
    printf("Move %d : %s %lu\n", index + 1, ce_print_move(move), last_nodes);
  }

  printf("\nTest complete : %lu nodes visited in %dms\n", leafNodes, (sys_time_ms() - start));
  return leafNodes;
}

