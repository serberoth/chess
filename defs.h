#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdlib.h>

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) do { \
	if (!(n)) { \
		printf("%s - Assertion failed in file %s at line %d on %s at %s.\n", \
			#n, __FILE__, __LINE__, __DATE__, __TIME__); \
		exit(1); \
	} \
} while(0)
#endif

typedef unsigned long long U64;

#define NAME		"Chess 1.0"
#define NUM_BRD_SQ	120

#define MAX_GAME_MOVES	2048

typedef enum { FALSE, TRUE } BOOL;

// Piece constants
enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
// Rank and file constants
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

// Color constants
enum { WHITE, BLACK, BOTH };

// Castling permissions
enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };

enum {
  A1 = 21, B1, C1, D1, E1, F1, G1, H1,
  A2 = 31, B2, C2, D2, E2, F2, G2, H2,
  A3 = 41, B3, C3, D3, E3, F3, G3, H3,
  A4 = 51, B4, C4, D4, E4, F4, G4, H4,
  A5 = 61, B5, C5, D5, E5, F5, G5, H5,
  A6 = 71, B6, C6, D6, E6, F6, G6, H6,
  A7 = 81, B7, C7, D7, E7, F7, G7, H7,
  A8 = 91, B8, C8, D8, E8, F8, G8, H8,
  NO_SQ
};

struct undo_s {
  int move;

  int enPassent;
  int fiftyMove;

  int castlePerms;

  U64 positionKey;
};

struct board_s {
  int pieces[NUM_BRD_SQ];
  U64 pawns[3];        // the position of the pawns

  int kingSq[2];       // the position of the kings

  int side;            // the current side to move
  int enPassent;       // en passent square position
  int fiftyMove;       // fifty move counter

  int castlePerm;      // castling permissions

  U64 positionKey;     // unique board position key

  int pieceNum[13];    // array of number of pieces
  int bigPieces[3];    // array of pieces containing anything that is not a pawn
  int majPieces[3];    // array of pieces containing rooks and queens
  int minPieces[3];    // array of pieces containing bishops and knights

  int ply;             // current ply
  int historyPly;      // historical play number

  struct undo_s history[MAX_GAME_MOVES];

  int pieceList[13][10]; // piece List

};

/* MACROS */

#define FR2SQ(f, r)		((21 + (f)) + ((r) * 10))

#define SQ64(sq120)		(tbl_sq120_to_sq64[(sq120)])
#define SQ120(sq64)		(tbl_sq64_to_sq120[(sq64)])

#define POP(b)			(ce_pop_bit(b))
#define CNT(b)			(ce_count_bits(b))

#define CLRBIT(b, sq)		((b) &= tbl_clear_mask[(sq)])
#define SETBIT(b, sq)		((b) |= tbl_set_mask[(sq)])

/* GLOBALS */

extern int tbl_sq120_to_sq64[NUM_BRD_SQ];
extern int tbl_sq64_to_sq120[64];
extern U64 tbl_set_mask[64];
extern U64 tbl_clear_mask[64];

/* FUNCTIONS */

// init.c
extern void ce_init();

// bitboards.c
extern int ce_pop_bit(U64 *board);
extern int ce_count_bits(U64 board);
extern void ce_print_bitboard(U64 board);

// diag.c
extern void ce_diag_tbl_print();

#endif

