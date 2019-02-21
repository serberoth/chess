
#include <stdio.h>
#include <string.h>
#include "defs.h"

#define FEN_MATE_IN_1		"r1b1k2r/pp3p1p/5p2/3pp2P/1b6/q1R5/P3PPP1/4KBN b --kq - 0 12"
#define FEN_MATE		"r1b1k2r/pp3p1p/5p2/3pp2P/1b6/2R5/P3PPP1/2q1KBN w --kq - 0 13"

static void _ce_print_console_board(const struct board_s *pos, int coloured) {
  if (coloured) {
    ce_print_coloured_board(pos, CLR_YELLOW, CLR_RED, CLR_GREEN);
  } else {
    ce_print_board(pos);
  }
}

void ce_console_loop(struct board_s *pos, struct search_info_s *info) {
  int depth = MAX_DEPTH, moveTime = 8000;
  int engineSide = BOTH;
  int move = NOMOVE;
  int coloured = TRUE;
  char line[256] = { 0 }, command[256] = { 0 };
  char fen[256] = { 0 };

  printf("Chess Engine\n");

  info->gameMode = MODE_CONSOLE;
  info->postThinking = TRUE;

  setbuf(stdin, NULL);
  setbuf(stdout, NULL);

  engineSide = BLACK;
  ce_parse_fen(START_FEN, pos);

  do {
    fflush(stdout);

    if (pos->side == engineSide && ce_check_result(pos) == FALSE) {
      info->startTime = sys_time_ms();
      info->depth = depth;

      if (moveTime != 0) {
        info->timeSet = TRUE;
        info->stopTime = info->startTime + moveTime;
      }

      ce_search_position(pos, info);
      // Print the board position after searching to make a move either coloured or not
      _ce_print_console_board(pos, coloured);
    }

    printf("> ");

    fflush(stdout);

    memset((void *) line, 0, sizeof(line));
    fflush(stdout);

    if (!fgets(line, 256, stdin)) {
      continue;
    }

    sscanf(line, "%s", command);

    if (!strncmp(command, "help", 4)) {
      printf("\nCommands:\n");
      printf("==============================\n");
      printf("quit - quit the game\n");
      printf("new - start a new game\n");
      printf("go - have the computer make the next move\n");
      printf("print - print the current board position\n");
      printf("fen - print the current board position FEN string\n");
      printf("load [fen] - load the provided FEN string as the current board position\n");
      printf("force - computer will not think\n");
      printf("depth x - set the search depth to x\n");
      printf("time x - set the search time to x (seconds)\n");
      printf("view - show the current depth and move time settings\n");
      printf("post - print the computer thinking\n");
      printf("nopost - do not show the computer thinking\n");
      printf("colour - enable printing the board position in colour\n");
      printf("nocolour - disable printing the board position in colour\n\n");
      printf("NOTE: to reset depth and time set them to 0\n\n");
      printf("moves are entered using algebraic notation (e.g.: pe2e4)\n\n");
      continue;
    } else if (!strncmp(command, "quit", 4)) {
      info->quit = TRUE;
      break;
    } else if (!strncmp(command, "post", 4)) {
      info->postThinking = TRUE;
      continue;
    } else if (!strncmp(command, "fen", 3)) {
      ce_print_fen(pos);
      continue;
    } else if (!strncmp(command, "load", 4)) {
      if (!ce_parse_fen(line + 5, pos)) {
        printf("Successfully loaded FEN position\n\n");
        _ce_print_console_board(pos, coloured);
      }
      continue;
    } else if (!strncmp(command, "print", 5)) {
      _ce_print_console_board(pos, coloured);
      continue;
    } else if (!strncmp(command, "nopost", 6)) {
      info->postThinking = FALSE;
      continue;
    } else if (!strncmp(command, "force", 5)) {
      engineSide = BOTH;
      continue;
    } else if (!strncmp(command, "view", 4)) {
      if (depth == MAX_DEPTH) {
        printf("depth not set ");
      } else {
        printf("depth %d ", depth);
      }

      if (moveTime != 0) {
        printf("move time %ds\n", (moveTime / 1000));
      } else {
        printf("move time not set\n");
      }

      printf("engine side: %s\n", engineSide == WHITE ? "white" : engineSide == BLACK ? "black" : "both");

      continue;
    } else if (!strncmp(command, "depth", 5)) {
      sscanf(line, "depth %d", &depth);
      if (depth == 0) {
        depth = MAX_DEPTH;
      }
      continue;
    } else if (!strncmp(command, "time", 4)) {
      sscanf(line, "time %d", &moveTime);
      moveTime *= 1000;
      continue;
    } else if (!strncmp(command, "new", 3)) {
      engineSide = BLACK;
      ce_parse_fen(START_FEN, pos);
      _ce_print_console_board(pos, coloured);
      continue;
    } else if (!strncmp(command, "go", 2)) {
      engineSide = pos->side;
      continue;
    } else if (!strncmp(command, "colour", 6)) {
      coloured = TRUE;
      continue;
    } else if (!strncmp(command, "nocolour", 8)) {
      coloured = FALSE;
      continue;
    }

    if ((move = ce_parse_move(line, pos)) == NOMOVE) {
      printf("Unknown command: %s\n", line);
      continue;
    }

    if (ce_move_make(pos, move)) {
      _ce_print_console_board(pos, coloured);
    } else {
      ce_check_result(pos);
    }
    pos->ply = 0;
  } while (!info->quit);
}

