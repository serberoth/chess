
#include "defs.h"

/**
 * Linux terminal colours (the order here matches the colours enum in defs.h)
 */
static const char *colours[] = {
  "\x1B[0m", "\x1B[0;31m", "\x1B[0;32m", "\x1B[0;33m", "\x1B[0;34m", "\x1B[0;35m", "\x1B[0;36m", "\x1B[0;37m", "\x1B[0;30m"
};
static const char *bold_colours[] = {
  "\x1B[0m", "\x1B[1;31m", "\x1B[1;32m", "\x1B[1;33m", "\x1B[1;34m", "\x1B[1;35m", "\x1B[1;36m", "\x1B[1;37m", "\x1B[1;30m"
};
static const char *bkg_colours[] = {
  "\x1B[0m", "\x1B[1;41m", "\x1B[1;42m", "\x1B[1;43m", "\x1B[1;44m", "\x1B[1;45m", "\x1B[1;46m", "\x1B[1;47m", "\x1B[1;40m"
};


/**
 * Chess Engine diagnostic function for printing the files and ranks
 * board tables.
 * [INTERNAL]
 */
static void _ce_diag_print_tbl_files_ranks_boards() {
  int index;

  printf("Files Board:\n");
  for (index = 0; index < NUM_BRD_SQ; ++index) {
    if ((index % 10) == 0) {
      printf("\n");
    }
    printf("%4d", tbl_files_board[index]);
  }
  printf("\n\nRanks Board:\n");
  for (index = 0; index < NUM_BRD_SQ; ++index) {
    if ((index % 10) == 0) {
      printf("\n");
    }
    printf("%4d", tbl_ranks_board[index]);
  }
  printf("\n\n");
}

/**
 * Chess Engine diagnostic function for printing out the 120 square
 * to 64 square board positiosn conversion tables.
 * [INTERNAL]
 */
