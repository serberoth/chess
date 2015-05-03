
#include "defs.h"

static const int tbl_pawn[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

static const int tbl_knight[64] = {
0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

static const int tbl_bishop[64] = {
0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

static const int tbl_rook[64] = {
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0
};

static const int tbl_queen[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,
0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,
0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,
0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,
0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,
0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,
0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0
};

static const int tbl_king[64] = {
0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,
0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,
0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,
0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,
0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,
0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,
0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,
0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0       ,       0
};

static const int *tbl_evals[13] = {
  NULL, tbl_pawn, tbl_knight, tbl_bishop, tbl_rook, tbl_queen, tbl_king, tbl_pawn, tbl_knight, tbl_bishop, tbl_rook, tbl_queen, tbl_king
};

static const int tbl_mirror64[64] = {
56,	57,	58,	59,	60,	61,	62,	63,
48,	49,	50,	51,	52,	53,	54,	55,
40,	41,	42,	43,	44,	45,	46,	47,
32,	33,	34,	35,	36,	37,	38,	39,
24,	25,	26,	27,	28,	29,	30,	31,
16,	17,	18,	19,	20,	21,	22,	23,
8,	9,	10,	11,	12,	13,	14,	15,
0,	1,	2,	3,	4,	5,	6,	7
};

#define MIR64(sq)	(tbl_mirror64[(sq)])

/**
 * Chess Engine function to evaluate the current board position and return a
 * weighted score for that position.
 * @param pos A pointer to a board position structure with the current board
 *    position.
 * @return The weighted score for the provided board position.
 */
int ce_eval_position(const struct board_s *pos) {
  int pce, pceNum;
  int score = pos->material[WHITE] - pos->material[BLACK];

  for (pce = wP; pce <= bK; ++pce) {
    int colour = tbl_piece_col[pce];
    for (pceNum = 0; pceNum < pos->pieceNum[pce]; ++pceNum) {
      int sq = pos->pieceList[pce][pceNum];
      ASSERT(ce_valid_square(sq));

      if (colour == WHITE) {
        score += tbl_evals[pce][SQ64(sq)];
      } else if (colour == BLACK) {
        score -= tbl_evals[pce][MIR64(SQ64(sq))];
      }
    }
  }

  if (pos->side == BLACK) {
    score = -score;
  }

  return score;
}

