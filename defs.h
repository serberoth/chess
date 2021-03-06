#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/**
 * Unsigned long 64-bit type used for various data representations including
 * bit-board representations of a chess board position.
 */
typedef unsigned long long U64;

/**
 * Boolean enumeration data type definition
 */
typedef enum { FALSE, TRUE } BOOL;

/**
 * Terminal colour enumeration
 */
enum { CLR_NORMAL, CLR_RED, CLR_GREEN, CLR_YELLOW, CLR_BLUE, CLR_MAGENTA, CLR_CYAN, CLR_WHITE, CLR_BLACK };

#define NAME			"Chess 1.0"
#define AUTHOR			"DarkSide Software"
#define NUM_BRD_SQ		120

#define MAX_GAME_MOVES		2048
#define MAX_POSITION_MOVES	246
#define MAX_DEPTH		64

#define INFINITY		300000
#define MATE			49000

#define START_FEN		"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

/**
 * Chess piece constants.
 */
// Piece constants
enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
/**
 * Chess board file constants.
 */
// Rank and file constants
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
/**
 * Chess board rank constants.
 */
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

/**
 * Chess board side/piece colour constants.
 */
// Colour constants
enum { WHITE, BLACK, BOTH };

/**
 * Engine protocol mode
 */
enum { MODE_UCI, MODE_XBOARD, MODE_CONSOLE };

/**
 * Chess castling permissions.
 */
// Castling permissions
enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };

/**
 * Chess board position constants for 64 square representations.
 */
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

/**
 * Chess move union this union represents the information of a
 * single chess move.  The information is packed into a single
 * unsigned integer value.
 */
/* GAME MOVE  */
/*
TODO: Maybe rearrange these later?
0000 0000 0000 0000 0000 0111 1111 -> From/At Square
0000 0000 0000 0011 1111 1000 0000 -> To Square
0000 0000 0011 1100 0000 0000 0000 -> Captured Piece
0000 0000 0100 0000 0000 0000 0000 -> En Passent
0000 0000 1000 0000 0000 0000 0000 -> Pawn Start
0000 1111 0000 0000 0000 0000 0000 -> Promoted Piece
0001 0000 0000 0000 0000 0000 0000 -> Castle
 */
union move_u {
  /** The unsigned integer value representing the chess move. */
  unsigned val;
  struct {
    /** The origination square of the move. */
    unsigned at : 7;
    /** The destination square of the move. */
    unsigned to : 7;
    /** The numerical value of the piece being captured by this move, if any. */
    unsigned captured : 4;
    /** Flag indicating if this is an enpassent move. */
    unsigned enPassent : 1;
    /** Flag indicating if this is a pawn start move. */
    unsigned pawnStart : 1;
    /** The numerical value of the piece being promoted to by this move, if any. */
    unsigned promoted : 4;
    /** Flag indicating if this is a castling move. */
    unsigned castle : 1;
    /** Flag indicating if this move ends up in check for the opponent */
    unsigned check : 1;
    /** Reserved value. */
    unsigned reserved : 6;
  };  
};

#define MV(m)                   (*((union move_u *) &(m)))

#define FROMSQ(m)               ((m) & 0x7f)
#define TOSQ(m)                 (((m) >> 7) & 0x7f)
#define CAPTURED(m)             (((m) >> 14) & 0xf)
#define PROMOTED(m)             (((m) >> 20) & 0xf)

// Move flag for enpassent moves
#define MFLAGEP			0x40000
// Move flag for pawn start moves
#define MFLAGPS			0x80000
// Move flag for castling moves
#define MFLAGCA			0x1000000

// Move flag for capture moves
#define MFLAGCAP		0x7c000
// Move flag for promotion moves
#define MFLAGPROM		0xf00000

#define NOMOVE			0

/* TYPEDEFS */

/**
 * Chess move structure representing a single move in chess.
 */
struct move_s {
  // TODO: Convert this to a single union move_u field instead of a struct
  union {
    /** Integral value representing the current move. */
    int move;
    /** move_u union type representing the individual fields of the current move. */
    union move_u fields;
  };

  /** The evaluated score of the current move. */
  int score;
};

/**
 * Chess move list structure containing a list of moves up to the maximum allowed
 * number of moves that can be evaluated by this engine at one time.
 */
struct move_list_s {
  /** The list of moves */
  struct move_s moves[MAX_POSITION_MOVES];
  /** The number of moves currently stored in this list. */
  int count;
};

