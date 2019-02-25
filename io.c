
#include "defs.h"

/**
 * Chess Engine printing function to print the provided chess board square.
 * @param sq The chess board square to print.
 * @return A string with the display value for the provided chess board square.
 */
char *ce_print_sq(const int sq) {
  // TODO: This function is dangerous
  static char sqStr[3];

  int file = tbl_files_board[sq];
  int rank = tbl_ranks_board[sq];

  sprintf(sqStr, "%c%c", ('a' + file), ('1' + rank));

  return sqStr;
}

/**
 * Chess Engine printing function to print the provided chess move.
 * @param move The move to print.
 * @return A string with the display value for the provided chess move.
 */
char *ce_print_move(const union move_u move) {
  // TODO: This function is dangerous
  static char moveStr[12] = { 0 };

  int fileAt = tbl_files_board[move.at];
  int rankAt = tbl_ranks_board[move.at];
  int fileTo = tbl_files_board[move.to];
  int rankTo = tbl_ranks_board[move.to];

  int promoted = move.promoted;
  memset(moveStr, 0, 6);

  if (move.castle) {
    switch (move.to) {
    case C1:
    case C8:
      sprintf(moveStr, "0-0-0");
      break;
    case G1:
    case G8:
      sprintf(moveStr, "0-0");
      break;
    }
  } else if (promoted) {
    char pchar = 'q';
    if (IsKn(promoted)) {
      pchar = 'n';
    } else if (IsRQ(promoted) && !IsBQ(promoted)) {
      pchar = 'r';
    } else if (IsBQ(promoted) && !IsRQ(promoted)) {
      pchar = 'b';
    }

    if (move.captured) {
      sprintf(moveStr, "%c%cx%c%c%c", ('a' + fileAt), ('1' + rankAt), ('a' + fileTo), ('1' + rankTo), pchar);
    } else {
      sprintf(moveStr, "%c%c%c%c%c", ('a' + fileAt), ('1' + rankAt), ('a' + fileTo), ('1' + rankTo), pchar);
    }
  } else {
    if (move.captured) {
      sprintf(moveStr, "%c%cx%c%c", ('a' + fileAt), ('1' + rankAt), ('a' + fileTo), ('1' + rankTo));
    } else {
      sprintf(moveStr, "%c%c%c%c", ('a' + fileAt), ('1' + rankAt), ('a' + fileTo), ('1' + rankTo));
    }
  }

  if (move.enPassent) {
    sprintf(moveStr, "%se.p.", moveStr);
  }
  if (move.check) {
    sprintf(moveStr, "%s+", moveStr);
  }

  return moveStr;
}

/**
 * Chess Engine printing function to print out the provided chess move list.
 * @param list A pointer to a chess move list structure to print the moves contained therein.
 */
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

/**
 * Chess Engine parsing function that parses the provided string for a
 * chess move validated with the provided board position.
 * @param ptrChar A string containing the chess move to parse.
 * @param pos The current board position used for move validation.
 * @return The integer representation of the parsed chess move.
 */
int ce_parse_move(char *ptrChar, struct board_s *pos) {
  // XXX: This function assumes a string of char[4+]
  struct move_list_s list;
  int moveNum, move;
  int at, to;

  if (!strncmp(ptrChar, "0-0-0", 5)) {
    if (pos->side == WHITE) {
      ptrChar = "e1c1";
    } else if (pos->side == BLACK) {
      ptrChar = "e8c8";
    }
  } else if (!strncmp(ptrChar, "0-0", 3)) {
    if (pos->side == WHITE) {
      ptrChar = "e1g1";
    } else if (pos->side == BLACK) {
      ptrChar = "e8g8";
    }
  }

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
    // printf("Move %2d at: %d to: %d => %d\n", moveNum, move.at, move.to, move.val);
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

