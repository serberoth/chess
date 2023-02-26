
#include "defs.h"

#define FEN1          u8"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2          u8"rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3          u8"rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
#define FEN4          u8"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

#define PAWNMOVESW    u8"rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define PAWNMOVESB    u8"rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1"
#define KNIGHTSKINGS  u8"5k2/1n6/4n3/6N1/8/3N4/8/5K2 w - - 0 1"
#define ROOKS         u8"6k1/8/5r2/8/1nR5/5N2/8/6K1 w - - 0 1"
#define QUEENS        u8"6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 w - - 0 1"
#define BISHOPS       u8"6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 b - - 0 1"
#define CASTLE1       u8"r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1"
#define CASTLE2       u8"3rk2r/8/8/8/8/8/6p1/R3K2R b KQk - 0 1"
#define CASTLE3       u8"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define PERFTFEN      u8"n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1"

#define WAC1          u8"2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -"
#define WAC2          u8"r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"

/**
 * Parse the provided FEN and setup the provided board state then print
 * the current board state out to the standard output.
 * @param fen A string containing the FEN formatted chess position
 * @param board A pointer to a board state that will be modified to
 *   contain the state of the provided position
 */
void ce_parse_and_print(char *fen, struct board_s *board) {
  ce_parse_fen(fen, board);
  ce_print_board(board);
  CHKBRD(board);
}

/**
 * Main entry method to the chess engine currently processing the UCI chess protocol
 * @return The main return method (typically 0)
 */
int main(int argc, char **argv) {
  struct board_s pos = { 0 };
  struct search_info_s info = { 0 };
  char line[256] = { 0 };

  ce_init();

  ce_pvtable_init(&pos.pvtable, 16);

  do {
    memset((void *) line, 0, sizeof(line));

    fflush(stdout);

    if (!fgets(line, 256, stdin)) {
      continue;
    } else if (line[0] == '\n') {
      continue;
      
    // TOP: Engine testing functions
    } else if (!strncmp(line, u8"perf", 4)) {
      ce_all_perf_tests();
      break;
    } else if (!strncmp(line, u8"mirror", 6)) {
      ce_mirror_eval_test(&pos);
      break;
    // END: Engine testing functions

    } else if (!strncmp(line, u8"uci", 3)) {
      ce_uci_loop(&pos, &info);
      if (info.quit == true) {
        break;
      }
      continue;
    } else if (!strncmp(line, u8"xboard", 6)) {
      ce_xboard_loop(&pos, &info);
      if (info.quit == true) {
        break;
      }
      continue;
    } else if (!strncmp(line, u8"console", 7)) {
      ce_console_loop(&pos, &info);
      if (info.quit == true) {
        break;
      }
      continue;
    } else if (!strncmp(line, u8"quit", 4)) {
      break;
    }
  } while (info.quit == false);

  ce_pvtable_free(&pos.pvtable);

  return 0;
}
