
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
    }
  }
}

