
#include "defs.h"

#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"

int main() {
  struct board_s board = { };

  ce_init();

  ce_parse_fen(START_FEN, &board);
  ce_print_board(&board);

  ce_parse_fen(FEN1, &board);
  ce_print_board(&board);

  ce_parse_fen(FEN2, &board);
  ce_print_board(&board);

  ce_parse_fen(FEN3, &board);
  ce_print_board(&board);

  return 0;
}

