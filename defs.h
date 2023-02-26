#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) do { \
	if (!(n)) { \
		printf(u8"%s - Assertion failed in file %s at line %d on %s at %s.\n", \
			#n, __FILE__, __LINE__, __DATE__, __TIME__); \
		exit(1); \
	} \
} while(0)
#endif

#define CHKBRD(b)		ASSERT(ce_check_board((b)))

/**
 * Terminal colour enumeration
 */
enum { CLR_NORMAL, CLR_RED, CLR_GREEN, CLR_YELLOW, CLR_BLUE, CLR_MAGENTA, CLR_CYAN, CLR_WHITE, CLR_BLACK };

#define NAME                u8"Chess 1.1"
#define AUTHOR              u8"DarkMatter Software"
#define NUM_BRD_SQ          120

#define MAX_GAME_MOVES      2048
#define MAX_POSITION_MOVES	246
#define MAX_DEPTH           64

#define INFINITY            300000
#define MATE                (INFINITY - MAX_DEPTH)    // 49000

#define START_FEN           u8"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

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
 * Transposition table score type
 */
enum { HFNONE, HFALPHA, HFBETA, HFEXACT, };

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
0000 0000 0000 0000 0000 0000 0111 1111 -> From/At Square
0000 0000 0000 0000 0011 1111 1000 0000 -> To Square
0000 0000 0000 0011 1100 0000 0000 0000 -> Captured Piece
0000 0000 0000 0100 0000 0000 0000 0000 -> En Passent
0000 0000 0000 1000 0000 0000 0000 0000 -> Pawn Start
0000 0000 1111 0000 0000 0000 0000 0000 -> Promoted Piece
0000 0001 0000 0000 0000 0000 0000 0000 -> Castle
0000 0010 0000 0000 0000 0000 0000 0000 -> Check
 */
union move_u {
  /** The unsigned integer value representing the chess move. */
  uint32_t val;
  struct {
    /** The origination square of the move. */
    uint32_t at : 7;
    /** The destination square of the move. */
    uint32_t to : 7;
    /** The numerical value of the piece being captured by this move, if any. */
    uint32_t captured : 4;
    /** Flag indicating if this is an enpassent move. */
    uint32_t enPassent : 1;
    /** Flag indicating if this is a pawn start move. */
    uint32_t pawnStart : 1;
    /** The numerical value of the piece being promoted to by this move, if any. */
    uint32_t promoted : 4;
    /** Flag indicating if this is a castling move. */
    uint32_t castle : 1;
    /** Flag indicating if this move ends up in check for the opponent */
    uint32_t check : 1;
    /** Reserved value. */
    uint32_t reserved : 6;
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

#define NOMOVE			((uint32_t) 0)

/* TYPEDEFS */

/**
 * Chess move structure representing a single move with a score in chess.
 */
struct move_s {
  /** move_u union type representing the individual fields of the current move. */
  union move_u move;

  /** The evaluated score of the current move. */
  int32_t score;
};

/**
 * Chess move list structure containing a list of moves up to the maximum allowed
 * number of moves that can be evaluated by this engine at one time.
 */
struct move_list_s {
  /** The list of moves */
  struct move_s moves[MAX_POSITION_MOVES];
  /** The number of moves currently stored in this list. */
  size_t count;
};

/**
 * Chess move hash entry for transposition tables
 * Chess principal variation table entry.
 */
struct pventry_s {
  /** The bit-board position key for this move. */
  uint64_t positionKey;
  /** The move of this entry */
  union move_u move;
  /** The score for this move */
  int32_t score;
  /** The search depth of this move */
  int32_t depth;
  /** Flags used in searching for this move */
  int32_t flags;
};

/**
 * Chess move hash table for transposition tables
 * Chess principal variation table data structure.
 */
struct pvtable_s {
  /** The hash table move entries */
  struct pventry_s *entries;
  /** The capacity of the table */
  size_t count;
  /** Number of new entries written into the table */
  int32_t newWrite;
  /** Number of over written entries into the table */
  int32_t overWrite;
  /** Number of hits when accessing the table */
  int32_t hit;
  /** Number of cuts from the table */
  int32_t cut;
};

/** Chess move undo data structure. */
struct undo_s {
  union {
    /** Integral representation of the chess move. */
    int32_t move;
    /** Chess move union structure for the current move. */
    union move_u fields;
  };

  /** The position of the en passent square for this move, if any. */ 
  int32_t enPassent;
  /** Fifty move counter for this move. */
  int32_t fiftyMove;

  /** Castle permission for this move. */
  int32_t castlePerms;

