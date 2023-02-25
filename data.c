
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