static void _ce_diag_print_sq120_to_sq64() {
  int index;

  for (index = 0; index < NUM_BRD_SQ; ++index) {
    if (index % 10 == 0) {
      printf("\n");
    }   
    printf("%5d", tbl_sq120_to_sq64[index]);
  }

  printf("\n\n");

  for (index = 0; index < 64; ++index) {
    if (index % 8 == 0) {
      printf("\n");
    }   
    printf("%5d", tbl_sq64_to_sq120[index]);
  }
  printf("\n\n");
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
void ce_diag_print_bitboard(U64 board) {
  U64 mask = 1ULL;
  int rank, file;

  printf("\n");
  for (rank = RANK_8; rank >= RANK_1; --rank) {
    for (file = FILE_A; file <= FILE_H; ++file) {
      int sq = FR2SQ(file, rank);
      int sq64 = SQ64(sq);

      if (board & (mask << sq64)) {
        printf("X");
      } else {
        printf("-");
      }   
    }   
    printf("\n");
  }
  printf("\n\n");
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

void ce_print_coloured_board(const struct board_s *pos, int white, int black, int highlight) {
  int move = NOMOVE, colour = CLR_NORMAL;
  int file, rank, piece;

  if (pos->historyPly > 0) {
    move = pos->history[pos->historyPly - 1].move;
  }

  for (rank = RANK_8; rank >= RANK_1; --rank) {
    printf("%d  ", rank + 1);
    for (file = FILE_A; file <= FILE_H; ++file) {
      int sq = FR2SQ(file, rank);
      piece = pos->pieces[sq];
      if (FROMSQ(move) == sq) {
        // Add the spaces in here manually because otherwise they will be highlighted
        printf("  %s%s%c", colours[CLR_NORMAL], bkg_colours[highlight], tbl_piece_char[piece]);
        printf("%s", colours[CLR_NORMAL]);
      } else if (TOSQ(move) == sq) {
        printf("%s%3c", bold_colours[highlight], tbl_piece_char[piece]);
        printf("%s", colours[CLR_NORMAL]);
      } else {
        colour = ((tbl_piece_col[piece] == WHITE) ? white : ((tbl_piece_col[piece] == BLACK) ? black : CLR_NORMAL));
        printf("%s%3c", colours[colour], tbl_piece_char[piece]);
        printf("%s", colours[CLR_NORMAL]);
      }
    }
    printf("%s\n", colours[CLR_NORMAL]);
  }

  printf("%s\n   ", colours[CLR_NORMAL]);
  for (file = FILE_A; file <= FILE_H; ++file) {
    printf("%3c", 'a' + file);
  }
  printf("\n");
  printf("side: %c\n", tbl_side_char[pos->side]);
  if (pos->enPassent != NO_SQ && pos->enPassent != OFFBOARD) {
    printf("enPassent: %c%c    %d\n",
      'a' + SQ2FILE(pos->enPassent),
      '1' + SQ2RANK(pos->enPassent),
      pos->enPassent);
  } else {
    printf("enPassent none\n");
  }
  printf("castle: %c%c%c%c\n",
    ((pos->castlePerms & WKCA) ? 'K' : '-'),
    ((pos->castlePerms & WQCA) ? 'Q' : '-'),
    ((pos->castlePerms & BKCA) ? 'k' : '-'),
    ((pos->castlePerms & BQCA) ? 'q' : '-'));
  printf("position: %llX\n", pos->positionKey);
  printf("\n");
}

/**
 * TODO:
 */
void ce_print_fen(const struct board_s *pos) {
  int rank = RANK_8, file = FILE_A;
  int piece = 0;
  int count = 0;
  int i = 0;
  int sq64 = 0, sq120 = 0;

  ASSERT(pos != NULL);

  printf("FEN: ");

  for (rank = RANK_8; rank >= RANK_1; --rank) {
    if (count > 0) {
      printf("%d", count);
    }
    if (rank != RANK_8) {
      printf("/");
    }
    count = 0;
    for (file = FILE_A; file <= FILE_H; ++file) {
      sq64 = rank * 8 + file;
      sq120 = SQ120(sq64);
      piece = pos->pieces[sq120];
      if (piece != EMPTY && count > 0) {
        printf("%d", count);
        count = 0;
      }
      switch (piece) {
      case bP: printf("p"); break;
      case bR: printf("r"); break;
      case bN: printf("n"); break;
      case bB: printf("b"); break;
      case bK: printf("k"); break;
      case bQ: printf("q"); break;
      case wP: printf("P"); break;
      case wR: printf("R"); break;
      case wN: printf("N"); break;
      case wB: printf("B"); break;
      case wK: printf("K"); break;
      case wQ: printf("Q"); break;
      case EMPTY:
      default: ++count;
      }
    }
  }

  printf(" %c", pos->side == WHITE ? 'w' : pos->side == BLACK ? 'b' : '-');

  printf(" %c%c%c%c", pos->castlePerms & WKCA ? 'K' : '-',
    pos->castlePerms & WQCA ? 'Q' : '-',
    pos->castlePerms & BKCA ? 'k' : '-',
    pos->castlePerms & BQCA ? 'q' : '-');

  if (pos->enPassent == NO_SQ) {
    printf(" -");
  } else {
    file = SQ2FILE(pos->enPassent);
    rank = SQ2RANK(pos->enPassent);

    ASSERT(file >= FILE_A && file <= FILE_H);
    ASSERT(rank >= RANK_1 && rank <= RANK_8);

     printf(" %c%c", 'a' + file, '1' + rank);
  }

  printf(" %d", pos->fiftyMove);
  printf(" %d", (pos->historyPly / 2) + 1);

  printf("\n");
}

/**
 * Chess Engine diagnostic function to print out the piece attacked status from
 * the provided board position and side.
 * @param side The side to display the attacked pieces
 * @param pos A pointer to a board position structure containing the current board
 *    position.
 */
void ce_diag_show_attacked_by_side(const int side, const struct board_s *pos) {
  int rank, file, sq;

  printf("\n\nSquares attacked by: %c\n", tbl_side_char[side]);
  for (rank = RANK_8; rank >= RANK_1; --rank) {
    for (file = FILE_A; file <= FILE_H; ++file) {
      sq = FR2SQ(file, rank);
      if (ce_is_square_attacked(sq, side, pos) == TRUE) {
        printf("X");
      } else {
        printf("-");
      }
    }
    printf("\n");
  }
  printf("\n\n");
}

/**
 * Chess Engine diagnostic function to print out the provided integer value
 * as a binary digit.
 * @param val The value to print.
 */
void ce_print_binary(int val) {
  const int size = sizeof(int) * 8;
  int index = 0;

  for (index = size - 1; index >= 0; --index) {
    if ((1 << index) & val) {
      printf("1");
    } else {
      printf("0");
    }
    if (((index % 4) == 0)) {
      printf(" ");
    }
  }
}

