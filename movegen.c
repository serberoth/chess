
#include "defs.h"

/*
ce_move_gen(struct board_s *board, struct move_list_s *list)
  for each piece
    slider loop each direction add move
      add move { list->moves[list->count] = move; list->count++ }
 */

void ce_add_quiet_move(const struct board_s *pos, int move, struct move_list_s *list) {
  list->moves[list->count].move = move;
  list->moves[list->count].score = 0;
  list->count++;
}

void ce_add_capture_move(const struct board_s *pos, int move, struct move_list_s *list) {
  list->moves[list->count].move = move;
  list->moves[list->count].score = 0;
  list->count++;
}

void ce_add_enpassent_move(const struct board_s *pos, int move, struct move_list_s *list) {
  list->moves[list->count].move = move;
  list->moves[list->count].score = 0;
  list->count++;
}

void ce_generate_all_moves(const struct board_s *pos, struct move_list_s *list) {
  list->count = 0;
}

