
#include "defs.h"

char *ce_print_sq(const int sq) {
  // TODO: This function is dangerous
  static char sqStr[3];

  int file = tbl_files_board[sq];
  int rank = tbl_ranks_board[sq];

  sprintf(sqStr, "%c%c", ('a' + file), ('1' + rank));

  return sqStr;
}


char *ce_print_move(const union move_u move) {
  // TODO: This function is dangerous
  static char moveStr[6];

  int fileAt = tbl_files_board[move.at];
  int rankAt = tbl_ranks_board[move.at];
  int fileTo = tbl_files_board[move.to];
  int rankTo = tbl_ranks_board[move.to];

  int promoted = move.promoted;

  if (promoted) {
    char pchar = 'q';
    if (IsKn(promoted)) {
      pchar = 'n';
    } else if (IsRQ(promoted) && !IsBQ(promoted)) {
      pchar = 'r';
    } else if (IsBQ(promoted) && !IsRQ(promoted)) {
      pchar = 'b';
    }
    sprintf(moveStr, "%c%c%c%c%c", ('a' + fileAt), ('1' + rankAt), ('a' + fileTo), ('1' + rankTo), pchar);
  } else {
    sprintf(moveStr, "%c%c%c%c", ('a' + fileAt), ('1' + rankAt), ('a' + fileTo), ('1' + rankTo));
  }

  return moveStr;
}

void ce_print_move_list(const struct move_list_s *list) {
  int index;
  printf("Move list: %d\n", list->count);

  for (index = 0; index < list->count; ++index) {
    union move_u move = list->moves[index].fields;
    int score = list->moves[index].score;

    printf("move: %d > %s (score: %d)\n", index + 1, ce_print_move(move), score);
  }

  printf("Move list total %d moves\n\n", list->count);
}

