
#include <string.h>
#include "defs.h"

#define INPUTBUFFER	400 * 6

/**
 * Chess Engine parsing function to parse the provided string according to the UCI
 * chess engine protocol for a board position and configure the provided board instance to
 * the position indicated in the string.
 * @param lineIn A string with the board position according to the UCI chess engine protocol.
 * @param pos A pointer to a chess board position structure to configure the position.
 */
/*
 * position fen
 * position startpos
 * ... moves e2e4 e7e5 b7b8q
 */
static void _ce_parse_position(char *lineIn, struct board_s *pos) {
  char *ptrChar;
  int move = NOMOVE;

  lineIn += 9;
  ptrChar = lineIn;

  if (strncmp(lineIn, "startpos", 8) == 0) {
    ce_parse_fen(START_FEN, pos);
  } else {
    ptrChar = strstr(lineIn, "fen");
    if (ptrChar == NULL) {
      ce_parse_fen(START_FEN, pos);
    } else {
      ptrChar += 4;
      ce_parse_fen(ptrChar, pos);
    }
  }

  ptrChar = strstr(lineIn, "moves");

  if (ptrChar != NULL) {
    ptrChar += 6;
    while(*ptrChar) {
      if ((move = ce_parse_move(ptrChar, pos)) == NOMOVE) {
        break;
      }
      ce_move_make(pos, move);
      pos->ply = 0;
      while(*ptrChar && *ptrChar != ' ') {
        ++ptrChar;
      }
      ++ptrChar;
    }
  }

  ce_print_board(pos);
}

/**
 * Chess Engine parsing function to parse the UCI chess engine protocol go message.
 * [INTERNAL]
 * @param line A string containing the UCI protocol go message.
 * @param info A pointer to a search information struct to configure search parameters.
 * @param pos A pointer to the current board position.
 */
// go depth 6 wtime 180000 btime 100000 binc 1000 winc 1000 movetime 1000 movestogo 40
static void _ce_parse_go(char *line, struct search_info_s *info, struct board_s *pos) {
  int depth = -1, movestogo = 30, movetime = -1;
  int time = -1, inc = 0;
  char *ptr = NULL;

  info->timeSet = FALSE;

  if ((ptr = strstr(line, "infinite"))) {
    ;
  }

  if ((ptr = strstr(line, "binc")) && pos->side == BLACK) {
    inc = atoi(ptr + 5);
  }

  if ((ptr = strstr(line, "winc")) && pos->side == WHITE) {
    inc = atoi(ptr + 5);
  }

  if ((ptr = strstr(line, "wtime")) && pos->side == WHITE) {
    time = atoi(ptr + 6);
  }

  if ((ptr = strstr(line, "btime")) && pos->side == BLACK) {
    time = atoi(ptr + 6);
  }

  if ((ptr = strstr(line, "movestogo")) != NULL) {
    movestogo = atoi(ptr + 10);
  }

  if ((ptr = strstr(line, "movetime")) != NULL) {
    movetime = atoi(ptr + 9);
  }

  if ((ptr = strstr(line, "depth")) != NULL) {
    depth = atoi(ptr + 6);
  }

  if (movetime != -1) {
    time = movetime;
    movestogo = 1;
  }

  info->startTime = sys_time_ms();
  info->depth = depth;

  if (time != -1) {
    info->timeSet = TRUE;
    time /= movestogo;
    // Take 50ms off the time so as not to overrun
    time -= 50;
    info->stopTime = info->startTime + time + inc;
  }

  if (depth == -1) {
    info->depth = MAX_DEPTH;
  }

  printf("time: %d start: %d stop: %d depth: %d timeset: %d\n",
    time, info->startTime, info->stopTime, info->depth, info->timeSet);
  ce_search_position(pos, info);
}

/**
 * Chess Engine UCI chess engine protocol function that implements the main loop
 * for processing messages according to the UCI chess engine protocol.
 * <http://wbec-ridderkerk.nl/html/UCIProtocol.html>
 */
void ce_uci_loop(struct board_s *pos, struct search_info_s *info) {
  char line[INPUTBUFFER] = { 0 };
  
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);

  info->gameMode = MODE_UCI;
  info->postThinking = TRUE;

  printf("id name %s\n", NAME);
  printf("id author %s\n", AUTHOR);
  printf("uciok\n");

  do {
    memset((void *) line, 0, sizeof(line));
    fflush(stdout);

    if (!fgets(line, INPUTBUFFER, stdin)) {
      continue;
    }

    if (line[0] == '\n') {
      continue;
    }

    if (!strncmp(line, "isready", 7)) {
      printf("readyok\n");
      continue;
    } else if (!strncmp(line, "position", 8)) {
      _ce_parse_position(line, pos);
    } else if (!strncmp(line, "ucinewgame", 10)) {
      _ce_parse_position("position startpos\n", pos);
    } else if (!strncmp(line, "go", 2)) {
      _ce_parse_go(line, info, pos);
    } else if (!strncmp(line, "quit", 4)) {
      info->quit = TRUE;
      break;
    } else if (!strncmp(line, "uci", 3)) {
      printf("id name %s\n", NAME);
      printf("id author %s\n", AUTHOR);
      printf("uciok\n");
    }
  } while (!info->quit);
}

