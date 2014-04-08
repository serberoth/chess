
#include "defs.h"

#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
#define FEN4 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

#define PAWNMOVESW "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define PAWNMOVESB "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1"
#define KNIGHTSKINGS "5k2/1n6/4n3/6N1/8/3N4/8/5K2 w - - 0 1"
#define ROOKS "6k1/8/5r2/8/1nR5/5N2/8/6K1 w - - 0 1"
#define QUEENS "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 w - - 0 1"
#define BISHOPS "6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 b - - 0 1"
#define CASTLE1 "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1"
#define CASTLE2 "3rk2r/8/8/8/8/8/6p1/R3K2R b KQk - 0 1"
#define CASTLE3 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define PERFTFEN "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1"

#define WAC1 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -"
#define WAC2 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"

void ce_parse_and_print(char *fen, struct board_s *board) {
  ce_parse_fen(fen, board);
  ce_print_board(board);
  CHKBRD(board);
}

int main() {
  struct board_s board = { 0 };
  struct search_info_s info = { 0 };
  char input[6] = { 0 };
  int move = 0;

  ce_init();

  // ce_parse_and_print(PERFTFEN, &board);
  // ce_parse_fen(START_FEN, &board);
  ce_parse_fen(WAC2, &board);

  while (TRUE) {
    int move = NOMOVE;

    ce_print_board(&board);
    printf("Move:> ");
    fgets(input, 6, stdin);

    if (input[0] == 'q' || input[0] == 'Q') {
      break;
    }

    switch (input[0]) {
    case 'u':
    case 'U':
      ce_take_move(&board);
      break;

    case 't':
    case 'T': {
      int depth = 4;
      if (input[1] >= '2' && input[1] <= '8') {
        depth = input[1] - '0';
      }
      ce_perf_test(depth, &board);
    } break;

    case 's':
    case 'S': {
      info.depth = 6;
      ce_search_position(&board, &info);
    } break;

    case 'p':
    case 'P': {
      int max = ce_pvtable_get_line(4, &board);
      int index = 0;
      
      printf("PvLine of %d moves: ", max);
      for (index = 0; index < max; ++index) {
        union move_u fields;
        fields.val = board.pvarray[index];
        printf(" %s", ce_print_move(fields));
      }
      printf("\n");
    } break;

    default:
      move = ce_parse_move(input, &board);
      if (move != NOMOVE) {
        ce_pvtable_store(&board, move);
        ce_make_move(&board, move);
        /*
        if (_ce_is_repetition(&board)) {
          printf("Position repeated\n");
        }
         */
      } else {
        printf("Invalid move: %s\n", input);
      }
    }

    fflush(stdin);
  }

  ce_pvtable_free(&board.pvtable);

  return 0;
}

