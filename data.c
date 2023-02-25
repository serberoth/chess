
#include "defs.h"

const char tbl_piece_char[]     = u8".PNBRQKpnbrqk";
const char tbl_side_char[]      = u8"wb-";
const char tbl_rank_char[]      = u8"12345678";
const char tbl_file_char[]      = u8"abcdefgh";

const bool tbl_piece_big[13]            = { false, false, true, true, true, true, true, false, true, true, true, true, true };
const bool tbl_piece_maj[13]            = { false, false, false, false, true, true, true, false, false, false, true, true, true };
const bool tbl_piece_min[13]            = { false, false, true, true, false, false, false, false, true, true, false, false, false };
const int32_t tbl_piece_val[13]         = { 0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000 };
const int32_t tbl_piece_col[13]         = { BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };

const bool tbl_piece_pawn[13]           = { false, true, false, false, false, false, false, true, false, false, false, false, false };
const bool tbl_piece_knight[13]         = { false, false, true, false, false, false, false, false, true, false, false, false, false };
const bool tbl_piece_king[13]           = { false, false, false, false, false, false, true, false, false, false, false, false, true };
const bool tbl_piece_rook_queen[13]     = { false, false, false, false, true, true, false, false, false, false, true, true, false };
const bool tbl_piece_bishop_queen[13]   = { false, false, false, true, false, true, false, false, false, true, false, true, false };

const bool tbl_piece_slides[13]         = { false, false, false, true, true, true, false, false, false, true, true, true, false };

const int32_t tbl_mirror64[64]          = {
  56, 57, 58, 59, 60, 61, 62, 63,
  48, 49, 50, 51, 52, 53, 54, 55,
  40, 41, 42, 43, 44, 45, 46, 47,
  32, 33, 34, 35, 36, 37, 38, 39,
  24, 25, 26, 27, 28, 29, 30, 31,
  16, 17, 18, 19, 20, 21, 22, 23,
   8,  9, 10, 11, 12, 13, 14, 15,
   0,  1,  2,  3,  4,  5,  6,  7,
};
