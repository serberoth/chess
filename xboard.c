
#include <stdio.h>
#include <string.h>
#include "defs.h"

int ce_three_fold_repetition(const struct board_s *pos) {
  int i = 0, r = 0;
  for (i = 0; i < pos->historyPly; ++i) {
    if (pos->history[i].positionKey == pos->positionKey) {
      ++r;
    }
  }
  return r;
}

int ce_draw_material(const struct board_s *pos) {
  if (pos->pieceNum[wP] || pos->pieceNum[bP]) {
    return FALSE;
  }
  if (pos->pieceNum[wQ] || pos->pieceNum[bQ] || pos->pieceNum[wR] || pos->pieceNum[bR]) {
    return FALSE;
  }
  if (pos->pieceNum[wB] > 1 || pos->pieceNum[bB] > 1) {
    return FALSE;
  }
  if (pos->pieceNum[wN] > 1 || pos->pieceNum[bN] > 1) {
    return FALSE;
  }
  if (pos->pieceNum[wN] && pos->pieceNum[wB]) {
    return FALSE;
  }
  if (pos->pieceNum[bN] && pos->pieceNum[bB]) {
    return FALSE;
  }
  return TRUE;
}

int ce_checkresult(struct board_s *pos) {
  struct move_list_s list = { 0 };
  int move_num = 0;
  int found = 0;
  int in_check = 0;

  if (pos->fiftyMove > 100) {
    printf("1/2-1/2 (fifty move rule)\n");
    return TRUE;
  }
  if (ce_three_fold_repetition(pos) >= 2) {
    printf("1/2-1/2 (3-fold repetition)\n");
    return TRUE;
  }
  if (ce_draw_material(pos) == TRUE) {
    printf("1/2-1/2 (insuficient material)\n");
    return TRUE;
  }

  ce_generate_all_moves(pos, &list);
  for (move_num = 0; move_num < list.count; ++move_num) {
    if (!ce_move_make(pos, list.moves[move_num].move)) {
      continue;
    }
    ++found;
    ce_move_take(pos);
    break;
  }

  if (found != 0) {
    return FALSE;
  }

  in_check = ce_is_square_attacked(pos->kingSq[pos->side], pos->side ^ 1, pos);
  if (in_check == TRUE) {
    if (pos->side == WHITE) {
      printf("0-1 (black mate)\n");
      return TRUE;
    }
    if (pos->side == BLACK) {
      printf("0-1 (white mate)\n");
      return TRUE;
    }
  } else {
    printf("\n1/2-1/2 (stalemate)\n");
    return TRUE;
  }
  return FALSE;
}

void ce_xboard_loop(struct board_s *pos, struct search_info_s *info) {
  int depth = -1, moves_to_go = 30, move_time = -1;
  int time = -1, inc = 0;
  int engine_side = BOTH;
  int time_left = 0;
  int moves_per_session = 0;
  int move = NOMOVE;
  int i = 0, score = 0;
  char line[80], command[80];

  setbuf(stdin, NULL);
  setbuf(stdout, NULL);

  do {
    fflush(stdout);

    if (pos->side == engine_side) {
      // think
    }

    fflush(stdout);

    memset((void *) line, 0, sizeof(line));
    fflush(stdout);
    if (!fgets(line, 80, stdin)) {
      continue;
    }

    sscanf(line, "%s", command);

    if (!strncmp(command, "quit", 4)) {
      info->quit = TRUE;
      break;
    } else if (!strncmp(command, "force", 5)) {
      engine_side = BOTH;
      continue;
    } else if (!strncmp(command, "protover", 8)) {
      printf("feature ping=1 setboard=1 colors=0 usermove=1\n");
      printf("feature done=1\n");
    } else if (!strncmp(command, "sd", 2)) {
      sscanf(line, "sd %d", &depth);
      continue;
    } else if (!strncmp(command, "st", 2)) {
      sscanf(line, "st %d", &move_time);
      continue;
    } else if (!strncmp(command, "ping", 4)) {
      printf("pong%s\n", line+4);
      continue;
    } else if (!strncmp(command, "new", 3)) {
      engine_side = BLACK;
      ce_parse_fen(START_FEN, pos);
      depth = -1;
      continue;
    } else if (!strncmp(command, "setboard", 8)) {
      engine_side = BOTH;
      ce_parse_fen(line + 9, pos);
      continue;
    } else if (!strncmp(command, "go", 2)) {
      engine_side = pos->side;
      continue;
    } else if (!strncmp(command, "usermove", 8)) {
      if ((move = ce_parse_move(line + 9, pos)) == NOMOVE) {
        continue;
      }
      ce_move_make(pos, move);
      pos->ply = 0;
    }
  } while (!info->quit);
}

