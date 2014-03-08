
#include "defs.h"

int main() {
  U64 board = 0ULL;
  int index;

  ce_init();

  // ce_diag_tbl_print();

  for (index = 0; index < 64; ++index) {
    printf("Index: %d\n", index);
    ce_print_bitboard(tbl_clear_mask[index]);
    printf("\n");
  }

  SETBIT(board, 61);
  ce_print_bitboard(board);

  CLRBIT(board, 61);
  ce_print_bitboard(board);

  return 0;
}

