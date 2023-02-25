
#include "defs.h"

/**
 * Linux terminal colours (the order here matches the colours enum in defs.h)
 */
static const char *colours[] = {
  u8"\x1B[0m", u8"\x1B[0;31m", u8"\x1B[0;32m", u8"\x1B[0;33m", u8"\x1B[0;34m", u8"\x1B[0;35m", u8"\x1B[0;36m", u8"\x1B[0;37m", u8"\x1B[0;30m"
};
static const char *bold_colours[] = {
  u8"\x1B[0m", u8"\x1B[1;31m", u8"\x1B[1;32m", u8"\x1B[1;33m", u8"\x1B[1;34m", u8"\x1B[1;35m", u8"\x1B[1;36m", u8"\x1B[1;37m", u8"\x1B[1;30m"
};
static const char *bkg_colours[] = {
  u8"\x1B[0m", u8"\x1B[1;41m", u8"\x1B[1;42m", u8"\x1B[1;43m", u8"\x1B[1;44m", u8"\x1B[1;45m", u8"\x1B[1;46m", u8"\x1B[1;47m", u8"\x1B[1;40m"
};


/**
 * Chess Engine diagnostic function for printing the files and ranks
 * board tables.
 * [INTERNAL]
 */
static void _ce_diag_print_tbl_files_ranks_boards() {
  int32_t index;

  printf(u8"Files Board:\n");
  for (index = 0; index < NUM_BRD_SQ; ++index) {
    if ((index % 10) == 0) {
      printf(u8"\n");
    }
    printf(u8"%4d", tbl_files_board[index]);
  }
  printf(u8"\n\nRanks Board:\n");
  for (index = 0; index < NUM_BRD_SQ; ++index) {
    if ((index % 10) == 0) {
      printf(u8"\n");
    }
    printf(u8"%4d", tbl_ranks_board[index]);
  }
  printf(u8"\n\n");
}

/**
 * Chess Engine diagnostic function for printing out the 120 square
 * to 64 square board positiosn conversion tables.
 * [INTERNAL]
 */
static void _ce_diag_print_sq120_to_sq64() {
  int32_t index;

  for (index = 0; index < NUM_BRD_SQ; ++index) {
    if (index % 10 == 0) {
      printf(u8"\n");
    }   
    printf(u8"%5d", tbl_sq120_to_sq64[index]);
  }

  printf(u8"\n\n");

  for (index = 0; index < 64; ++index) {
    if (index % 8 == 0) {
      printf(u8"\n");
    }   
    printf(u8"%5d", tbl_sq64_to_sq120[index]);
  }
  printf(u8"\n\n");
}

/**
 * Chess Engine diagnostic method for printing out the conversion
 * tables the engine uses for board positions and board states.
 */
void ce_diag_print_tbls() {
  _ce_diag_print_sq120_to_sq64();
  _ce_diag_print_tbl_files_ranks_boards();
}

/**
 * Chess Engine diagnostic function for printing out the provided
 * board position.
 * @param board The chess board position as a 64-bit unsigned int
 */
void ce_diag_print_bitboard(uint64_t board) {
  uint64_t mask = 1ULL;
  int32_t rank, file;

  printf(u8"\n");
  for (rank = RANK_8; rank >= RANK_1; --rank) {
    for (file = FILE_A; file <= FILE_H; ++file) {
      int32_t sq = FR2SQ(file, rank);
      int32_t sq64 = SQ64(sq);

      if (board & (mask << sq64)) {
        printf(u8"X");
      } else {
        printf(u8"-");
      }   
    }   
    printf(u8"\n");
  }
  printf(u8"\n\n");
}

/**
 * Chess Engine diagnostic function for printing out the provided
 * board position.
 * @param pos A pointer to a board_s containing the board position
 *   to print.
 */
void ce_print_board(const struct board_s *pos) {
  ce_print_coloured_board(pos, CLR_NORMAL, CLR_NORMAL, CLR_NORMAL);
}

