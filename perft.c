
#include "defs.h"

// static long leafNodes;

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
    if (!ce_make_move(pos, list.moves[index].move)) {
      continue;
    }

    leafNodes += _ce_perft(depth - 1,  pos);
    ce_take_move(pos);
  }

  return leafNodes;
}

void ce_perft_test(int depth, struct board_s *pos) {
  struct move_list_s list = { 0 };
  unsigned long leafNodes = 0UL;
  unsigned long cumm_nodes, last_nodes;
  int index;

  CHKBRD(pos);

  ce_print_board(pos);

  printf("Starting test to depth: %d\n", depth);

  ce_generate_all_moves(pos, &list);

  for (index = 0; index < list.count; ++index) {
    union move_u move = list.moves[index].fields;
    if (!ce_make_move(pos, move.val)) {
      continue;
    }

    cumm_nodes = leafNodes;
    leafNodes += _ce_perft(depth - 1, pos);
    ce_take_move(pos);
    last_nodes = leafNodes - cumm_nodes;
    printf("Move %d : %s %lu\n", index + 1, ce_print_move(move), last_nodes);
  }

  printf("\nTest complete : %lu nodes visited\n", leafNodes);
}

