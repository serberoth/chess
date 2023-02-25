
#include <assert.h>

#include "defs.h"

/**
 * Chess Engine structure representing a performance test line.
 */
struct perf_line_s {
  /** The chess position FEN string representing the board position for the performance test line. */
  char *fen;
  /** The length of the position FEN string. */
  size_t fen_length;
  /** An array of 24 unsigned long values containing the tree node count of the position. */
  size_t node_count[24];
};

/**
 * Chess Engine performance function that reads a performance test line from the provided FILE
 * pointer into the provided performance line structure pointer.
 * @param file A pointer to a FILE resource from which to read the performance test line.
 * @param line A pointer to a performance line structure where to load the performance test content.
 * @return Boolean value indicating the success status of this function.
 */
static bool _ce_perft_read_line(FILE *file, struct perf_line_s *line) {
  char str[2048] = { 0 };
  int32_t length = 0, index = 0;
  int32_t node_index = 0;
  unsigned long node_count = 0UL;
  char ch = 0;

  while ((ch = fgetc(file)) != '\n' && ch != EOF && length < 2048) {
    str[length++] = ch;
  }

  if (length == 0 || length >= 2048) {
    return false;
  }
  str[length] = '\0';
  printf(u8"Perf Test: %s\n", str);

  if (line->fen != NULL) {
    free(line->fen);
    line->fen = NULL;
  }

  for (line->fen_length = 0ull; str[line->fen_length] != ';' && str[line->fen_length] != '\0'; ++line->fen_length);

  line->fen = (char *) calloc(sizeof(char), line->fen_length + 1);
  memcpy(line->fen, str, line->fen_length);
  printf(u8"FEN: '%s'\n", line->fen);

  for (size_t index = line->fen_length + 1; index < length; ) {
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
        node_count *= 10ull;
        node_count += (ch - '0');
      }
      line->node_count[node_index] = node_count;
      printf(u8"Test[%3d] %lu\n", node_index, node_count);
    } else {
      ++index;
    }
  }

  return true;
}

/**
 * Chess Engine performance function that executes and evaluates all performance tests from the
 * perfsuite.epd file located in the same directory as the executable.
 */
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ;D1 20 ;D2 400 ;D3 8902 ;D4 197281 ;D5 4865609 ;D6 119060324
void ce_all_perf_tests() {
  const char *filename = u8"perfsuite.epd";
  const size_t max_depth = 24ull;

  FILE *file = NULL;

  if ((file = fopen(filename, u8"rt")) == NULL) {
    return;
  }

  while (feof(file) == 0) {
    struct perf_line_s line = { 0 };
    if (!_ce_perft_read_line(file, &line)) {
      continue;
    }

    struct board_s board = { 0 };
    ce_parse_fen(line.fen, &board);
    ce_print_board(&board);
    CHKBRD(&board);

    for (size_t index = 0ull; index < max_depth; ++index) {
      if (line.node_count[index] > 0) {
        printf(u8"Testing %3lu for %10lu nodes\n", index + 1, line.node_count[index]);
        size_t node_count = ce_perf_test(index + 1, &board);
        // ASSERT(node_count == line.node_count[index]);
        assert(node_count == line.node_count[index]);
      }
    }

    if (line.fen != NULL) {
      free(line.fen);
    }
    memset((void *) &line, 0, sizeof(struct perf_line_s));
  }

  fclose(file);
}
