
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

/**
 * Chess Engine function that pops a bit from the provided bit-board.
 * @param board A pointer to the bit-board.
 * @return The square of the bit that was poped from the bit-board.
 */
int ce_pop_bit(U64 *board) {
  U64 b = *board ^ (*board - 1);
  unsigned int fold = (unsigned) ((b & 0xffffffff) ^ (b >> 32));
  *board &= (*board - 1);
  return tbl_bits[(fold * 0x783a9b23) >> 26];
}

/**
 * Chess Engine function that counts the number of bits on the
 * provided bit-board.
 * @param board The bit board value.
 * @return The count of active bits on the provided bit-board.
 */
int ce_count_bits(U64 board) {
  int count;
  for (count = 0; board; count++, board &= board - 1);
  return count;
}

