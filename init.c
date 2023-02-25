
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

uint64_t tbl_file_bb_mask[8];
uint64_t tbl_rank_bb_mask[8];

uint64_t tbl_black_passed_mask[64];
uint64_t tbl_white_passed_mask[64];
uint64_t tbl_isolated_mask[64];

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
 * Chess Engine initialization function that generates the file and rank
 * evaluation masks lookup tables.
 * [INTERNAL]
 */
static void _ce_init_tbl_eval_masks() {
  for (size_t index = 0; index < 8; ++index) {
    tbl_file_bb_mask[index] = 0ull;
    tbl_rank_bb_mask[index] = 0ull;
  }

  for (int32_t r = RANK_8; r >= RANK_1; --r) {
    for (int32_t f = FILE_A; f <= FILE_H; ++f) {
      int32_t offset = r * 8 + f;
      tbl_file_bb_mask[f] |= (1ull << offset);
      tbl_rank_bb_mask[r] |= (1ull << offset);
    }
  }

  for (int32_t sq = 0; sq < 64; ++sq) {
    tbl_black_passed_mask[sq] = 0ull;
    tbl_white_passed_mask[sq] = 0ull;
    tbl_isolated_mask[sq] = 0ull;
    int32_t offset = 0;
    
    for (offset = sq + 8; offset < 64; offset += 8) {
      tbl_white_passed_mask[sq] |= (1ull << offset);
    }

    for (offset = sq - 8; offset >= 0; offset -= 8) {
      tbl_black_passed_mask[sq] |= (1ull << offset);
    }

    if (tbl_files_board[SQ120(sq)] > FILE_A) {
      tbl_isolated_mask[sq] |= tbl_file_bb_mask[tbl_files_board[SQ120(sq)] - 1];

      for (offset = sq + 7; offset < 64; offset += 8) {
        tbl_white_passed_mask[sq] |= (1ull << offset);
      }

      for (offset = sq - 9; offset >= 0; offset -= 8) {
        tbl_black_passed_mask[sq] |= (1ull << offset);
      }
    }

    if (tbl_files_board[SQ120(sq)] < FILE_H) {
      tbl_isolated_mask[sq] |= tbl_file_bb_mask[tbl_files_board[SQ120(sq)] + 1];

      for (offset = sq + 9; offset < 64; offset += 8) {
        tbl_white_passed_mask[sq] |= (1ull << offset);
      }

      for (offset = sq - 7; offset >= 0; offset -= 8) {
        tbl_black_passed_mask[sq] |= (1ull << offset);
      }
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
  _ce_init_tbl_eval_masks();
  ce_init_mvv_lva();

}

