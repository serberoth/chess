
#include <stdio.h>
#include <string.h>
#include "defs.h"

void ce_console_loop(struct board_s *pos, struct search_info_s *info) {
  int depth = MAX_DEPTH, moveTime = 3000;
  int engineSide = BOTH;
  int move = NOMOVE;
  char line[80] = { 0 }, command[80] = { 0 };

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
    }

    printf("?> ");

    fflush(stdout);

    memset((void *) line, 0, sizeof(line));
    fflush(stdout);

    if (!fgets(line, 80, stdin)) {
      continue;
    }

    sscanf(line, "%s", command);

    if (!strncmp(command, "help", 4)) {
      printf("Commands:\n");
      printf("quit - quit the game\n");
      printf("force - computer will not think\n");
      printf("print - print the current board position\n");
      printf("post - print the computer thinking\n");
      printf("nopost - do not show the computer thinking\n");
      printf("new - start a new game\n");
      printf("go - start the computer thinking\n");
      printf("depth x - set the search depth to x\n");
      printf("time x - set the search time to x (seconds)\n");
      printf("view - show the current depth and move time settings\n");
      printf("** note ** - to reset depth and time set them to 0\n");
      printf("moves are entered using mathematical notation (e.g.: pe2e4)\n\n\n");
    } else if (!strncmp(command, "quit", 4)) {
      info->quit = TRUE;
      break;
    } else if (!strncmp(command, "post", 4)) {
      info->postThinking = TRUE;
      continue;
    } else if (!strncmp(command, "print", 5)) {
      ce_print_board(pos);
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
      continue;
    } else if (!strncmp(command, "go", 2)) {
      engineSide = pos->side;
      continue;
    }

    if ((move = ce_parse_move(line, pos)) == NOMOVE) {
      printf("Unknown command: %s\n", line);
      continue;
    }

    ce_move_make(pos, move);
    pos->ply = 0;
  } while (!info->quit);
}