/**
 * Chess principal variation table entry.
 */
struct pventry_s {
  /** The bit-board position key for this move. */
  U64 positionKey;
  /** The move of this principal variation. */
  union move_u move;
};

/**
 * Chess principal variation table data structure.
 */
struct pvtable_s {
  /** The principal variation chess move table entries. */
  struct pventry_s *entries;
  /** The number of entries currently stored in this table. */
  int count;
};

/**
 * Chess move undo data structure.
 */
struct undo_s {
  union {
    /** Integral representation of the chess move. */
    int move;
    /** Chess move union structure for the current move. */
    union move_u fields;
  };

  /** The position of the en passent square for this move, if any. */ 
  int enPassent;
  /** Fifty move counter for this move. */
  int fiftyMove;

  /** Castle permission for this move. */
  int castlePerms;

  /** Bit-board position key for this move. */
  U64 positionKey;
};

/**
 * Chess board data structuer.
 */
struct board_s {
  /** The representation of the chess pieces on the chess board in a 64 square board. */
  int pieces[NUM_BRD_SQ];
  /** The bit-board position of the pawns for each color and both colors. */
  U64 pawns[3];        // the position of the pawns

  /** The 64 square board position for the kings. */
  int kingSq[2];       // the position of the kings

  /** The value of the current side to move. */
  int side;            // the current side to move
  /** The position of the en passent square for this board position, if any. */
  int enPassent;       // en passent square position
  /** The value of the fifty move counter for this board position. */
  int fiftyMove;       // fifty move counter

  /** The current castling permissions for this board position. */
  int castlePerms;     // castling permissions

  /** The bit-board position key hash */
  U64 positionKey;     // unique board position key (board hashkey)

  /** The number of pieces remaining on the board broken down by piece and colour. */
  int pieceNum[13];    // array of number of pieces
  /** The number of big pieces (pieces which are not pawns) on the board by colour. */
  int bigPieces[2];    // array of pieces containing anything that is not a pawn
  /** The number of major pieces (rooks and queens) on the board by colour. */
  int majPieces[2];    // array of pieces containing rooks and queens
  /** The number of minor pieces (bishops and knights) on the board by colour. */
  int minPieces[2];    // array of pieces containing bishops and knights
  /** The numerical value of the material for each colour that remains on the board. */
  int material[2];     // the value of the material score

  /** The current ply of the game. */
  int ply;             // current ply
  /** The historical ply number of this board position. */
  int historyPly;      // historical play number

  /** The undo history for this board position. */
  struct undo_s history[MAX_GAME_MOVES];

  /** The piece list for this board position. */
  int pieceList[13][10]; // piece list

  /** The principal variation table for the current board position. */
  struct pvtable_s pvtable; // principal variation table
  /** The principal variation depth array. */
  int pvarray[MAX_DEPTH];   // principal variation depth array

  /** The alhpa-beta search history for the current board position. */
  int searchHistory[13][NUM_BRD_SQ]; // alpha-beta search history
  /** The beta cuttof moves for the current board position. */
  int searchKillers[2][MAX_DEPTH];   // beta cuttoff moves
};

/**
 * Chess engine search constraints data structure.
 */
struct search_info_s {
  /** The search start time. */
  int startTime;
  /** The search cutoff/stop time. */
  int stopTime;
  /** The current search depth. */
  int depth;
  /** The search maximum depth. */
  int depthSet;
  /** The search maximum time allotment. */
  int timeSet;
  /* The number of moves to go in this search. */
  int movesToGo;
  /* Infinite search mode flag indicator. */
  int infinite;

  /** The number of nodes examined in the current search. */
  unsigned long nodes;

  /** Protocol mode */
  int gameMode;
  /** Post thinking flag */
  int postThinking;

  /** Search quite flag. */
  int quit;
  /** Search stopped flag. */
  int stopped;

  /** Search fail high ratio. */
  float failHigh;
  /** Search fail high first ratio. */
  float failHighFirst;
};

/* MACROS */

#define FR2SQ(f, r)		((21 + (f)) + ((r) * 10))
#define SQ2RANK(sq)		(((sq) - 21) / 10)
#define SQ2FILE(sq)		(((sq) - 21) % 10)

#define SQ64(sq120)		(tbl_sq120_to_sq64[(sq120)])
#define SQ120(sq64)		(tbl_sq64_to_sq120[(sq64)])

