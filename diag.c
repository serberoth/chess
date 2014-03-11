
#include "defs.h"

void _ce_diag_print_tbl_files_ranks_boards() {
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

void _ce_diag_print_sq120_to_sq64() {
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

void ce_diag_print_tbls() {
  _ce_diag_print_sq120_to_sq64();
  _ce_diag_print_tbl_files_ranks_boards();
}

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

void ce_print_board(const struct board_s *pos) {
  int file, rank, piece;

  for (rank = RANK_8; rank >= RANK_1; --rank) {
    printf("%d  ", rank + 1);
    for (file = FILE_A; file <= FILE_H; ++file) {
      int sq = FR2SQ(file, rank);
      piece = pos->pieces[sq];
      printf("%3c", tbl_piece_char[piece]);
    }
    printf("\n");
  }

  printf("\n   ");
  for (file = FILE_A; file <= FILE_H; ++file) {
    printf("%3c", 'a' + file);
  }
  printf("\n");
  printf("side: %c\n", tbl_side_char[pos->side]);
  printf("enPassent: %d\n", pos->enPassent);
  printf("castle: %c%c%c%c\n",
    ((pos->castlePerms & WKCA) ? 'K' : '-'),
    ((pos->castlePerms & WQCA) ? 'Q' : '-'),
    ((pos->castlePerms & BKCA) ? 'k' : '-'),
    ((pos->castlePerms & BQCA) ? 'q' : '-'));
  printf("position: %llX\n", pos->positionKey);
  printf("\n");
}

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

