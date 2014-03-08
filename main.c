
#include <stdio.h>
#include "defs.h"

int main() {
  U64 board = 0ULL;
  int count, index;

  ce_init();

  // ce_diag_tbl_print();

  board |= (1ULL << SQ64(D2));
  board |= (1ULL << SQ64(D3));
  board |= (1ULL << SQ64(D4));

  ce_print_bitboard(board);

  count = CNT(board);

  printf("Count: %d\n", count);

  index = POP(&board);
  printf("Index: %d\n", index);
  ce_print_bitboard(board);
  count = CNT(board);
  printf("Count: %d\n", count);

  return 0;
}