void ce_print_coloured_board(const struct board_s *pos, int32_t white, int32_t black, int32_t highlight) {
  int32_t move = NOMOVE, colour = CLR_NORMAL;
  int32_t file, rank, piece;

  if (pos->historyPly > 0) {
    move = pos->history[pos->historyPly - 1].move;
  }

  for (rank = RANK_8; rank >= RANK_1; --rank) {
    printf(u8"%d  ", rank + 1);
    for (file = FILE_A; file <= FILE_H; ++file) {
      int32_t sq = FR2SQ(file, rank);
      piece = pos->pieces[sq];
      if (FROMSQ(move) == sq) {
        // Add the spaces in here manually because otherwise they will be highlighted
        printf(u8"  %s%s%c", colours[CLR_NORMAL], bkg_colours[highlight], tbl_piece_char[piece]);
        printf(u8"%s", colours[CLR_NORMAL]);
      } else if (TOSQ(move) == sq) {
        printf(u8"%s%3c", bold_colours[highlight], tbl_piece_char[piece]);
        printf(u8"%s", colours[CLR_NORMAL]);
      } else {
        colour = ((tbl_piece_col[piece] == WHITE) ? white : ((tbl_piece_col[piece] == BLACK) ? black : CLR_NORMAL));
        printf(u8"%s%3c", colours[colour], tbl_piece_char[piece]);
        printf(u8"%s", colours[CLR_NORMAL]);
      }
    }
    printf(u8"%s\n", colours[CLR_NORMAL]);
  }

  printf(u8"%s\n   ", colours[CLR_NORMAL]);
  for (file = FILE_A; file <= FILE_H; ++file) {
    printf(u8"%3c", 'a' + file);
  }
  printf(u8"\n");
  printf(u8"side: %c\n", tbl_side_char[pos->side]);
  if (pos->enPassent != NO_SQ && pos->enPassent != OFFBOARD) {
    printf(u8"enPassent: %c%c    %d\n",
      'a' + SQ2FILE(pos->enPassent),
      '1' + SQ2RANK(pos->enPassent),
      pos->enPassent);
  } else {
    printf(u8"enPassent none\n");
  }
  printf(u8"castle: %c%c%c%c\n",
    ((pos->castlePerms & WKCA) ? 'K' : '-'),
    ((pos->castlePerms & WQCA) ? 'Q' : '-'),
    ((pos->castlePerms & BKCA) ? 'k' : '-'),
    ((pos->castlePerms & BQCA) ? 'q' : '-'));
  printf(u8"position: %llX\n", pos->positionKey);
  printf(u8"\n");
}

/**
 * TODO:
 */
void ce_print_fen(const struct board_s *pos) {
  int32_t rank = RANK_8, file = FILE_A;
  int32_t piece = 0;
  int32_t count = 0;
  int32_t i = 0;
  int32_t sq64 = 0, sq120 = 0;

  ASSERT(pos != NULL);

  printf(u8"FEN: ");

  for (rank = RANK_8; rank >= RANK_1; --rank) {
    if (count > 0) {
      printf(u8"%d", count);
    }
    if (rank != RANK_8) {
      printf(u8"/");
    }
    count = 0;
    for (file = FILE_A; file <= FILE_H; ++file) {
      sq64 = rank * 8 + file;
      sq120 = SQ120(sq64);
      piece = pos->pieces[sq120];
      if (piece != EMPTY && count > 0) {
        printf(u8"%d", count);
        count = 0;
      }
      switch (piece) {
      case bP: printf(u8"p"); break;
      case bR: printf(u8"r"); break;
      case bN: printf(u8"n"); break;
      case bB: printf(u8"b"); break;
      case bK: printf(u8"k"); break;
      case bQ: printf(u8"q"); break;
      case wP: printf(u8"P"); break;
      case wR: printf(u8"R"); break;
      case wN: printf(u8"N"); break;
      case wB: printf(u8"B"); break;
      case wK: printf(u8"K"); break;
      case wQ: printf(u8"Q"); break;
      case EMPTY:
      default: ++count;
      }
    }
  }

  printf(u8" %c", pos->side == WHITE ? 'w' : pos->side == BLACK ? 'b' : '-');

  printf(u8" %c%c%c%c", pos->castlePerms & WKCA ? 'K' : '-',
    pos->castlePerms & WQCA ? 'Q' : '-',
    pos->castlePerms & BKCA ? 'k' : '-',
    pos->castlePerms & BQCA ? 'q' : '-');

  if (pos->enPassent == NO_SQ) {
    printf(u8" -");
  } else {
    file = SQ2FILE(pos->enPassent);
    rank = SQ2RANK(pos->enPassent);

    ASSERT(file >= FILE_A && file <= FILE_H);
    ASSERT(rank >= RANK_1 && rank <= RANK_8);

     printf(u8" %c%c", 'a' + file, '1' + rank);
  }

  printf(u8" %d", pos->fiftyMove);
  printf(u8" %d", (pos->historyPly / 2) + 1);

  printf(u8"\n");
}

/**
 * Chess Engine diagnostic function to print out the piece attacked status from
 * the provided board position and side.
 * @param side The side to display the attacked pieces
 * @param pos A pointer to a board position structure containing the current board
 *    position.
 */
void ce_diag_show_attacked_by_side(const int32_t side, const struct board_s *pos) {
  int32_t rank, file, sq;

  printf(u8"\n\nSquares attacked by: %c\n", tbl_side_char[side]);
  for (rank = RANK_8; rank >= RANK_1; --rank) {
    for (file = FILE_A; file <= FILE_H; ++file) {
      sq = FR2SQ(file, rank);
      if (ce_is_square_attacked(sq, side, pos) == true) {
        printf(u8"X");
      } else {
        printf(u8"-");
      }
    }
    printf(u8"\n");
  }
  printf(u8"\n\n");
}

/**
 * Chess Engine diagnostic function to print out the provided integer value
 * as a binary digit.
 * @param val The value to print.
 */
void ce_print_binary(int32_t val) {
  const int32_t size = sizeof(int32_t) * 8;
  int32_t index = 0;

  for (index = size - 1; index >= 0; --index) {
    if ((1 << index) & val) {
      printf(u8"1");
    } else {
      printf(u8"0");
    }
    if (((index % 4) == 0)) {
      printf(u8" ");
    }
  }
}
