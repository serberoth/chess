
#include "defs.h"

#define MOVE(a, t, c, p, fl)		((a) | ((t) << 7) | ((c) << 14) | ((p) << 20) | (fl))
#define SQOFFBOARD(sq)			(tbl_files_board[(sq)] == OFFBOARD)

const int tbl_loop_slide_pce[8] = { wB, wR, wQ, 0, bB, bR, bQ, 0 };
const int tbl_loop_slide_index[2] = { 0, 4 };

const int tbl_loop_non_slide_pce[6] = { wN, wK, 0, bN, bK, 0 };
const int tbl_loop_non_slide_index[2] = { 0, 3 };

const int tbl_piece_dir[13][8] = {
  { 0 },
  { 0 },
  { -8, -19, -21, -12,  8,  19, 21, 12 },
  { -9, -11,  11,   9,  0 },
  { -1, -10,   1,  10,  0 },
  { -1, -10,   1,  10, -9, -11, 11, 9 },
  { -1, -10,   1,  10, -9, -11, 11, 9 },
  { 0 },
  { -8, -19, -21, -12,  8,  19, 21, 12 },
  { -9, -11,  11,   9,  0 },
  { -1, -10,   1,  10,  0 },
  { -1, -10,   1,  10, -9, -11, 11, 9 },
  { -1, -10,   1,  10, -9, -11, 11, 9 },
};
const int tbl_piece_dir_num[13] = { 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8 };

/*
ce_move_gen(struct board_s *board, struct move_list_s *list)
  for each piece
    slider loop each direction add move
      add move { list->moves[list->count] = move; list->count++ }
 */

static void _ce_add_quiet_move(const struct board_s *pos, int move, struct move_list_s *list) {
  list->moves[list->count].move = move;
  list->moves[list->count].score = 0;
  list->count++;
}

static void _ce_add_capture_move(const struct board_s *pos, int move, struct move_list_s *list) {
  list->moves[list->count].move = move;
  list->moves[list->count].score = 0;
  list->count++;
}

static void _ce_add_enpassent_move(const struct board_s *pos, int move, struct move_list_s *list) {
  list->moves[list->count].move = move;
  list->moves[list->count].score = 0;
  list->count++;
}

// TODO: Clean up these pawn move methods there is much duplication
static void _ce_add_white_pawn_capture_move(const struct board_s *pos, const int at, const int to, const int cap, struct move_list_s *list) {
  ASSERT(ce_valid_piece_empty(cap));
  ASSERT(ce_valid_square(at));
  ASSERT(ce_valid_square(to));

  if (tbl_ranks_board[at] == RANK_7) {
    _ce_add_capture_move(pos, MOVE(at, to, cap, wQ, 0), list);
    _ce_add_capture_move(pos, MOVE(at, to, cap, wR, 0), list);
    _ce_add_capture_move(pos, MOVE(at, to, cap, wB, 0), list);
    _ce_add_capture_move(pos, MOVE(at, to, cap, wN, 0), list);
  } else {
    _ce_add_capture_move(pos, MOVE(at, to, cap, EMPTY, 0), list);
  }
}

static void _ce_add_white_pawn_move(const struct board_s *pos, const int at, const int to, struct move_list_s *list) {
  ASSERT(ce_valid_square(at));
  ASSERT(ce_valid_square(to));

  if (tbl_ranks_board[at] == RANK_7) {
    _ce_add_quiet_move(pos, MOVE(at, to, EMPTY, wQ, 0), list);
    _ce_add_quiet_move(pos, MOVE(at, to, EMPTY, wR, 0), list);
    _ce_add_quiet_move(pos, MOVE(at, to, EMPTY, wB, 0), list);
    _ce_add_quiet_move(pos, MOVE(at, to, EMPTY, wN, 0), list);
  } else {
    _ce_add_quiet_move(pos, MOVE(at, to, EMPTY, EMPTY, 0), list);
  }
}

