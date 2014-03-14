
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

int ce_parse_move(char *ptrChar, struct board_s *pos) {
  // XXX: This function assumes a string of char[4+]
  struct move_list_s list;
  int moveNum, move;
  int at, to;

  // Convert the string to lowercase characters
  if (ptrChar[0] >= 'A' && ptrChar[0] <= 'H') {
    ptrChar[0] = 'a' + (ptrChar[0] - 'A');
  }
  if (ptrChar[2] >= 'A' && ptrChar[2] <= 'H') {
    ptrChar[2] = 'a' + (ptrChar[2] - 'A');
  }

  // Assert that the string contains a valid move
  if ((ptrChar[1] > '8' || ptrChar[1] < '1')
    || (ptrChar[3] > '8' || ptrChar[3] < '1')
    || (ptrChar[0] > 'h' || ptrChar[0] < 'a')
    || (ptrChar[2] > 'h' || ptrChar[2] < 'a')) {
    return NOMOVE;
  }

  // Convert the move string to board positions
  at = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
  to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');

  // printf("Move: %s at: %d to: %d\n", ptrChar, at, to);

  ASSERT(ce_valid_square(at));
  ASSERT(ce_valid_square(to));

  ce_generate_all_moves(pos, &list);

  for (moveNum = 0; moveNum < list.count; ++moveNum) {
    union move_u move = list.moves[moveNum].fields;
    if (move.at == at && move.to == to) {
      int promPce = move.promoted;
      if (promPce != EMPTY) {
        if (IsRQ(promPce) && !IsBQ(promPce) && (ptrChar[4] == 'r' || ptrChar[4] == 'R')) {
          return move.val;
        } else if (!IsRQ(promPce) && IsBQ(promPce) && (ptrChar[4] == 'b' || ptrChar[4] == 'B')) {
          return move.val;
        } else if (IsRQ(promPce) && IsBQ(promPce) && (ptrChar[4] == 'q' || ptrChar[4] == 'Q')) {
          return move.val;
        } else if (IsKn(promPce) && (ptrChar[4] == 'n' || ptrChar[4] == 'N')) {
          return move.val;
        }
        continue;
      }
      return move.val;
    }
  }

  return NOMOVE;
}

