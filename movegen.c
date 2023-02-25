
#include "defs.h"

#define MOVE(a, t, c, p, fl)		((a) | ((t) << 7) | ((c) << 14) | ((p) << 20) | (fl))
#define SQOFFBOARD(sq)			(tbl_files_board[(sq)] == OFFBOARD)

static const int32_t tbl_loop_slide_pce[8] = { wB, wR, wQ, 0, bB, bR, bQ, 0 };
static const int32_t tbl_loop_slide_index[2] = { 0, 4 };

static const int32_t tbl_loop_non_slide_pce[6] = { wN, wK, 0, bN, bK, 0 };
static const int32_t tbl_loop_non_slide_index[2] = { 0, 3 };

static const int32_t tbl_piece_dir[13][8] = {
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
static const int32_t tbl_piece_dir_num[13] = { 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8 };

/*
 * PV Move
 * Capture Moves -> MVVLVA or SEE
 * Killers
 * History Score
 *
 * MVVLVA (Most valuable victim, least valuable attacker)
 * SEE (Static Exchange Evaluator)
 *
 *  None, wPawn, wKnight, wBishop, wRook, wQueen, wKing, bPawn, bKnight, bBishop, bRook, bQueen, bKing
 */
static const int32_t tbl_victim_scores[13] = { 0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };
static int32_t tbl_mvv_lva_scores[13][13];

/**
 * Chess Engine initialization function that generates the most valuable victim, least valuable attacker
 * position scores lookup tables.
 */
void ce_init_mvv_lva() {
  int32_t attacker;
  int32_t victim;

  for (attacker = wP; attacker <= bK; ++attacker) {
    for (victim = wP; victim <= bK; ++victim) {
      tbl_mvv_lva_scores[victim][attacker] = tbl_victim_scores[victim] + 6 - (tbl_victim_scores[attacker] / 100);
    }   
  }
}


static int32_t _ce_add_check_flag(const struct board_s *pos, int32_t move) {
  struct board_s b = { 0 };
  union move_u m = MV(move);
  int32_t inCheck = 0;

  ASSERT(ce_valid_square(FROMSQ(move)));
  ASSERT(ce_valid_square(TOSQ(move)));

  // XXX I think this makes every movegen call that much more xpensive.
  memcpy((void *) &b, (void *) pos, sizeof(struct board_s));

  ce_move_make(&b, move);

  inCheck = ce_is_square_attacked(b.kingSq[b.side], b.side ^ 1, &b);
  // printf(u8"%s %s\n", ce_print_move(m), inCheck ? u8"check!!!" : u8"NOPE");

  ce_move_take(&b);

  m.check = inCheck;
  return m.val;
}

/*
ce_move_gen(struct board_s *board, struct move_list_s *list)
  for each piece
    slider loop each direction add move
      add move { list->moves[list->count] = move; list->count++ }
 */

/**
 * Chess Engine function to add a quite (non-capturing) move to the provided move list based on the
 * provided board position.
 * [INTERNAL]
 * @param pos A pointer to the current board position.
 * @param move The move being considered.
 * @param list A pointer to the current move list.
 */
static void _ce_add_quiet_move(const struct board_s *pos, int32_t move, struct move_list_s *list) {
  ASSERT(ce_valid_square(FROMSQ(move)));
  ASSERT(ce_valid_square(TOSQ(move)));

  move = _ce_add_check_flag(pos, move);

  list->moves[list->count].move = move;

  if (pos->searchKillers[0][pos->ply] == move) {
    list->moves[list->count].score = 900000;
  } else if (pos->searchKillers[1][pos->ply] == move) {
    list->moves[list->count].score = 800000;
  } else {
    list->moves[list->count].score = pos->searchHistory[pos->pieces[FROMSQ(move)]][TOSQ(move)];
  }

  list->count++;
}

/**
 * Chess Engine function to add a capture move to the provided move list based on the provided 
 * board position.
 * [INTERNAL]
 * @param pos A pointer to the current board position.
 * @param move The move being considered.
 * @param list A pointer to the current move list.
 */
static void _ce_add_capture_move(const struct board_s *pos, int32_t move, struct move_list_s *list) {
  move = _ce_add_check_flag(pos, move);

  list->moves[list->count].move = move;
  // add 1000000 to the score to account for the killers search
  list->moves[list->count].score = tbl_mvv_lva_scores[CAPTURED(move)][pos->pieces[FROMSQ(move)]] + 1000000;
  list->count++;
}

/**
 * Chess Engine function to add an enpassent move to the provided move list based on the provided
 * bpard position.
 * [INTERNAL]
 * @param pos A pointer to the current board position.
 * @param move The move being considered.
 * @param list A pointer to the current move list.
 */
static void _ce_add_enpassent_move(const struct board_s *pos, int32_t move, struct move_list_s *list) {
  move = _ce_add_check_flag(pos, move);

  list->moves[list->count].move = move;
  // 105 is the precalculated score from the tbl_mvv_lva_scores table
  list->moves[list->count].score = 105 + 1000000;
  list->count++;
}

/**
 * Chess Engine function to add a pawn capture move for the white pieces to the provided move
 * list based on the provided board position.
 * [INTERNAL]
 * @param pos A pointer to the current board position.
 * @param at The current square of the pawn being considered.
 * @param to The destination square of the pawn being considered after the capture.
 * @param cap The piece being evaluated for capture.
 * @param list A pointer to the current move list.
 */
// TODO: Clean up these pawn move methods there is much duplication
static void _ce_add_white_pawn_capture_move(const struct board_s *pos, const int32_t at, const int32_t to, const int32_t cap, struct move_list_s *list) {
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

/**
 * Chess Engine function to add a quite (non-capturing) pawn move for the white pieces to the provided
 * move list based on the provided board position.
 * [INTERNAL]
 * @param pos A pointer to the current board position.
 * @param at The current square of the pawn being considered.
 * @param to The destination square of the pawn being considered.
 * @param list A pointer to the current move list.
 */
static void _ce_add_white_pawn_move(const struct board_s *pos, const int32_t at, const int32_t to, struct move_list_s *list) {
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

/**
 * Chess Engine function to add a pawn capture move for the black pieces to the provided move list
 * based on the provided board position.
 * [INTERNAL]
 * @param pos A pointer to the current board position.
 * @param at The current square of the pawn being considered.
 * @param to The destination square of the pawn being considered.
 * @param cap The piece being evaluated for capture.
 * @param list A pointer to the current move list.
 */
static void _ce_add_black_pawn_capture_move(const struct board_s *pos, const int32_t at, const int32_t to, const int32_t cap, struct move_list_s *list) {
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

/**
 * Chess Engine function to add a quite (non-capturing) pawn move for the black pieces to the provided
 * move list based on the provided board position.
 * [INTERNAL]
 * @param pos A pointer to the current board position.
 * @param at The current square of the pawn being considered.
 * @param to The destination square of the pawn being considered.
 * @param list A pointer to the current move list.
 */
static void _ce_add_black_pawn_move(const struct board_s *pos, const int32_t at, const int32_t to, struct move_list_s *list) {
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

/**
 * Chess Engine generation function that generates the set of capture moves for the
 * provided board position filling in the provided move list structure.
 * @param pos A pointer to the current board position.
 * @param list A pointer to the current move list being generated.
 */
void ce_generate_capture_moves(const struct board_s *pos, struct move_list_s *list) {
  int32_t pce = EMPTY;
  int32_t side = pos->side;
  int32_t sq = 0;
  int32_t t_sq = 0;
  int32_t dir = 0;
  int32_t pceIndex = 0;

  CHKBRD(pos);

  list->count = 0;

  if (side == WHITE) {
    for (int32_t pceNum = 0; pceNum < pos->pieceNum[wP]; ++pceNum) {
      sq = pos->pieceList[wP][pceNum];

      ASSERT(ce_valid_square(sq));

      // Handle pawn capture
      if (!SQOFFBOARD(sq + 9) && tbl_piece_col[pos->pieces[sq + 9]] == BLACK) {
        _ce_add_white_pawn_capture_move(pos, sq, sq + 9, pos->pieces[sq + 9], list);
      }
      if (!SQOFFBOARD(sq + 11) && tbl_piece_col[pos->pieces[sq + 11]] == BLACK) {
        _ce_add_white_pawn_capture_move(pos, sq, sq + 11, pos->pieces[sq + 11], list);
      }

      // Handle en-passent
      if (pos->enPassent != NO_SQ) {
        if (sq + 9 == pos->enPassent) {
          _ce_add_enpassent_move(pos, MOVE(sq, sq + 9, EMPTY, EMPTY, MFLAGEP), list);
          // _ce_add_capture_move(pos, MOVE(sq, sq + 9, EMPTY, EMPTY, MFLAGEP), list);
        }
        if (sq + 11 == pos->enPassent) {
          _ce_add_enpassent_move(pos, MOVE(sq, sq + 11, EMPTY, EMPTY, MFLAGEP), list);
          // _ce_add_capture_move(pos, MOVE(sq, sq + 11, EMPTY, EMPTY, MFLAGEP), list);
        }
      }
    }
  } else if (side == BLACK) {
    for (int32_t pceNum = 0; pceNum < pos->pieceNum[bP]; ++pceNum) {
      sq = pos->pieceList[bP][pceNum];

      ASSERT(ce_valid_square(sq));

      // Handle pawn capture
      if (!SQOFFBOARD(sq - 9) && tbl_piece_col[pos->pieces[sq - 9]] == WHITE) {
        _ce_add_black_pawn_capture_move(pos, sq, sq - 9, pos->pieces[sq - 9], list);
      }
      if (!SQOFFBOARD(sq - 11) && tbl_piece_col[pos->pieces[sq - 11]] == WHITE) {
        _ce_add_black_pawn_capture_move(pos, sq, sq - 11, pos->pieces[sq - 11], list);
      }

      // Handle en-passent
      if (pos->enPassent != NO_SQ) {
        if (sq - 9 == pos->enPassent) {
          _ce_add_enpassent_move(pos, MOVE(sq, sq - 9, EMPTY, EMPTY, MFLAGEP), list);
          // _ce_add_capture_move(pos, MOVE(sq, sq - 9, EMPTY, EMPTY, MFLAGEP), list);
        }
        if (sq - 11 == pos->enPassent) {
          _ce_add_enpassent_move(pos, MOVE(sq, sq - 11, EMPTY, EMPTY, MFLAGEP), list);
          // _ce_add_capture_move(pos, MOVE(sq, sq - 11, EMPTY, EMPTY, MFLAGEP), list);
        }
      }
    }
  }

  /* Sliding pieces (bishop, rook, and queen) */
  pceIndex = tbl_loop_slide_index[side];
  while ((pce = tbl_loop_slide_pce[pceIndex++]) != 0) {
    ASSERT(ce_valid_piece(pce));

    for (int32_t pceNum = 0; pceNum < pos->pieceNum[pce]; ++pceNum) {
      sq = pos->pieceList[pce][pceNum];
      ASSERT(ce_valid_square(sq));

      for (size_t index = 0; index < tbl_piece_dir_num[pce]; ++index) {
        dir = tbl_piece_dir[pce][index];
        t_sq = sq + dir;

        while (!SQOFFBOARD(t_sq)) {
          if (pos->pieces[t_sq] != EMPTY) {
            if (tbl_piece_col[pos->pieces[t_sq]] == (side ^ 1)) {
              _ce_add_capture_move(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
            }   
            break;
          }
          t_sq += dir;
        }
      }   
    }
  }

  /* Non-sliding pieces (knight, king) */
  pceIndex = tbl_loop_non_slide_index[side];
  while ((pce = tbl_loop_non_slide_pce[pceIndex++]) != 0) {
    ASSERT(ce_valid_piece(pce));

    for (int32_t pceNum = 0; pceNum < pos->pieceNum[pce]; ++pceNum) {
      sq = pos->pieceList[pce][pceNum];
      ASSERT(ce_valid_square(sq));

      for (size_t index = 0; index < tbl_piece_dir_num[pce]; ++index) {
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
      }
    }
  }
}

/**
 * Chess Engine generation function that generages all moves for the provided board
 * position and populates the provided move list structure.
 * @param A pointer to the current board position.
 * @param A pointer to the current move list being generated.
 */
void ce_generate_all_moves(const struct board_s *pos, struct move_list_s *list) {
  int32_t pce = EMPTY;
  int32_t side = pos->side;
  int32_t sq = 0;
  int32_t t_sq = 0;
  int32_t dir = 0;
  int32_t pceIndex = 0;

  CHKBRD(pos);

  list->count = 0;

  if (side == WHITE) {
    for (int32_t pceNum = 0; pceNum < pos->pieceNum[wP]; ++pceNum) {
      sq = pos->pieceList[wP][pceNum];

      ASSERT(ce_valid_square(sq));

      // Handle standard pawn move
      if (pos->pieces[sq + 10] == EMPTY) {
        _ce_add_white_pawn_move(pos, sq, sq + 10, list);
        if (tbl_ranks_board[sq] == RANK_2 && pos->pieces[sq + 20] == EMPTY) {
          _ce_add_quiet_move(pos, MOVE(sq, sq + 20, EMPTY, EMPTY, MFLAGPS), list);
        }
      }

      // Handle pawn capture
      if (!SQOFFBOARD(sq + 9) && tbl_piece_col[pos->pieces[sq + 9]] == BLACK) {
        _ce_add_white_pawn_capture_move(pos, sq, sq + 9, pos->pieces[sq + 9], list);
      }
      if (!SQOFFBOARD(sq + 11) && tbl_piece_col[pos->pieces[sq + 11]] == BLACK) {
        _ce_add_white_pawn_capture_move(pos, sq, sq + 11, pos->pieces[sq + 11], list);
      }

      // Handle en-passent
      if (pos->enPassent != NO_SQ) {
        if (sq + 9 == pos->enPassent) {
          _ce_add_enpassent_move(pos, MOVE(sq, sq + 9, EMPTY, EMPTY, MFLAGEP), list);
          // _ce_add_capture_move(pos, MOVE(sq, sq + 9, EMPTY, EMPTY, MFLAGEP), list);
        }
        if (sq + 11 == pos->enPassent) {
          _ce_add_enpassent_move(pos, MOVE(sq, sq + 11, EMPTY, EMPTY, MFLAGEP), list);
          // _ce_add_capture_move(pos, MOVE(sq, sq + 11, EMPTY, EMPTY, MFLAGEP), list);
        }
      }
    }

    // King side castling
    if (pos->castlePerms & WKCA) {
      if (pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY) {
        if (!ce_is_square_attacked(E1, BLACK, pos) && !ce_is_square_attacked(F1, BLACK, pos)) {
          _ce_add_quiet_move(pos, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA), list);
        }
      }
    }

    // Queen side castling
    if (pos->castlePerms & WQCA) {
      if (pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1] == EMPTY) {
        if (!ce_is_square_attacked(E1, BLACK, pos) && !ce_is_square_attacked(D1, BLACK, pos) && !ce_is_square_attacked(C1, BLACK, pos)) {
          _ce_add_quiet_move(pos, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA), list);
        }
      }
    }
  } else if (side == BLACK) {
    for (int32_t pceNum = 0; pceNum < pos->pieceNum[bP]; ++pceNum) {
      sq = pos->pieceList[bP][pceNum];

      ASSERT(ce_valid_square(sq));

      // Handle standard pawn move
      if (pos->pieces[sq - 10] == EMPTY) {
        _ce_add_black_pawn_move(pos, sq, sq - 10, list);
        if (tbl_ranks_board[sq] == RANK_7 && pos->pieces[sq - 20] == EMPTY) {
          _ce_add_quiet_move(pos, MOVE(sq, sq - 20, EMPTY, EMPTY, MFLAGPS), list);
        }
      }

      // Handle pawn capture
      if (!SQOFFBOARD(sq - 9) && tbl_piece_col[pos->pieces[sq - 9]] == WHITE) {
        _ce_add_black_pawn_capture_move(pos, sq, sq - 9, pos->pieces[sq - 9], list);
      }
      if (!SQOFFBOARD(sq - 11) && tbl_piece_col[pos->pieces[sq - 11]] == WHITE) {
        _ce_add_black_pawn_capture_move(pos, sq, sq - 11, pos->pieces[sq - 11], list);
      }

      // Handle en-passent
      if (pos->enPassent != NO_SQ) {
        if (sq - 9 == pos->enPassent) {
          _ce_add_enpassent_move(pos, MOVE(sq, sq - 9, EMPTY, EMPTY, MFLAGEP), list);
          // _ce_add_capture_move(pos, MOVE(sq, sq - 9, EMPTY, EMPTY, MFLAGEP), list);
        }
        if (sq - 11 == pos->enPassent) {
          _ce_add_enpassent_move(pos, MOVE(sq, sq - 11, EMPTY, EMPTY, MFLAGEP), list);
          // _ce_add_capture_move(pos, MOVE(sq, sq - 11, EMPTY, EMPTY, MFLAGEP), list);
        }
      }
    }

    // King side castling
    if (pos->castlePerms & BKCA) {
      if (pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY) {
        if (!ce_is_square_attacked(E8, WHITE, pos) && !ce_is_square_attacked(F8, WHITE, pos)) {
          _ce_add_quiet_move(pos, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCA), list);
        }
      }
    }

    // Queen side castling
    if (pos->castlePerms & BQCA) {
      if (pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8] == EMPTY) {
        if (!ce_is_square_attacked(E8, WHITE, pos) && !ce_is_square_attacked(D8, WHITE, pos) && !ce_is_square_attacked(C8, WHITE, pos)) {
          _ce_add_quiet_move(pos, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA), list);
        }
      }
    }
  }

  /* Sliding pieces (bishop, rook, and queen) */
  pceIndex = tbl_loop_slide_index[side];
  while ((pce = tbl_loop_slide_pce[pceIndex++]) != 0) {
    ASSERT(ce_valid_piece(pce));

    for (int32_t pceNum = 0; pceNum < pos->pieceNum[pce]; ++pceNum) {
      sq = pos->pieceList[pce][pceNum];
      ASSERT(ce_valid_square(sq));

      for (size_t index = 0; index < tbl_piece_dir_num[pce]; ++index) {
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

  /* Non-sliding pieces (knight, king) */
  pceIndex = tbl_loop_non_slide_index[side];
  while ((pce = tbl_loop_non_slide_pce[pceIndex++]) != 0) {
    ASSERT(ce_valid_piece(pce));

    for (int32_t pceNum = 0; pceNum < pos->pieceNum[pce]; ++pceNum) {
      sq = pos->pieceList[pce][pceNum];
      ASSERT(ce_valid_square(sq));

      for (size_t index = 0; index < tbl_piece_dir_num[pce]; ++index) {
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

/**
 * Chess Engine move function that validates that the provided move
 * exists for the provided board position.
 * @param pos The current board position to evaluate.
 * @param move The move to evaluate for the current position.
 * @return Boolean status if the provided move exists for the provided board position.
 */
bool ce_move_exists(struct board_s *pos, const uint32_t move) {
  struct move_list_s list = { 0 };

  ce_generate_all_moves(pos, &list);

  for (size_t index = 0; index < list.count; ++index) {
    if (!ce_move_make(pos, list.moves[index].move)) {
      continue;
    }

    ce_move_take(pos);

    if (list.moves[index].move == move) {
      return true;
    }
  }

  return false;
}