static void _ce_add_black_pawn_capture_move(const struct board_s *pos, const int at, const int to, const int cap, struct move_list_s *list) {
  ASSERT(ce_valid_piece_empty(cap));
  ASSERT(ce_valid_square(at));
  ASSERT(ce_valid_square(to));

  if (tbl_ranks_board[at] == RANK_2) {
    _ce_add_capture_move(pos, MOVE(at, to, cap, bQ, 0), list);
    _ce_add_capture_move(pos, MOVE(at, to, cap, bR, 0), list);
    _ce_add_capture_move(pos, MOVE(at, to, cap, bB, 0), list);
    _ce_add_capture_move(pos, MOVE(at, to, cap, bN, 0), list);
  } else {
    _ce_add_capture_move(pos, MOVE(at, to, cap, EMPTY, 0), list);
  }
}

static void _ce_add_black_pawn_move(const struct board_s *pos, const int at, const int to, struct move_list_s *list) {
  ASSERT(ce_valid_square(at));
  ASSERT(ce_valid_square(to));

  if (tbl_ranks_board[at] == RANK_2) {
    _ce_add_quiet_move(pos, MOVE(at, to, EMPTY, bQ, 0), list);
    _ce_add_quiet_move(pos, MOVE(at, to, EMPTY, bR, 0), list);
    _ce_add_quiet_move(pos, MOVE(at, to, EMPTY, bB, 0), list);
    _ce_add_quiet_move(pos, MOVE(at, to, EMPTY, bN, 0), list);
  } else {
    _ce_add_quiet_move(pos, MOVE(at, to, EMPTY, EMPTY, 0), list);
  }
}

