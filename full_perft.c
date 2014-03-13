
#include <assert.h>
#include <string.h>
#include "defs.h"

struct perf_line_s {
  char *fen;
  int fen_length;
  unsigned long node_count[24];
};

static int _ce_perft_read_line(FILE *file, struct perf_line_s *line) {
  char str[2048] = { 0 };
  int length = 0, index = 0;
  int node_index = 0;
  unsigned long node_count = 0UL;
  char ch = 0;

  while ((ch = fgetc(file)) != '\n' && ch != EOF && length < 2048) {
    str[length++] = ch;
  }

  if (length == 0 || length >= 2048) {
    return FALSE;
  }
  str[length] = '\0';
  printf("Perf Test: %s\n", str);

  if (line->fen != NULL) {
    free(line->fen);
    line->fen = NULL;
  }

  for (line->fen_length = 0; str[line->fen_length] != ';'; ++line->fen_length);

  line->fen = (char *) malloc(sizeof(char) * line->fen_length + 1);
  memset(line->fen, 0, line->fen_length + 1);
  memcpy(line->fen, str, line->fen_length);
  printf("FEN: '%s'\n", line->fen);

  for (index = line->fen_length + 1; index < length; ) {
    ch = str[index];

    if (ch == 'D') {
      ch = str[++index];
      node_index = 0;
      ASSERT(ch >= '0' && ch <= '9');
      for ( ; ch >= '0' && ch <= '9' && index < length; ch = str[++index]) {
        node_index *= 10;
        node_index += (ch - '0');
      }
      node_index--;
    } else if (ch >= '0' && ch <= '9') {
      node_count = 0;
      for ( ; ch >= '0' && ch <= '9' && index < length; ch = str[++index]) {
        node_count *= 10UL;
        node_count += (ch - '0');
      }
      line->node_count[node_index] = node_count;
      printf("Test[%3d] %lu\n", node_index, node_count);
    } else {
      ++index;
    }
  }

  return TRUE;
}

// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ;D1 20 ;D2 400 ;D3 8902 ;D4 197281 ;D5 4865609 ;D6 119060324
void ce_all_perf_tests() {
  const char *filename = "perfsuite.epd";
  struct board_s board;
  struct perf_line_s line;
  int index;
  unsigned long node_count;
  FILE *file;

  if ((file = fopen(filename, "rt")) == NULL) {
    return;
  }

  while (feof(file) == 0) {
    if (!_ce_perft_read_line(file, &line)) {
      continue;
    }

    ce_parse_fen(line.fen, &board);
    ce_print_board(&board);
    CHKBRD(&board);

    for (index = 0; index < 24; ++index) {
      if (line.node_count[index] > 0) {
        printf("Testing %3d for %10lu nodes\n", index + 1, line.node_count[index]);
        // node_count = ce_perf_test(index + 1, &board);
        // ASSERT(node_count == line.node_count[index]);
        // assert(node_count == line.node_count[index]);
      }
    }
  }

  if (line.fen != NULL) {
    free(line.fen);
    line.fen = NULL;
  }

  fclose(file);
}
 
