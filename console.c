
#include "defs.h"

#define FEN_MATE_IN_1   u8"r1b1k2r/pp3p1p/5p2/3pp2P/1b6/q1R5/P3PPP1/4KBN b --kq - 0 12"
#define FEN_MATE        u8"r1b1k2r/pp3p1p/5p2/3pp2P/1b6/2R5/P3PPP1/2q1KBN w --kq - 0 13"

static void _ce_print_console_board(const struct board_s *pos, bool coloured) {
  if (coloured) {
    ce_print_coloured_board(pos, CLR_YELLOW, CLR_RED, CLR_GREEN);
  } else {
    ce_print_board(pos);
  }
}

void ce_console_loop(struct board_s *pos, struct search_info_s *info) {
  int32_t depth = MAX_DEPTH, moveTime = 8000;
  int32_t engineSide = BOTH;
  uint32_t move = NOMOVE;
  bool coloured = true;
  bool playing = false;
  bool autoPlay = false;
  char line[256] = { 0 }, command[256] = { 0 };
  char fen[256] = { 0 };
  int32_t i = 0;

  printf(u8"Chess Engine\n");

  info->gameMode = MODE_CONSOLE;
  info->postThinking = true;

  setbuf(stdin, NULL);
  setbuf(stdout, NULL);

  engineSide = BLACK;
  ce_parse_fen(START_FEN, pos);

  do {
    fflush(stdout);

    if (playing && (autoPlay || pos->side == engineSide) && (playing = !ce_check_result(pos)) == true) {
      info->startTime = sys_time_ms();
      info->depth = depth;

      if (moveTime != 0) {
        info->timeSet = true;
        info->stopTime = info->startTime + moveTime;
      }

      ce_search_position(pos, info);
      // Print the board position after searching to make a move either coloured or not
      _ce_print_console_board(pos, coloured);

      if (autoPlay) {
        continue;
      }
    }

    printf(u8"> ");

    fflush(stdout);

    memset((void *) line, 0, sizeof(line));
    fflush(stdout);

    if (!fgets(line, 256, stdin)) {
      continue;
    }

    sscanf(line, u8"%s", command);

    if (!strncmp(command, u8"help", 4)) {
      printf(u8"\nCommands:\n");
      printf(u8"==============================\n");
      printf(u8"quit - quit the game\n");
      printf(u8"new - start a new game\n");
      printf(u8"go - have the computer make the next move\n");
      printf(u8"undo - undo the previous move\n");
      printf(u8"print - print the current board position\n");
      printf(u8"history - print the move history for the current game\n");
      printf(u8"fen - print the current board position FEN string\n");
      printf(u8"load [fen] - load the provided FEN string as the current board position\n");
      printf(u8"force - computer will not think\n");
      printf(u8"auto - the computer will play out the rest of the game\n");
      printf(u8"depth x - set the search depth to x\n");
      printf(u8"time x - set the search time to x (seconds)\n");
      printf(u8"view - show the current depth and move time settings\n");
      printf(u8"post - print the computer thinking\n");
      printf(u8"nopost - do not show the computer thinking\n");
      printf(u8"colour - enable printing the board position in colour\n");
      printf(u8"nocolour - disable printing the board position in colour\n\n");
      printf(u8"NOTE: to reset depth and time set them to 0\n\n");
      printf(u8"moves are entered using algebraic notation (e.g.: pe2e4)\n\n");
      continue;
    } else if (!strncmp(command, u8"quit", 4)) {
      info->quit = true;
      break;
    } else if (!strncmp(command, u8"post", 4)) {
      info->postThinking = true;
      continue;
    } else if (!strncmp(command, u8"fen", 3)) {
      ce_print_fen(pos);
      continue;
    } else if (!strncmp(command, u8"load", 4)) {
      if (ce_parse_fen(line + 5, pos)) {
        printf(u8"Successfully loaded FEN position\n\n");
        playing = true;
        _ce_print_console_board(pos, coloured);
        engineSide = pos->side ^ 1;
      }
      continue;
    } else if (!strncmp(command, u8"print", 5)) {
      _ce_print_console_board(pos, coloured);
      continue;
    } else if (!strncmp(command, u8"mirror", 6)) {
      _ce_print_console_board(pos, coloured);
      printf("Evaluation: %d\n", ce_eval_position(pos));
      ce_mirror_board(pos);
      _ce_print_console_board(pos, coloured);
      printf("Evaluation: %d\n", ce_eval_position(pos));
      ce_mirror_board(pos);
      continue;
    } else if (!strncmp(command, u8"history", 7)) {
      printf(u8"\n");
      for (i = 0; i < pos->historyPly; ++i) {
        printf(u8"%3d)  %s\n", (i + 1), ce_print_move(pos->history[i].fields));
      }
      printf(u8"\n");
      continue;
    } else if (!strncmp(command, u8"nopost", 6)) {
      info->postThinking = false;
      continue;
    } else if (!strncmp(command, u8"force", 5)) {
      engineSide = BOTH;
      continue;
    } else if (!strncmp(command, u8"auto", 4)) {
      playing = true;
      autoPlay = true;
      continue;
    } else if (!strncmp(command, u8"view", 4)) {
      if (depth == MAX_DEPTH) {
        printf(u8"depth not set ");
      } else {
        printf(u8"depth %d ", depth);
      }

      if (moveTime != 0) {
        printf(u8"move time %ds\n", (moveTime / 1000));
      } else {
        printf(u8"move time not set\n");
      }

      printf(u8"engine side: %s\n", engineSide == WHITE ? u8"white" : engineSide == BLACK ? u8"black" : u8"both");

      continue;
    } else if (!strncmp(command, u8"depth", 5)) {
      sscanf(line, u8"depth %d", &depth);
      if (depth == 0) {
        depth = MAX_DEPTH;
      }
      continue;
    } else if (!strncmp(command, u8"time", 4)) {
      sscanf(line, u8"time %d", &moveTime);
      moveTime *= 1000;
      continue;
    } else if (!strncmp(command, u8"new", 3)) {
      playing = true;
      engineSide = BLACK;
      ce_parse_fen(START_FEN, pos);
      printf(u8"\n");
      _ce_print_console_board(pos, coloured);
      continue;
    } else if (!strncmp(command, u8"go", 2)) {
      engineSide = pos->side;
      continue;
    } else if (!strncmp(command, u8"undo", 4)) {
      ce_move_take(pos);
      ce_move_take(pos);
      _ce_print_console_board(pos, coloured);
      continue;
    } else if (!strncmp(command, u8"colour", 6)) {
      coloured = true;
      continue;
    } else if (!strncmp(command, u8"nocolour", 8)) {
      coloured = false;
      continue;
    }

    if ((move = ce_parse_move(line, pos)) == NOMOVE) {
      printf(u8"Unknown command: %s\n", line);
      continue;
    }

    if (ce_move_make(pos, move)) {
      _ce_print_console_board(pos, coloured);
    } else {
      playing = !ce_check_result(pos);
    }
    pos->ply = 0;
  } while (!info->quit);
}
