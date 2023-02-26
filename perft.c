
#include "defs.h"

/**
 * Chess Engine performance test function that evaluates the provided board position
 * at the provided depth.
 * [INTERNAL]
 * @param depth The current move tree evaluation depth.
 * @param pos A pointer to the current board position.
 * @return The number of evaluated leaf nodes.
 */
static size_t _ce_perft(int32_t depth, struct board_s *pos) {
  struct move_list_s list = { };
  size_t leafNodes = 0ull;

  CHKBRD(pos);

  if (depth == 0) {
    return 1ull;
    // leafNodes++;
    // return;
  }

  ce_generate_all_moves(pos, &list);

  for (size_t index = 0; index < list.count; ++index) {
    if (!ce_move_make(pos, list.moves[index].move.val)) {
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
size_t ce_perf_test(int32_t depth, struct board_s *pos) {
  struct move_list_s list = { };
  size_t leafNodes = 0ull;
  size_t curr_nodes, last_nodes;
  int32_t start;

  CHKBRD(pos);

  ce_print_board(pos);

  printf(u8"Starting test to depth: %d\n", depth);
  start = sys_time_ms();

  ce_generate_all_moves(pos, &list);

  for (size_t index = 0; index < list.count; ++index) {
    union move_u move = list.moves[index].move;
    if (!ce_move_make(pos, move.val)) {
      continue;
    }

    curr_nodes = leafNodes;
    leafNodes += _ce_perft(depth - 1, pos);
    ce_move_take(pos);
    last_nodes = leafNodes - curr_nodes;
    printf(u8"Move %lu : %s %zu\n", index + 1, ce_print_move(move), last_nodes);
  }

  printf(u8"\nTest complete : %zu nodes visited in %dms\n", leafNodes, (sys_time_ms() - start));
  return leafNodes;
}
