
#include "defs.h"

#define ENDGAME_MAT     (1 * tbl_piece_val[wR] + 2 * tbl_piece_val[wN] + 2 * tbl_piece_val[wP])

static const int32_t pawn_isolated = -10;
static const int32_t pawn_passed[8] = { 0, 5, 10, 20, 35, 60, 100, 200 };
static const int32_t rook_open_file = 10;
static const int32_t rook_semi_open_file = 5;
static const int32_t queen_open_file = 5;
static const int32_t queen_semi_open_file = 3;
static const int32_t bishop_pair = 30;

static const int32_t tbl_pawn[64] = {
  0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
  10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
  5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
  0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
  5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
  10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
  20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
  0	,	0	,	0	,	0	,	0	,	0	,	0	,	0 ,
};

static const int32_t tbl_knight[64] = {
  0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
  0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
  0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
  0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
  5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
  5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
  0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
  0	,	0	,	0	,	0	,	0	,	0	,	0	,	0 ,
};

static const int32_t tbl_bishop[64] = {
  0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
  0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
  0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
  0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
  0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
  0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
  0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
  0	,	0	,	0	,	0	,	0	,	0	,	0	,	0 ,
};

static const int32_t tbl_rook[64] = {
  0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
  0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
  0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
  0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
  0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
  0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
  25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
  0	,	0	,	5	,	10	,	10	,	5	,	0	,	0 ,
};

static const int32_t tbl_queen[64] = {
  0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
};

static const int32_t tbl_king[64] = {
  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
};
static const int32_t tbl_king_opening[64] = {
  0, 5, 5, -10, -10, 0, 10, 5,
  -10, -10, -10, -10, -10, -10, -10, -10,
  -30, -30, -30, -30, -30, -30, -30, -30,
  -70, -70, -70, -70, -70, -70, -70, -70,
  -70, -70, -70, -70, -70, -70, -70, -70,
  -70, -70, -70, -70, -70, -70, -70, -70,
  -70, -70, -70, -70, -70, -70, -70, -70,
  -70, -70, -70, -70, -70, -70, -70, -70,
};
static const int32_t tbl_king_endgame[64] = {
  -50, -10, 0, 0, 0, 0, -10, -50,
  -10, 0, 10, 10, 10, 10, 0, -10,
  0, 10, 15, 15, 15, 15, 10, 0,
  0, 10, 15, 20, 20, 15, 10, 0,
  0, 10, 15, 20, 20, 15, 10, 0,
  0, 10, 15, 15, 15, 15, 10, 0,
  -10, 0, 10, 10, 10, 10, 0, -10,
  -50, -10, 0, 0, 0, 0, -10, -50,
};

static const int32_t *tbl_evals[13] = {
  NULL, tbl_pawn, tbl_knight, tbl_bishop, tbl_rook, tbl_queen, tbl_king, tbl_pawn, tbl_knight, tbl_bishop, tbl_rook, tbl_queen, tbl_king,
};
static const uint64_t *tbl_passed_pawn[2] = { tbl_white_passed_mask, tbl_black_passed_mask, };

static bool _ce_material_draw(const struct board_s *pos) {
  // 8/6R1/2k5/6P1/8/8/4nP2/6K1 w - - 1 41
  if (pos->pieceNum[wR] == 0
      && pos->pieceNum[bR] == 0
      && pos->pieceNum[wQ] == 0
      && pos->pieceNum[bQ] == 0) {
    if (pos->pieceNum[wB] == 0 && pos->pieceNum[bB] == 0) {
      if (pos->pieceNum[wN] < 3 && pos->pieceNum[bN] < 3) {
        return true;
      }
    } else if (pos->pieceNum[bN] == 0
        && pos->pieceNum[wN] == 0) {
      if (abs(pos->pieceNum[wB] - pos->pieceNum[bB]) < 2) {
        return true;
      }
    } else if ((pos->pieceNum[wN] < 3 && pos->pieceNum[wB] == 0)
        || (pos->pieceNum[wB] == 1 && pos->pieceNum[wN] == 0)) {
      if ((pos->pieceNum[bN] < 3 && pos->pieceNum[bB] == 0) || (pos->pieceNum[bB] == 1 && pos->pieceNum[bN] == 0)) {
        return true;
      }
    }
  } else if (pos->pieceNum[wQ] == 0 && pos->pieceNum[bQ] == 0) {
    if (pos->pieceNum[wR] == 1 && pos->pieceNum[bR] == 1) {
      if ((pos->pieceNum[wN] + pos->pieceNum[wB]) < 2 && (pos->pieceNum[bN] + pos->pieceNum[bB]) < 2) {
        return true;
      }
    } else if (pos->pieceNum[wR] == 1 && pos->pieceNum[bR]) {
      if ((pos->pieceNum[wN] + pos->pieceNum[wB]) == 0
          && (((pos->pieceNum[bN] + pos->pieceNum[bB]) == 1) || ((pos->pieceNum[bN] + pos->pieceNum[bB]) == 2))) {
        return true;
      }
    } else if (pos->pieceNum[bR] == 1 && pos->pieceNum[wR]) {
      if ((pos->pieceNum[bN] + pos->pieceNum[bB]) == 0
          && (((pos->pieceNum[wN] + pos->pieceNum[wB]) == 1) || ((pos->pieceNum[wN] + pos->pieceNum[wB]) == 2))) {
        return true;
      }
    }
  }
  return false;
}
/**
 * Chess Engine function to evaluate the current board position and return a
 * weighted score for that position.
 * @param pos A pointer to a board position structure with the current board
 *    position.
 * @return The weighted score for the provided board position.
 */
