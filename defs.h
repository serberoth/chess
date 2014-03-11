#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdio.h>
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

#define CHKBRD(b)		ASSERT(ce_check_board((b)))

typedef unsigned long long U64;

typedef enum { FALSE, TRUE } BOOL;

#define NAME		"Chess 1.0"
#define NUM_BRD_SQ	120

#define MAX_GAME_MOVES	2048

#define START_FEN	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

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
  NO_SQ, OFFBOARD
};

struct move_s {
  int move;
  int score;
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

  int castlePerms;     // castling permissions

  U64 positionKey;     // unique board position key (board hashkey)

  int pieceNum[13];    // array of number of pieces
  int bigPieces[2];    // array of pieces containing anything that is not a pawn
  int majPieces[2];    // array of pieces containing rooks and queens
  int minPieces[2];    // array of pieces containing bishops and knights
  int material[2];     // the value of the material score

  int ply;             // current ply
  int historyPly;      // historical play number

  struct undo_s history[MAX_GAME_MOVES];

  int pieceList[13][10]; // piece List

};

/* GAME MOVE  */
/*
TODO: Maybe rearrange these later?
0000 0000 0000 0000 0000 0111 1111 -> From Square
0000 0000 0000 0011 1111 1000 0000 -> To Square
0000 0000 0011 1100 0000 0000 0000 -> Captured Piece
0000 0000 0100 0000 0000 0000 0000 -> En Passent
0000 0000 1000 0000 0000 0000 0000 -> Pawn Start
0000 1111 0000 0000 0000 0000 0000 -> Promoted Piece
0001 0000 0000 0000 0000 0000 0000 -> Castle
 */
union move_u {
  int move;
  struct {
    int fromSq : 7;
    int toSq : 7;
    int capturedPiece : 4;
    int enPassent : 1;
    int pawnStart : 1;
    int promotedPiece : 4;
    int castle : 1;
    int reserved : 7;
  } fields;
};

#define MV(m)			((union move_u) (m))

#define FROMSQ(m)		((m) & 0x3f)
#define TOSQ(m)			(((m) >> 7) & 0x3f)
#define CAPTURED(m)		(((m) >> 14) & 0xf)
#define PROMOTED(m)		(((m) >> 20) & 0xf)

#define MFLAGEP			0x40000
#define MFLAGPS			0x80000
#define MFLAGCA			0x1000000

#define MFLAGCAP		0x7c000
#define MFLAGPROM		0xf00000

/* MACROS */

#define FR2SQ(f, r)		((21 + (f)) + ((r) * 10))

#define SQ64(sq120)		(tbl_sq120_to_sq64[(sq120)])
#define SQ120(sq64)		(tbl_sq64_to_sq120[(sq64)])

#define POP(b)			(ce_pop_bit(b))
#define CNT(b)			(ce_count_bits(b))

#define CLRBIT(b, sq)		((b) &= tbl_clear_mask[(sq)])
#define SETBIT(b, sq)		((b) |= tbl_set_mask[(sq)])

#define IsKn(p)			(tbl_piece_knight[(p)])
#define IsKi(p)			(tbl_piece_king[(p)])
#define IsRQ(p)			(tbl_piece_rook_queen[(p)])
#define IsBQ(p)			(tbl_piece_bishop_queen[(p)])
 
/* GLOBALS */

extern int tbl_sq120_to_sq64[NUM_BRD_SQ];
extern int tbl_sq64_to_sq120[64];

extern U64 tbl_set_mask[64];
extern U64 tbl_clear_mask[64];

extern U64 tbl_piece_keys[13][120];
extern U64 side_key;
extern U64 tbl_castle_keys[16];

extern char tbl_piece_char[];
extern char tbl_side_char[];
extern char tbl_rank_char[];
extern char tbl_file_char[];

extern int tbl_piece_big[13];
extern int tbl_piece_maj[13];
extern int tbl_piece_min[13];
extern int tbl_piece_val[13];
extern int tbl_piece_col[13];

extern int tbl_files_board[NUM_BRD_SQ];
extern int tbl_ranks_board[NUM_BRD_SQ];

extern int tbl_piece_knight[13];
extern int tbl_piece_king[13];
extern int tbl_piece_rook_queen[13];
extern int tbl_piece_bishop_queen[13];

/* FUNCTIONS */

// init.c
extern void ce_init();

// bitboards.c
extern int ce_pop_bit(U64 *);
extern int ce_count_bits(U64);

// hashkeys.c
extern U64 ce_generate_position_key(const struct board_s *);

// board.c
extern void ce_reset_board(struct board_s *);
extern int ce_parse_fen(char *, struct board_s *);
extern void ce_update_material_list(struct board_s *);
extern int ce_check_board(const struct board_s *);

// attack.c
extern int ce_is_square_attacked(const int, const int, const struct board_s *);

// data.c

// diag.c
extern void ce_diag_print_tbls();
extern void ce_diag_print_bitboard(U64);
extern void ce_print_board(const struct board_s *);
extern void ce_diag_show_attacked_by_side(const int, const struct board_s *);

#endif