#define POP(b)			(ce_pop_bit(b))
#define CNT(b)			(ce_count_bits(b))

#define CLRBIT(b, sq)		((b) &= tbl_clear_mask[(sq)])
#define SETBIT(b, sq)		((b) |= tbl_set_mask[(sq)])

#define IsPw(p)			((p) != EMPTY && (p) != OFFBOARD && tbl_piece_pawn[(p)])
#define IsKn(p)			((p) != EMPTY && (p) != OFFBOARD && tbl_piece_knight[(p)])
#define IsKi(p)			((p) != EMPTY && (p) != OFFBOARD && tbl_piece_king[(p)])
#define IsRQ(p)			((p) != EMPTY && (p) != OFFBOARD && tbl_piece_rook_queen[(p)])
#define IsBQ(p)			((p) != EMPTY && (p) != OFFBOARD && tbl_piece_bishop_queen[(p)])
 
/* GLOBALS */

// init.c
extern int tbl_sq120_to_sq64[NUM_BRD_SQ];
extern int tbl_sq64_to_sq120[64];

extern U64 tbl_set_mask[64];
extern U64 tbl_clear_mask[64];

extern U64 tbl_piece_keys[13][120];
extern U64 side_key;
extern U64 tbl_castle_keys[16];

extern int tbl_files_board[NUM_BRD_SQ];
extern int tbl_ranks_board[NUM_BRD_SQ];

// data.c
extern const char tbl_piece_char[];
extern const char tbl_side_char[];
extern const char tbl_rank_char[];
extern const char tbl_file_char[];

extern const int tbl_piece_big[13];
extern const int tbl_piece_maj[13];
extern const int tbl_piece_min[13];
extern const int tbl_piece_val[13];
extern const int tbl_piece_col[13];

extern const int tbl_piece_pawn[13];
extern const int tbl_piece_knight[13];
extern const int tbl_piece_king[13];
extern const int tbl_piece_rook_queen[13];
extern const int tbl_piece_bishop_queen[13];

extern const int tbl_piece_slides[13];

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

// io.c
extern char *ce_print_sq(const int);
extern char *ce_print_move(const union move_u);
extern void ce_print_move_list(const struct move_list_s *);
extern int ce_parse_move(char *, struct board_s *);

// movegen.c
extern void ce_init_mvv_lva();
extern void ce_generate_capture_moves(const struct board_s *, struct move_list_s *);
extern void ce_generate_all_moves(const struct board_s *, struct move_list_s *);
extern int ce_move_exists(struct board_s *, const int);

// valid.c
extern int ce_valid_square(const int);
extern int ce_valid_side(const int);
extern int ce_valid_file_rank(const int);
extern int ce_valid_piece_empty(const int);
extern int ce_valid_piece(const int);

// makemove.c
extern void ce_move_take(struct board_s *);
extern int ce_move_make(struct board_s *, int);

// perft.c
extern unsigned long ce_perf_test(int, struct board_s *);

// full_perft.c
extern void ce_all_perf_tests();

// search.c
extern void ce_search_position(struct board_s *, struct search_info_s *);

// utils.c
extern int sys_time_ms();
extern int sys_input_waiting();
extern void sys_read_input(struct search_info_s *);

// pvtable.c
extern void ce_pvtable_init(struct pvtable_s *);
extern void ce_pvtable_free(struct pvtable_s *);
extern void ce_pvtable_clear(struct pvtable_s *);
extern void ce_pvtable_store(const struct board_s *, const int);
extern int ce_pvtable_probe(const struct board_s *);
extern int ce_pvtable_get_line(const int, struct board_s *);

// evaluate.c
extern int ce_eval_position(const struct board_s *);

// uci.c
extern void ce_uci_loop(struct board_s *, struct search_info_s *);

// xboard.c
extern int ce_check_result(struct board_s *);
extern void ce_xboard_loop(struct board_s *, struct search_info_s *);

// console.c
extern void ce_console_loop(struct board_s *, struct search_info_s *);

// data.c

// diag.c
extern void ce_diag_print_tbls();
extern void ce_diag_print_bitboard(U64);
extern void ce_print_board(const struct board_s *);
extern void ce_print_coloured_board(const struct board_s *, int, int, int);
extern void ce_print_fen(const struct board_s *);
extern void ce_diag_show_attacked_by_side(const int, const struct board_s *);
extern void ce_print_binary(int);

#endif

