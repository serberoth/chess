
#include "defs.h"

/**
 * Chess Engine function that resets the provided board position
 * to the initial board position clearing all moves, captures, and
 * resetting all other associated state for the provided board
 * position.
 * @param pos A pointer to the current board structure to reset.
 */
void ce_reset_board(struct board_s *pos) {
  int32_t index;

  for (index = 0; index < NUM_BRD_SQ; ++index) {
    pos->pieces[index] = OFFBOARD;
  }

  for (index = 0; index < 64; ++index) {
    pos->pieces[SQ120(index)] = EMPTY;
  }

  for (index = 0; index < 2; ++index) {
    pos->bigPieces[index] = 0;
    pos->majPieces[index] = 0;
    pos->minPieces[index] = 0;
    pos->material[index] = 0;
  }

  for (index = 0; index < 3; ++index) {
    pos->pawns[index] = 0ULL;
  }

  for (index = 0; index < 13; ++index) {
    pos->pieceNum[index] = 0;
  }

  pos->kingSq[WHITE] = pos->kingSq[BLACK] = NO_SQ;

  pos->side = BOTH;
  pos->enPassent = NO_SQ;
  pos->fiftyMove = 0;

  pos->ply = 0;
  pos->historyPly = 0;

  pos->castlePerms = 0;

  pos->positionKey = 0ULL;
}

/**
 * Chess Engine mirror the board position.
 * @param pos A pointer to the current board position
*/
void ce_mirror_board(struct board_s *pos) {
  int32_t piecesArray[64];
  int32_t side = pos->side ^ 1;
  int32_t swapPiece[13] = { EMPTY, bP, bN, bB, bR, bQ, bK, wP, wN, wB, wR, wQ, wK };
  int32_t castlePerms = 0;
  int32_t enPassent = NO_SQ;

  if (pos->castlePerms & WKCA) { castlePerms |= BKCA; }
  if (pos->castlePerms & WQCA) { castlePerms |= BQCA; }
  if (pos->castlePerms & BKCA) { castlePerms |= WKCA; }
  if (pos->castlePerms & BQCA) { castlePerms |= WQCA; }

  if (pos->enPassent != NO_SQ) {
    enPassent = SQ120(MIR64(SQ64(pos->enPassent)));
  }
  for (size_t sq = 0; sq < 64; ++sq) {
    piecesArray[sq] = pos->pieces[SQ120(MIR64(sq))];
  }

  ce_reset_board(pos);

  for (size_t sq = 0; sq < 64; ++sq) {
    pos->pieces[SQ120(sq)] = swapPiece[piecesArray[sq]];
  }

  pos->side = side;
  pos->castlePerms = castlePerms;
  pos->enPassent = enPassent;

  pos->positionKey = ce_generate_position_key(pos);

  ce_update_material_list(pos);

  CHKBRD(pos);
}

/**
 * Chess Engine parsing function to parse the provided FEN chess position string
 * and configure the provided board position structure according to the FEN
 * position.
 * @param fen A string containing the FEN chess position information.
 * @param pos A pointer to the current board position.
 * @return Boolean value indicating the success or failure of this function.
 */
bool ce_parse_fen(char *fen, struct board_s *pos) {
  ASSERT(fen != NULL);
  ASSERT(pos != NULL);

  int32_t rank = RANK_8, file = FILE_A;
  int32_t piece = 0;
  int32_t count = 0;
  int32_t sq64 = 0;
  int32_t sq120 = 0;
  int32_t i = 0;

  ce_reset_board(pos);

  while ((rank >= RANK_1) && *fen) {
    count = 1;

    switch (*fen) {
    case 'p': piece = bP; break;
    case 'r': piece = bR; break;
    case 'n': piece = bN; break;
    case 'b': piece = bB; break;
    case 'k': piece = bK; break;
    case 'q': piece = bQ; break;
    case 'P': piece = wP; break;
    case 'R': piece = wR; break;
    case 'N': piece = wN; break;
    case 'B': piece = wB; break;
    case 'K': piece = wK; break;
    case 'Q': piece = wQ; break;

    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
      piece = EMPTY;
      count = *fen - '0';
      break;

    case '/':
    case ' ':
      --rank;
      file = FILE_A;
      ++fen;
      continue;

    default:
      printf(u8"FEN error\n");
      return false;
    }

    for (i = 0; i < count; ++i) {
      sq64 = rank * 8 + file;
      sq120 = SQ120(sq64);
      if (piece != EMPTY) {
        pos->pieces[sq120] = piece;
      }
      ++file;
    }

    ++fen;
  }

  ASSERT(*fen == 'w' || *fen == 'b');
  pos->side = (*fen == 'w') ? WHITE : BLACK;
  fen += 2;

  for (i = 0; i < 4; ++i) {
    if (*fen == ' ') {
      break;
    }
    switch (*fen) {
    case 'K': pos->castlePerms |= WKCA; break;
    case 'Q': pos->castlePerms |= WQCA; break;
    case 'k': pos->castlePerms |= BKCA; break;
    case 'q': pos->castlePerms |= BQCA; break;
    default: break;
    }
    ++fen;
  }
  ++fen;  

  ASSERT(pos->castlePerms >= 0 && pos->castlePerms <= 15);

  if (*fen != '-') {
    file = fen[0] - 'a';
    rank = fen[1] - '1';

    ASSERT(file >= FILE_A && file <= FILE_H);
    ASSERT(rank >= RANK_1 && rank <= RANK_8);

    pos->enPassent = FR2SQ(file, rank);
  }
  fen += 2;

  // TODO: Parse the half-move clock as well as the full-move clock

  pos->positionKey = ce_generate_position_key(pos);
  ce_update_material_list(pos);

  if (pos->pvtable.count > 0) {
    ce_pvtable_free(&pos->pvtable);
  }

  ce_pvtable_init(&pos->pvtable, 16);

  return true;
}

