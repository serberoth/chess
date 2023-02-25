
#include "defs.h"
#include "xoshiro256.h"

// NOTE: We have removed this in favor of using the xoshiro256 prng
// #define RAND_64		((uint64_t) rand() | \
// 			((uint64_t) rand() << 15) | \
// 			((uint64_t) rand() << 30) | \
// 			((uint64_t) rand() << 45) | \
// 			(((uint64_t) rand() & 0xf) << 60))

/* GLOBALS */

int32_t tbl_sq120_to_sq64[NUM_BRD_SQ];
int32_t tbl_sq64_to_sq120[64];

uint64_t tbl_set_mask[64];
uint64_t tbl_clear_mask[64];

uint64_t tbl_piece_keys[13][120];
uint64_t side_key;
uint64_t tbl_castle_keys[16];

int32_t tbl_files_board[NUM_BRD_SQ];
int32_t tbl_ranks_board[NUM_BRD_SQ];

/* FUNCTIONS */

/**
 * Chess Engine initialization function that generates the 120 square
 * and 64 square lookup conversion tables.
 * [INTERNAL]
 */
static void _ce_init_tbl_sq120_to_sq64() {
  for (size_t index = 0; index < NUM_BRD_SQ; ++index) {
    tbl_sq120_to_sq64[index] = 65;
  }

  for (size_t index = 0; index < 64; ++index) {
    tbl_sq64_to_sq120[index] = NUM_BRD_SQ;
  }

  int32_t sq64 = 0;
  for (int32_t rank = RANK_1; rank <= RANK_8; ++rank) {
    for (int32_t file = FILE_A; file <= FILE_H; ++file) {
      int32_t sq = FR2SQ(file, rank);
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
  for (size_t index = 0; index < 64; ++index) {
    tbl_set_mask[index] = 0ULL;
    tbl_clear_mask[index] = 0ULL;
  }

  for (size_t index = 0; index < 64; ++index) {
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
  struct xoshiro256_s state = { 0 };

  xoshiro256_init(&state, 0xdeadc0deba5eb411ull);
  xoshiro256_long_jump(&state);

  for (size_t index = 0; index < 13; ++index) {
    for (size_t index2 = 0; index2 < 120; ++index2) {
      tbl_piece_keys[index][index2] = xoshiro256_next_long(&state);
    }
  }

  side_key = xoshiro256_next_long(&state);

  for (size_t index = 0; index < 16; ++index) {
    tbl_castle_keys[index] = xoshiro256_next_long(&state);
  }
}

/**
 * Chess Engine initialization function that generates the file and rank
 * board position lookup tables.
 * [INTERNAL]
 */
static void _ce_init_files_ranks_boards() {
  for (size_t index = 0; index < NUM_BRD_SQ; ++index) {
    tbl_files_board[index] = tbl_ranks_board[index] = OFFBOARD;
  }

  for (int32_t rank = RANK_1; rank <= RANK_8; ++rank) {
    for (int32_t file = FILE_A; file <= FILE_H; ++file) {
      int32_t sq = FR2SQ(file, rank);
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

