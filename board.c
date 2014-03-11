
#include "defs.h"

void ce_reset_board(struct board_s *board) {
  int index;

  for (index = 0; index < NUM_BRD_SQ; ++index) {
    board->pieces[index] = OFFBOARD;
  }

  for (index = 0; index < 64; ++index) {
    board->pieces[SQ120(index)] = EMPTY;
  }

  for (index = 0; index < 3; ++index) {
    board->bigPieces[index] = 0;
    board->majPieces[index] = 0;
    board->minPieces[index] = 0;
    board->pawns[index] = 0;
  }

  for (index = 0; index < 13; ++index) {
    board->pieceNum[index] = 0;
  }

  board->kingSq[WHITE] = board->kingSq[BLACK] = NO_SQ;

  board->side = BOTH;
  board->enPassent = NO_SQ;
  board->fiftyMove = 0;

  board->ply = 0;
  board->historyPly = 0;

  board->castlePerms = 0;

  board->positionKey = 0ULL;
}

int ce_parse_fen(char *fen, struct board_s *pos) {
  ASSERT(fen != NULL);
  ASSERT(pos != NULL);

  int rank = RANK_8, file = FILE_A;
  int piece = 0;
  int count = 0;
  int sq64 = 0;
  int sq120 = 0;
  int i = 0;

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
      printf("FEN error\n");
      return -1;
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

  return 0;
}

void ce_update_material_list(struct board_s *pos) {
  int piece, sq, index, colour;

  for (index = 0; index < NUM_BRD_SQ; ++index) {
    sq = index;
    piece = pos->pieces[index];

    if (piece != OFFBOARD && piece != EMPTY) {
      colour = tbl_piece_col[piece];
      if (tbl_piece_big[piece] == TRUE) { pos->bigPieces[colour]++; }
      if (tbl_piece_maj[piece] == TRUE) { pos->majPieces[colour]++; }
      if (tbl_piece_min[piece] == TRUE) { pos->minPieces[colour]++; }

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

int ce_check_board(const struct board_s *pos) {
  int t_pceNum[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  int t_bigPce[2] = { 0, 0 };
  int t_majPce[2] = { 0, 0 };
  int t_minPce[2] = { 0, 0 };
  int t_material[2] = { 0, 0 };

  int sq64, t_piece, t_pce_num, sq120, colour, pcount;

  U64 t_pawns[3] = {
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
    if (tbl_piece_big[t_piece] == TRUE) { t_bigPce[colour]++; }
    if (tbl_piece_maj[t_piece] == TRUE) { t_majPce[colour]++; }
    if (tbl_piece_min[t_piece] == TRUE) { t_minPce[colour]++; }

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
  ce_print_board(pos);
  ASSERT(ce_generate_position_key(pos) == pos->positionKey);

  ASSERT(pos->enPassent == NO_SQ
    || (tbl_ranks_board[pos->enPassent] == RANK_6 && pos->side == WHITE)
    || (tbl_ranks_board[pos->enPassent] == RANK_3 && pos->side == BLACK));

  ASSERT(pos->pieces[pos->kingSq[WHITE]] == wK);
  ASSERT(pos->pieces[pos->kingSq[BLACK]] == bK);

  return TRUE;
}

