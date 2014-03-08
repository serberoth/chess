
#include "defs.h"

int tbl_sq120_to_sq64[NUM_BRD_SQ];
int tbl_sq64_to_sq120[64];

static void _ce_init_tbl_sq120_to_sq64() {
  int index, file, rank;
  int sq64 = 0;

  for (index = 0; index < NUM_BRD_SQ; ++index) {
    tbl_sq120_to_sq64[index] = 65;
  }

  for (index = 0; index < 64; ++index) {
    tbl_sq64_to_sq120[index] = NUM_BRD_SQ;
  }

  for (rank = RANK_1; rank <= RANK_8; ++rank) {
    for (file = FILE_A; file <= FILE_H; ++file) {
      int sq = FR2SQ(file, rank);
      tbl_sq64_to_sq120[sq64] = sq;
      tbl_sq120_to_sq64[sq] = sq64++;
    }
  }
}

void ce_init() {
  _ce_init_tbl_sq120_to_sq64();

}

