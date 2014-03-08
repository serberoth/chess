
#include <stdio.h>
#include "defs.h"

int main() {
  U64 board = 0ULL;

  ce_init();

  // ce_diag_tbl_print();
  printf("Start:\n");
  ce_print_bitboard(board);  

  board |= (1ULL << SQ64(D2));

  ce_print_bitboard(board);

  board |= (1ULL << SQ64(G2));

  ce_print_bitboard(board);

  return 0;
}