  /** Bit-board position key for this move. */
  uint64_t positionKey;
};

/** The main chess board data structure. */
struct board_s {
  /** The representation of the chess pieces on the chess board in a 64 square board. */
  int32_t pieces[NUM_BRD_SQ];
  /** The bit-board position of the pawns for each color and both colors. */
  uint64_t pawns[3];                      // the position of the pawns

  /** The 64 square board position for the kings. */
  int32_t kingSq[2];                      // the position of the kings

  /** The value of the current side to move. */
  int32_t side;                           // the current side to move
  /** The position of the en passent square for this board position, if any. */
  int32_t enPassent;                      // en passent square position
  /** The value of the fifty move counter for this board position. */
  int32_t fiftyMove;                      // fifty move counter

  /** The current castling permissions for this board position. */
  int32_t castlePerms;                    // castling permissions

  /** The bit-board position key hash */
  uint64_t positionKey;                   // unique board position key (board hashkey)

  /** The number of pieces remaining on the board broken down by piece and colour. */
  int32_t pieceNum[13];                   // array of number of pieces
  /** The number of big pieces (pieces which are not pawns) on the board by colour. */
  int32_t bigPieces[2];                   // array of pieces containing anything that is not a pawn
  /** The number of major pieces (rooks and queens) on the board by colour. */
  int32_t majPieces[2];                   // array of pieces containing rooks and queens
  /** The number of minor pieces (bishops and knights) on the board by colour. */
  int32_t minPieces[2];                   // array of pieces containing bishops and knights
  /** The numerical value of the material for each colour that remains on the board. */
  int32_t material[2];                    // the value of the material score

  /** The current ply of the game. */
  int32_t ply;                            // current ply
  /** The historical ply number of this board position. */
  int32_t historyPly;                     // historical play number

  /** The undo history for this board position. */
  struct undo_s history[MAX_GAME_MOVES];

  /** The piece list for this board position. */
  int32_t pieceList[13][10];              // piece list
  
  /** Transposition table containing principal variation scores for previously seen moves */
  struct pvtable_s pvtable;               // principal variation table
  /** The principal variation depth array. */
  uint32_t pvarray[MAX_DEPTH];            // principal variation depth array

  /** The alhpa-beta search history for the current board position. */
  int32_t searchHistory[13][NUM_BRD_SQ];  // alpha-beta search history
  /** The beta cuttof moves for the current board position. */
  uint32_t searchKillers[2][MAX_DEPTH];   // beta cuttoff moves
};

/**
 * Chess engine search constraints data structure.
 */
struct search_info_s {
  /** The search start time. */
  int32_t startTime;
  /** The search cutoff/stop time. */
  int32_t stopTime;
  /** The current search depth. */
  int32_t depth;
  /** The search maximum depth. */
  int32_t depthSet;
  /** The search maximum time allotment. */
  int32_t timeSet;
  /* The number of moves to go in this search. */
  int32_t movesToGo;
  /* Infinite search mode flag indicator. */
  int32_t infinite;

  /** The number of nodes examined in the current search. */
  uint32_t nodes;

  /** Protocol mode */
  int32_t gameMode;
  /** Post thinking flag */
  int32_t postThinking;

