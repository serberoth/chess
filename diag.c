
#include "defs.h"

void ce_diag_tbl_print() {
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
  printf("\n");
}

void ce_print_bitboard(U64 board) {
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