void ce_generate_all_moves(const struct board_s *pos, struct move_list_s *list) {
  int pce = EMPTY;
  int side = pos->side;
  int sq = 0;
  int t_sq = 0;
  int pceNum = 0;
  int dir = 0;
  int index = 0;
  int pceIndex = 0;

  CHKBRD(pos);

  list->count = 0;

  if (side == WHITE) {
    for (pceNum = 0; pceNum < pos->pieceNum[wP]; ++pceNum) {
      sq = pos->pieceList[wP][pceNum];

      ASSERT(ce_valid_square(sq));

      // handle standard pawn move
      if (pos->pieces[sq + 10] == EMPTY) {
        _ce_add_white_pawn_move(pos, sq, sq + 10, list);
        if (tbl_ranks_board[sq] == RANK_2 && pos->pieces[sq + 20] == EMPTY) {
          _ce_add_quiet_move(pos, MOVE(sq, sq + 20, EMPTY, EMPTY, MFLAGPS), list);
        }
      }

      // handle pawn capture
      if (!SQOFFBOARD(sq + 9) && tbl_piece_col[pos->pieces[sq + 9]] == BLACK) {
        _ce_add_white_pawn_capture_move(pos, sq, sq + 9, pos->pieces[sq + 9], list);
      }
      if (!SQOFFBOARD(sq + 11) && tbl_piece_col[pos->pieces[sq + 11]] == BLACK) {
        _ce_add_white_pawn_capture_move(pos, sq, sq + 11, pos->pieces[sq + 11], list);
      }

      // handle en-passent
      if (sq + 9 == pos->enPassent) {
        _ce_add_capture_move(pos, MOVE(sq, sq + 9, EMPTY, EMPTY, MFLAGEP), list);
      }
      if (sq + 11 == pos->enPassent) {
        _ce_add_capture_move(pos, MOVE(sq, sq + 11, EMPTY, EMPTY, MFLAGEP), list);
      }
    }

    // king side castling
    if (pos->castlePerms & WKCA) {
      if (pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY) {
        if (!ce_is_square_attacked(E1, BLACK, pos) && !ce_is_square_attacked(F1, BLACK, pos)) {
          _ce_add_quiet_move(pos, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA), list);
        }
      }
    }

    // queen side castling
    if (pos->castlePerms & WQCA) {
      if (pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1] == EMPTY) {
        if (!ce_is_square_attacked(E1, BLACK, pos) && !ce_is_square_attacked(D1, BLACK, pos)) {
          _ce_add_quiet_move(pos, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA), list);
        }
      }
    }
  } else if (side == BLACK) {
    for (pceNum = 0; pceNum < pos->pieceNum[bP]; ++pceNum) {
      sq = pos->pieceList[bP][pceNum];

      ASSERT(ce_valid_square(sq));

      // handle standard pawn move
      if (pos->pieces[sq - 10] == EMPTY) {
        _ce_add_black_pawn_move(pos, sq, sq - 10, list);
        if (tbl_ranks_board[sq] == RANK_7 && pos->pieces[sq - 20] == EMPTY) {
          _ce_add_quiet_move(pos, MOVE(sq, sq - 20, EMPTY, EMPTY, MFLAGPS), list);
        }
      }

      // handle pawn capture
      if (!SQOFFBOARD(sq - 9) && tbl_piece_col[pos->pieces[sq - 9]] == WHITE) {
        _ce_add_black_pawn_capture_move(pos, sq, sq - 9, pos->pieces[sq - 9], list);
      }
      if (!SQOFFBOARD(sq - 11) && tbl_piece_col[pos->pieces[sq - 11]] == WHITE) {
        _ce_add_black_pawn_capture_move(pos, sq, sq - 11, pos->pieces[sq - 11], list);
      }

      // handle en-passent
      if (sq - 9 == pos->enPassent) {
        _ce_add_capture_move(pos, MOVE(sq, sq - 9, EMPTY, EMPTY, MFLAGEP), list);
      }
      if (sq - 11 == pos->enPassent) {
        _ce_add_capture_move(pos, MOVE(sq, sq - 11, EMPTY, EMPTY, MFLAGEP), list);
      }
    }

    // king side castling
    if (pos->castlePerms & BKCA) {
      if (pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY) {
        if (!ce_is_square_attacked(E8, WHITE, pos) && !ce_is_square_attacked(F8, WHITE, pos)) {
          _ce_add_quiet_move(pos, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCA), list);
        }
      }
    }

    // queen side castling
    if (pos->castlePerms & BQCA) {
      if (pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8] == EMPTY) {
        if (!ce_is_square_attacked(E8, WHITE, pos) && !ce_is_square_attacked(D8, WHITE, pos)) {
          _ce_add_quiet_move(pos, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA), list);
        }
      }
    }
  }

  /* sliding pieces (bishop, rook, and queen) */
  pceIndex = tbl_loop_slide_index[side];
  while ((pce = tbl_loop_slide_pce[pceIndex++]) != 0) {
    ASSERT(ce_valid_piece(pce));

    for (pceNum = 0; pceNum < pos->pieceNum[pce]; ++pceNum) {
      sq = pos->pieceList[pce][pceNum];
      ASSERT(ce_valid_square(sq));

      for (index = 0; index < tbl_piece_dir_num[pce]; ++index) {
        dir = tbl_piece_dir[pce][index];
        t_sq = sq + dir;

        while (!SQOFFBOARD(t_sq)) {
          if (pos->pieces[t_sq] != EMPTY) {
            if (tbl_piece_col[pos->pieces[t_sq]] == (side ^ 1)) {
              _ce_add_capture_move(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
            }   
            break;
          }
          _ce_add_quiet_move(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
          t_sq += dir;
        }
      }   
    }
  }

  /* non-sliding pieces (knight, king) */
  pceIndex = tbl_loop_non_slide_index[side];
  while ((pce = tbl_loop_non_slide_pce[pceIndex++]) != 0) {
    ASSERT(ce_valid_piece(pce));

    for (pceNum = 0; pceNum < pos->pieceNum[pce]; ++pceNum) {
      sq = pos->pieceList[pce][pceNum];
      ASSERT(ce_valid_square(sq));

      for (index = 0; index < tbl_piece_dir_num[pce]; ++index) {
        dir = tbl_piece_dir[pce][index];
        t_sq = sq + dir;

        if (SQOFFBOARD(t_sq)) {
          continue;
        }

        if (pos->pieces[t_sq] != EMPTY) {
          if (tbl_piece_col[pos->pieces[t_sq]] == (side ^ 1)) {
            _ce_add_capture_move(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
          }
          continue;
        }
        _ce_add_quiet_move(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
      }
    }
  }
}

int ce_move_exists(struct board_s *pos, const int move) {
  struct move_list_s list = { 0 };
  int index = 0;

  ce_generate_all_moves(pos, &list);

  for (index = 0; index < list.count; ++index) {
    if (!ce_make_move(pos, list.moves[index].move)) {
      continue;
    }

    ce_take_move(pos);

    if (list.moves[index].move == move) {
      return TRUE;
    }
  }

  return FALSE;
}

  