  /** Search quit flag. */
  int32_t quit;
  /** Search stopped flag. */
  int32_t stopped;

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

#define MIR64(sq)     (tbl_mirror64[(sq)])

#define POP(b)        (ce_pop_bit(b))
#define CNT(b)        (ce_count_bits(b))

#define CLRBIT(b, sq)   ((b) &= tbl_clear_mask[(sq)])
#define SETBIT(b, sq)		((b) |= tbl_set_mask[(sq)])

#define IsPw(p)			((p) != EMPTY && (p) != OFFBOARD && tbl_piece_pawn[(p)])
#define IsKn(p)			((p) != EMPTY && (p) != OFFBOARD && tbl_piece_knight[(p)])
#define IsKi(p)			((p) != EMPTY && (p) != OFFBOARD && tbl_piece_king[(p)])
#define IsRQ(p)			((p) != EMPTY && (p) != OFFBOARD && tbl_piece_rook_queen[(p)])
#define IsBQ(p)			((p) != EMPTY && (p) != OFFBOARD && tbl_piece_bishop_queen[(p)])
 
/* GLOBALS */

// init.c
extern int32_t tbl_sq120_to_sq64[NUM_BRD_SQ];
extern int32_t tbl_sq64_to_sq120[64];

extern uint64_t tbl_set_mask[64];
extern uint64_t tbl_clear_mask[64];

extern uint64_t tbl_piece_keys[13][120];
extern uint64_t side_key;
extern uint64_t tbl_castle_keys[16];

extern int32_t tbl_files_board[NUM_BRD_SQ];
extern int32_t tbl_ranks_board[NUM_BRD_SQ];

extern uint64_t tbl_file_bb_mask[8];
extern uint64_t tbl_rank_bb_mask[8];

extern uint64_t tbl_black_passed_mask[64];
extern uint64_t tbl_white_passed_mask[64];
extern uint64_t tbl_isolated_mask[64];

// data.c
extern const char tbl_piece_char[];
extern const char tbl_side_char[];
extern const char tbl_rank_char[];
extern const char tbl_file_char[];

extern const bool tbl_piece_big[13];
extern const bool tbl_piece_maj[13];
extern const bool tbl_piece_min[13];
extern const int32_t tbl_piece_val[13];
extern const int32_t tbl_piece_col[13];

extern const bool tbl_piece_pawn[13];
extern const bool tbl_piece_knight[13];
extern const bool tbl_piece_king[13];
extern const bool tbl_piece_rook_queen[13];
extern const bool tbl_piece_bishop_queen[13];

extern const bool tbl_piece_slides[13];

extern const int32_t tbl_mirror64[64];

/* FUNCTIONS */

// init.c
extern void ce_init();

// bitboards.c
extern int32_t ce_pop_bit(uint64_t *);
extern int32_t ce_count_bits(uint64_t);

// hashkeys.c
extern uint64_t ce_generate_position_key(const struct board_s *);

// board.c
extern void ce_reset_board(struct board_s *);
extern void ce_mirror_board(struct board_s *);
extern bool ce_parse_fen(char *, struct board_s *);
extern void ce_update_material_list(struct board_s *);
extern bool ce_check_board(const struct board_s *);

// attack.c
extern bool ce_is_square_attacked(const int32_t, const int32_t, const struct board_s *);

// io.c
extern char *ce_print_sq(const int32_t);
extern char *ce_print_move(const union move_u);
extern void ce_print_move_list(const struct move_list_s *);
extern uint32_t ce_parse_move(char *, struct board_s *);

// movegen.c
extern void ce_init_mvv_lva();
extern void ce_generate_capture_moves(const struct board_s *, struct move_list_s *);
extern void ce_generate_all_moves(const struct board_s *, struct move_list_s *);
extern bool ce_move_exists(struct board_s *, const uint32_t);

// validate.c
extern bool ce_valid_square(const int32_t);
extern bool ce_valid_side(const int32_t);
extern bool ce_valid_file_rank(const int32_t);
extern bool ce_valid_piece_empty(const int32_t);
extern bool ce_valid_piece(const int32_t);

extern bool ce_is_fifty_move(const struct board_s *);
extern bool ce_is_three_fold_repetition(const struct board_s *);
extern bool ce_is_draw_material(const struct board_s *);
extern bool ce_is_moves_available(const struct board_s *);

extern void ce_mirror_eval_test(struct board_s *);

// makemove.c
extern void ce_move_take(struct board_s *);
extern void ce_move_take_null(struct board_s *);
extern bool ce_move_make(struct board_s *, uint32_t);
extern bool ce_move_make_null(struct board_s *);

// perft.c
extern size_t ce_perf_test(int32_t, struct board_s *);

// full_perft.c
extern void ce_all_perf_tests();

// search.c
extern void ce_search_position(struct board_s *, struct search_info_s *);

// utils.c
extern int32_t sys_time_ms();
extern int32_t sys_input_waiting();
extern void sys_read_input(struct search_info_s *);

// pvtable.c
extern void ce_pvtable_init(struct pvtable_s *, size_t);
extern void ce_pvtable_free(struct pvtable_s *);
extern void ce_pvtable_clear(struct pvtable_s *);
extern void ce_pvtable_store(struct board_s *, const uint32_t, int32_t, const int32_t, const int32_t);
extern bool ce_pvtable_probe(struct board_s *, uint32_t *, int32_t *, int32_t, int32_t, int32_t);
extern uint32_t ce_pvtable_pvprobe(const struct board_s *);
extern size_t ce_pvtable_get_line(const int32_t, struct board_s *);

// evaluate.c
extern int32_t ce_eval_position(const struct board_s *);

// uci.c
extern void ce_uci_loop(struct board_s *, struct search_info_s *);

// xboard.c
extern bool ce_check_result(struct board_s *);
extern void ce_xboard_loop(struct board_s *, struct search_info_s *);

// console.c
extern void ce_console_loop(struct board_s *, struct search_info_s *);

// data.c

// diag.c
extern void ce_diag_print_tbls();
extern void ce_diag_print_bitboard(uint64_t);
extern void ce_print_board(const struct board_s *);
extern void ce_print_coloured_board(const struct board_s *, int32_t, int32_t, int32_t);
extern void ce_print_fen(const struct board_s *);
extern void ce_diag_show_attacked_by_side(const int32_t, const struct board_s *);
extern void ce_print_binary(int32_t);

#endif
