
#include <stdio.h>
#include "defs.h"

// bit shift mask to table position
// converts the rank and file index from the bottom up
const int tbl_bits[64] = {
  63, 30,  3, 32, 25, 41, 22, 33,
  15, 50, 42, 13, 11, 53, 19, 34,
  61, 29,  2, 51, 21, 43, 45, 10,
  18, 47,  1, 54,  9, 57,  0, 35,
  62, 31, 40,  4, 49,  5, 52, 26,
  60,  6, 23, 44, 46, 27, 56, 16,
   7, 39, 48, 24, 59, 14, 12, 55,
  38, 28, 58, 20, 37, 17, 36,  8
};

int ce_pop_bit(U64 *board) {
  U64 b = *board ^ (*board - 1);
  unsigned int fold = (unsigned) ((b & 0xffffffff) ^ (b >> 32));
  *board &= (*board - 1);
  return tbl_bits[(fold * 0x783a9b23) >> 26];
}

int ce_count_bits(U64 board) {
  int count;
  for (count = 0; board; count++, board &= board - 1);
  return count;
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