/**
 * Chess Engine function to update the material lists for the provided
 * board position.
 * @param pos A pointer to the current board position.
 */
void ce_update_material_list(struct board_s *pos) {
  int32_t piece, sq, index, colour;

  for (index = 0; index < NUM_BRD_SQ; ++index) {
    sq = index;
    piece = pos->pieces[index];

    if (piece != OFFBOARD && piece != EMPTY) {
      colour = tbl_piece_col[piece];
      if (tbl_piece_big[piece]) { pos->bigPieces[colour]++; }
      if (tbl_piece_maj[piece]) { pos->majPieces[colour]++; }
      if (tbl_piece_min[piece]) { pos->minPieces[colour]++; }

      pos->material[colour] += tbl_piece_val[piece];

      // piece list
      pos->pieceList[piece][pos->pieceNum[piece]] = sq;
      pos->pieceNum[piece]++;

      if (piece == wK) {
        pos->kingSq[WHITE] = sq;
      }
      if (piece == bK) {
        pos->kingSq[BLACK] = sq;
      }

      if (piece == wP) {
        SETBIT(pos->pawns[WHITE], SQ64(sq));
        SETBIT(pos->pawns[BOTH], SQ64(sq));
      } else if (piece == bP) {
        SETBIT(pos->pawns[BLACK], SQ64(sq));
        SETBIT(pos->pawns[BOTH], SQ64(sq));
      }
    }
  }
}

/**
 * Chess Engine function to validate the provided board position.
 * This function is used for debugging the engine and relies heavily on
 * assertions that when compiled in debug will cause a fault in the
 * application if an error is detected in the provided board position.
 * @param pos A pointer to the current board position.
 * @return This function always returns true.
 */
bool ce_check_board(const struct board_s *pos) {
  int32_t t_pceNum[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  int32_t t_bigPce[2] = { 0, 0 };
  int32_t t_majPce[2] = { 0, 0 };
  int32_t t_minPce[2] = { 0, 0 };
  int32_t t_material[2] = { 0, 0 };

  int32_t sq64, t_piece, t_pce_num, sq120, colour, pcount;

  uint64_t t_pawns[3] = {
    pos->pawns[WHITE],
    pos->pawns[BLACK],
    pos->pawns[BOTH]
  };

  for (t_piece = wP; t_piece <= bK; ++t_piece) {
    for (t_pce_num = 0; t_pce_num < pos->pieceNum[t_piece]; ++t_pce_num) {
      sq120 = pos->pieceList[t_piece][t_pce_num];
      ASSERT(pos->pieces[sq120] == t_piece);
    }
  }

  for (sq64 = 0; sq64 < 64; ++sq64) {
    sq120 = SQ120(sq64);
    t_piece = pos->pieces[sq120];
    t_pceNum[t_piece]++;
    colour = tbl_piece_col[t_piece];
    if (tbl_piece_big[t_piece]) { t_bigPce[colour]++; }
    if (tbl_piece_maj[t_piece]) { t_majPce[colour]++; }
    if (tbl_piece_min[t_piece]) { t_minPce[colour]++; }

    t_material[colour] += tbl_piece_val[t_piece];
  }

  for (t_piece = wP; t_piece <= bK; ++t_piece) {
    ASSERT(t_pceNum[t_piece] == pos->pieceNum[t_piece]);
  }

  pcount = CNT(t_pawns[WHITE]);
  ASSERT(pcount == pos->pieceNum[wP]);
  pcount = CNT(t_pawns[BLACK]);
  ASSERT(pcount == pos->pieceNum[bP]);
  pcount = CNT(t_pawns[BOTH]);
  ASSERT(pcount == (pos->pieceNum[wP] + pos->pieceNum[bP]));

  while (t_pawns[WHITE]) {
    sq64 = POP(&t_pawns[WHITE]);
    ASSERT(pos->pieces[SQ120(sq64)] == wP);
  }

  while (t_pawns[BLACK]) {
    sq64 = POP(&t_pawns[BLACK]);
    ASSERT(pos->pieces[SQ120(sq64)] == bP);
  }

  while (t_pawns[BOTH]) {
    sq64 = POP(&t_pawns[BOTH]);
    sq120 = SQ120(sq64);
    ASSERT((pos->pieces[sq120] == wP) || (pos->pieces[sq120] == bP));
  }

  ASSERT(t_bigPce[WHITE] == pos->bigPieces[WHITE] && t_bigPce[BLACK] == pos->bigPieces[BLACK]);
  ASSERT(t_majPce[WHITE] == pos->majPieces[WHITE] && t_majPce[BLACK] == pos->majPieces[BLACK]);
  ASSERT(t_minPce[WHITE] == pos->minPieces[WHITE] && t_minPce[BLACK] == pos->minPieces[BLACK]);
  ASSERT(t_material[WHITE] == pos->material[WHITE] && t_material[BLACK] == pos->material[BLACK]);

  ASSERT(pos->side == WHITE || pos->side == BLACK);
  // ce_print_board(pos);
  ASSERT(ce_generate_position_key(pos) == pos->positionKey);

  ASSERT(pos->enPassent == NO_SQ
    || (tbl_ranks_board[pos->enPassent] == RANK_6 && pos->side == WHITE)
    || (tbl_ranks_board[pos->enPassent] == RANK_3 && pos->side == BLACK));

  ASSERT(pos->pieces[pos->kingSq[WHITE]] == wK);
  ASSERT(pos->pieces[pos->kingSq[BLACK]] == bK);

  return true;
}
