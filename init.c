
#include "defs.h"

#define RAND_64		((U64) rand() | \
			((U64) rand() << 15) | \
			((U64) rand() << 30) | \
			((U64) rand() << 45) | \
			(((U64) rand() & 0xf) << 60))

/* GLOBALS */

int tbl_sq120_to_sq64[NUM_BRD_SQ];
int tbl_sq64_to_sq120[64];

U64 tbl_set_mask[64];
U64 tbl_clear_mask[64];

U64 tbl_piece_keys[13][120];
U64 side_key;
U64 tbl_castle_keys[16];

int tbl_files_board[NUM_BRD_SQ];
int tbl_ranks_board[NUM_BRD_SQ];

/* FUNCTIONS */

/**
 * Chess Engine initialization function that generates the 120 square
 * and 64 square lookup conversion tables.
 * [INTERNAL]
 */
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

/**
 * Chess Engine initialization function that generates the position
 * bit-mask lookup tables.
 * [INTERNAL]
 */
static void _ce_init_tbl_bit_masks() {
  int index;

  for (index = 0; index < 64; ++index) {
    tbl_set_mask[index] = 0ULL;
    tbl_clear_mask[index] = 0ULL;
  }

  for (index = 0; index < 64; ++index) {
    tbl_set_mask[index] |= (1ULL << index);
    tbl_clear_mask[index] = ~tbl_set_mask[index];
  }
}

/**
 * Chess Engine initialization function that generates the piece and castle
 * hash key lookup tables.
 * [INTERNAL]
 */
static void _ce_init_hash_keys() {
  int index, index2;

  for (index = 0; index < 13; ++index) {
    for (index2 = 0; index2 < 120; ++index2) {
      tbl_piece_keys[index][index2] = RAND_64;
    }
  }

  side_key = RAND_64;

  for (index = 0; index < 16; ++index) {
    tbl_castle_keys[index] = RAND_64;
  }
}

/**
 * Chess Engine initialization function that generates the file and rank
 * board position lookup tables.
 * [INTERNAL]
 */
static void _ce_init_files_ranks_boards() {
  int index, file, rank;
  int sq64;

  for (index = 0; index < NUM_BRD_SQ; ++index) {
    tbl_files_board[index] = tbl_ranks_board[index] = OFFBOARD;
  }

  for (rank = RANK_1; rank <= RANK_8; ++rank) {
    for (file = FILE_A; file <= FILE_H; ++file) {
      int sq = FR2SQ(file, rank);
      tbl_files_board[sq] = file;
      tbl_ranks_board[sq] = rank;
    }
  }
}

/**
 * Chess Engine initialization function that generates the various
 * board position and hash function lookup tables required by the
 * engine to evaluate and generate board positions.
 */
void ce_init() {
  _ce_init_tbl_sq120_to_sq64();
  _ce_init_tbl_bit_masks();
  _ce_init_hash_keys();
  _ce_init_files_ranks_boards();
  ce_init_mvv_lva();

}

