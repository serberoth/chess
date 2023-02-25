
#include "defs.h"

/**
 * Chess Engine validation function that validates that the provided
 * square is a valid square on the chess board.
 * @param sq The chess board square to validate.
 * @return Boolean value indicating the validity of the square.
 */
bool ce_valid_square(const int32_t sq) {
  return tbl_files_board[sq] == OFFBOARD ? false : true;
}

/**
 * Chess Engine validation function that validates the provided side
 * value is either of WHITE or BLACK.
 * @param side The side value to validate.
 * @param Boolean value indicating the validity of the side value.
 */
bool ce_valid_side(const int32_t side) {
  return (side == WHITE || side == BLACK) ? true: false;
}

/**
 * Chess Engine validation function that validates the provided
 * file rank on the board.
 * @param fr The file rank of the board to validte.
 * @return Boolean value indicating the validity of the file rank value.
 */
bool ce_valid_file_rank(const int32_t fr) {
  return (fr >= FILE_A && fr <= FILE_H) ? true : false;
}

/**
 * Chess Engine validation function that validates the provided
 * piece including the empty/no piece board representation value.
 * @param pce The numerical value of the piece to validate.
 * @return Boolean value indicating the validity of the piece value.
 */
bool ce_valid_piece_empty(const int32_t pce) {
  return (pce >= EMPTY && pce <= bK) ? true : false;
}

/**
 * Chess Engine validation function that validates the provided
 * piece value.
 * @param pce The numerical value of the piece to validate.
 * @return Boolean value indicating the validity of the piece value.
 */
bool ce_valid_piece(const int32_t pce) {
  return (pce >= wP && pce <= bK) ? true : false;
}


/**
 * Chess Engine validation function for the fifty move rule
 * @param pos The current board position
 * @return Boolean value indicating if the fifty move rule has been invoked
 */
bool ce_is_fifty_move(const struct board_s *pos) {
  return pos->fiftyMove > 100;
}

/**
 * Chess Engine validation function for a position with a three fold repetition
 * @param pos The current board position
 * @return Boolean value indicating if the board position represents a three fold repetition
 */
bool ce_is_three_fold_repetition(const struct board_s *pos) {
  int32_t i = 0, r = 0;
  for (i = 0; i < pos->historyPly; ++i) {
    if (pos->history[i].positionKey == pos->positionKey) {
      ++r;
    }
  }
  return r > 2;
}

/**
 * Chess Engine valiation function for a position with insufficient material
 * @param pos The current board position
 * @return Boolean value indicating if the board position represents a position with insufficient material
 */
bool ce_is_draw_material(const struct board_s *pos) {
  if (pos->pieceNum[wP] || pos->pieceNum[bP]) {
    return false;
  }
  if (pos->pieceNum[wQ] || pos->pieceNum[bQ] || pos->pieceNum[wR] || pos->pieceNum[bR]) {
    return false;
  }
  if (pos->pieceNum[wB] > 1 || pos->pieceNum[bB] > 1) {
    return false;
  }
  if (pos->pieceNum[wN] > 1 || pos->pieceNum[bN] > 1) {
    return false;
  }
  if (pos->pieceNum[wN] && pos->pieceNum[wB]) {
    return false;
  }
  if (pos->pieceNum[bN] && pos->pieceNum[bB]) {
    return false;
  }
  return true;
}

/**
 * Chess Engine validation function for a checkmated position
 * @param pos The current board position
 * @return Boolean value indicating if the board position represents a checkmate
 */
bool ce_is_moves_available(const struct board_s *pos) {
  struct board_s b = { 0 };
  struct move_list_s list = { 0 };
  bool found = false;

  // XXX: I think this makes every movegen call that much more expensive.
  memcpy((void *) &b, (void *) pos, sizeof(struct board_s));

  ce_generate_all_moves(&b, &list);
  for (size_t moveNum = 0; moveNum < list.count; ++moveNum) {
    if (!ce_move_make(&b, list.moves[moveNum].move.val)) {
      continue;
    }
    ce_move_take(&b);
    return true;
  }

  return false;
}

void ce_mirror_eval_test(struct board_s *pos) {
  FILE *file = fopen("mirror.epd", "rt");
  char line[1024] = { 0 };
  int32_t ev1, ev2 = 0;
  int32_t positions = 0;

  if (file == NULL) {
    return;
  }

  while (fgets(line, 1024, file) != NULL) {
    ce_parse_fen(line, pos);
    ++positions;
    ev1 = ce_eval_position(pos);
    ce_mirror_board(pos);
    ev2 = ce_eval_position(pos);

    if (ev1 != ev2) {
      printf("\n\n\n");
      ce_parse_fen(line, pos);
      ce_print_board(pos);
      printf("Board Evaluation: %d\n", ev1);
      ce_mirror_board(pos);
      ce_print_board(pos);
      printf("Board Evaluation: %d\n", ev2);
      printf("\n\nMirror Failed:\n%s\n", line);
      getchar();
      return;
    }

    if ((positions % 1000) == 0) {
      printf("position %d\n", positions);
    }

    memset(line, 0, sizeof(line));
  }
}