int32_t ce_eval_position(const struct board_s *pos) {
  int32_t score = pos->material[WHITE] - pos->material[BLACK];

  if (pos->pieceNum[wP] == 0 && pos->pieceNum[bP] == 0 && _ce_material_draw(pos)) {
    return 0;
  }

  for (int32_t pce = wP; pce <= bK; ++pce) {
    int32_t colour = tbl_piece_col[pce];
    for (int32_t pceNum = 0; pceNum < pos->pieceNum[pce]; ++pceNum) {
      int32_t sq = pos->pieceList[pce][pceNum];
      ASSERT(ce_valid_square(sq));

      if (colour == WHITE) {
        score += tbl_evals[pce][SQ64(sq)];
      } else if (colour == BLACK) {
        score -= tbl_evals[pce][MIR64(SQ64(sq))];
      }

      // Pawn specific move evaluations
      if (pce == wP || pce == bP) {
        // Calculate a score for isolated pawns
        if ((tbl_isolated_mask[SQ64(sq)] & pos->pawns[colour]) == 0) {
          // printf("%cP Isolated: %s\n", (colour == WHITE ? 'w' :'b'), ce_print_sq(sq));
          // If the colour is black `subtract` the score
          if (colour == WHITE) {
            score += pawn_isolated;
          } else if (colour == BLACK) {
            score -= pawn_isolated;
          }
        }
        // Calculate a score for passed pawns
        if ((tbl_passed_pawn[colour][SQ64(sq)] & pos->pawns[colour ^ 1]) == 0) {
          // printf("%cP Passed: %s\n", (colour == WHITE ? 'w' : 'b'), ce_print_sq(sq));
          if (colour == WHITE) {
            score += pawn_passed[tbl_ranks_board[sq]];
          } else if (colour == BLACK) {
            score -= pawn_passed[7 - tbl_ranks_board[sq]];
          }
        }
      }

      // Rook specific evaluations
      if (pce == wR || pce == bR) {
        if ((pos->pawns[BOTH] & tbl_file_bb_mask[tbl_files_board[sq]]) == 0) {
          if (colour == WHITE) {
            score += rook_open_file;
          } else if (colour == BLACK) {
            score -= rook_open_file;
          }
        } else if ((pos->pawns[colour] & tbl_file_bb_mask[tbl_files_board[sq]]) == 0) {
          if (colour == WHITE) {
            score += rook_semi_open_file;
          } else if (colour == BLACK) {
            score -= rook_semi_open_file;
          }
        }
      }

      // Queen specific evauations
      if (pce == wQ || pce == bQ) {
        if ((pos->pawns[BOTH] & tbl_file_bb_mask[tbl_files_board[sq]]) == 0) {
          if (colour == WHITE) {
            score += queen_open_file;
          } else if (colour == BLACK) {
            score -= queen_open_file;
          }
        } else if ((pos->pawns[colour] & tbl_file_bb_mask[tbl_files_board[sq]]) == 0) {
          if (colour == WHITE) {
            score += queen_semi_open_file;
          } else if (colour == BLACK) {
            score -= queen_semi_open_file;
          }
        }

      }
    }

    if (pce == wK || pce == bK) {
      int32_t sq = pos->pieceList[pce][0];

      if (pos->material[colour ^ 1] <= ENDGAME_MAT) {
        score += colour == WHITE ? tbl_king_endgame[SQ64(sq)] : -tbl_king_endgame[MIR64(SQ64(sq))];
      } else {
        score += colour == WHITE ? tbl_king_opening[SQ64(sq)] : -tbl_king_opening[MIR64(SQ64(sq))];
      }
    }
  }

  if (pos->pieceNum[wB] >= 2) { score += bishop_pair; }
  if (pos->pieceNum[bB] >= 2) { score -= bishop_pair; }

  if (pos->side == BLACK) {
    score = -score;
  }

  return score;
}
