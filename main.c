
#include "defs.h"

#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
#define FEN4 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define FEN2Q "8/3q4/8/8/4Q3/8/8/8 w - - 0 2"
#define FEN2Q2P "8/3q1p2/8/5P2/4Q3/8/8/8 w - - 0 2"
#define FEN2N "8/8/3n4/8/4N3/8/8/8 w - - 0 2"
#define FEN2R "8/8/3r4/8/5R2/8/8/8 w - - 0 2"
#define FEN2B "8/8/3b4/8/3B4/8/8/8 w - - 0 2"
#define FEN2K "8/3k4/8/8/4K3/8/8/8 w - - 0 2"

void ce_parse_and_print(char *fen, struct board_s *board) {
  ce_parse_fen(fen, board);
  ce_print_board(board);
  // CHKBRD(board);
}

void ce_parse_and_attack(char *fen) {
  struct board_s board = { };
  ce_parse_and_print(fen, &board);
  ce_diag_show_attacked_by_side(WHITE, &board);
  ce_diag_show_attacked_by_side(BLACK, &board);
}

int main() {
  ce_init();
  // ce_diag_print_tbls();

  ce_parse_and_attack(FEN2Q2P);
  ce_parse_and_attack(FEN2N);
  ce_parse_and_attack(FEN2R);
  ce_parse_and_attack(FEN2B);
  ce_parse_and_attack(FEN2K);

  return 0;
}

