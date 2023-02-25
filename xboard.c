
#include "defs.h"

int32_t ce_three_fold_repetition(const struct board_s *pos) {
  int32_t i = 0, r = 0;
  for (i = 0; i < pos->historyPly; ++i) {
    if (pos->history[i].positionKey == pos->positionKey) {
      ++r;
    }
  }
  return r;
}

int32_t ce_draw_material(const struct board_s *pos) {
  if (pos->pieceNum[wP] || pos->pieceNum[bP]) {
    return false;
  }
  if (pos->pieceNum[wQ] || pos->pieceNum[bQ] || pos->pieceNum[wR] || pos->pieceNum[bR]) {
    return false;
  }
  if (pos->pieceNum[wB] > 1 || pos->pieceNum[bB] > 1) {
    return false;
  }
  if (pos->pieceNum[wN] > 1 || pos->pieceNum[bN] > 1) {
    return false;
  }
  if (pos->pieceNum[wN] && pos->pieceNum[wB]) {
    return false;
  }
  if (pos->pieceNum[bN] && pos->pieceNum[bB]) {
    return false;
  }
  return true;
}

bool ce_check_result(struct board_s *pos) {
  struct move_list_s list = { 0 };
  int32_t found = 0;

  if (pos->fiftyMove > 100) {
    printf(u8"1/2-1/2 (fifty move rule)\n");
    return true;
  }
  if (ce_three_fold_repetition(pos) >= 2) {
    printf(u8"1/2-1/2 (3-fold repetition)\n");
    return true;
  }
  if (ce_draw_material(pos) == true) {
    printf(u8"1/2-1/2 (insuficient material)\n");
    return true;
  }

  ce_generate_all_moves(pos, &list);
  for (size_t moveNum = 0; moveNum < list.count; ++moveNum) {
    if (!ce_move_make(pos, list.moves[moveNum].move)) {
      continue;
    }
    ++found;
    ce_move_take(pos);
    break;
  }

  if (found != 0) {
    return false;
  }

  bool inCheck = ce_is_square_attacked(pos->kingSq[pos->side], pos->side ^ 1, pos);
  if (inCheck == true) {
    if (pos->side == WHITE) {
      printf(u8"0-1 (black mate)\n");
      return true;
    }
    if (pos->side == BLACK) {
      printf(u8"0-1 (white mate)\n");
      return true;
    }
  } else {
    printf(u8"\n1/2-1/2 (stalemate)\n");
    return true;
  }
  return false;
}

void ce_xboard_loop(struct board_s *pos, struct search_info_s *info) {
  int32_t depth = -1, movesToGo[2] = { 30, 30 }, moveTime = -1;
  int32_t time = -1, inc = 0;
  int32_t engineSide = BOTH;
  int32_t timeLeft = 0, sec = 0;
  int32_t movesPerSession = 0;
  uint32_t move = NOMOVE;
  int32_t i = 0, score = 0;
  char line[80], command[80];

  info->gameMode = MODE_XBOARD;
  info->postThinking = true;

  setbuf(stdin, NULL);
  setbuf(stdout, NULL);

  // Hack to send protover after getting into xboard mode
  printf(u8"feature ping=1 setboard=1 colors=0 usermove=1\n");
  printf(u8"feature done=1\n");

  do {
    fflush(stdout);

    if (pos->side == engineSide && ce_check_result(pos) == false) {
      info->startTime = sys_time_ms();
      info->depth = depth;

      if (time != -1) {
        info->timeSet = true;
        time /= movesToGo[pos->side];
        time -= 50;
        info->stopTime = info->startTime + time + inc;
      }

      if (depth == -1 || depth > MAX_DEPTH) {
        info->depth = MAX_DEPTH;
      }

      printf(u8"time:%d start:%d stop:%d depth:%d time set:%d moves to go:%d moves per session:%d\n",
        time, info->startTime, info->stopTime, info->depth, info->timeSet, movesToGo[pos->side], movesPerSession);

      ce_search_position(pos, info);

      if (movesPerSession != 0) {
        --movesToGo[pos->side ^ 1];
        if (movesToGo[pos->side ^ 1] < 1) {
          movesToGo[pos->side ^ 1] = movesPerSession;
        }
      }
    }

    fflush(stdout);

    memset((void *) line, 0, sizeof(line));
    fflush(stdout);
    if (!fgets(line, 80, stdin)) {
      continue;
    }

    sscanf(line, u8"%s", command);

    if (!strncmp(command, u8"quit", 4)) {
      info->quit = true;
      break;
    } else if (!strncmp(command, u8"force", 5)) {
      engineSide = BOTH;
      continue;
    } else if (!strncmp(command, u8"protover", 8)) {
      printf(u8"feature ping=1 setboard=1 colors=0 usermove=1\n");
      printf(u8"feature done=1\n");
    } else if (!strncmp(command, u8"sd", 2)) {
      sscanf(line, u8"sd %d", &depth);
      continue;
    } else if (!strncmp(command, u8"st", 2)) {
      sscanf(line, u8"st %d", &moveTime);
      continue;
    } else if (!strncmp(command, u8"time", 4)) {
      sscanf(line, u8"time %d", &time);
      time *= 10;
      printf(u8"DEBUG: time: %d\n", time);
      continue;
    } else if (!strncmp(command, u8"level", 5)) {
      sec = 0;
      moveTime = -1;
      if (sscanf(line, u8"level %d %d %d", &movesPerSession, &timeLeft, &inc) != 3) {
        sscanf(line, u8"level %d %d:%d %d", &movesPerSession, &timeLeft, &sec, &inc);
        printf(u8"DEBUG level with :\n");
      } else {
        printf(u8"DEBUG level without :\n");
      }
      timeLeft *= 60000;
      timeLeft += sec * 1000;
      movesToGo[0] = movesToGo[1] = 30;
      if (movesPerSession != 0) {
        movesToGo[0] = movesToGo[1] = movesPerSession;
      }
      time = -1;
      printf(u8"DEBUG level timeLeft:%d movesToGo:%d inc:%d movesPerSession:%d\n", timeLeft, movesToGo[0], inc, movesPerSession);
      continue;
    } else if (!strncmp(command, u8"ping", 4)) {
      printf(u8"pong%s\n", line + 4);
      continue;
    } else if (!strncmp(command, u8"new", 3)) {
      engineSide = BLACK;
      ce_parse_fen(START_FEN, pos);
      depth = -1;
      continue;
    } else if (!strncmp(command, u8"setboard", 8)) {
      engineSide = BOTH;
      ce_parse_fen(line + 9, pos);
      continue;
    } else if (!strncmp(command, u8"go", 2)) {
      engineSide = pos->side;
      continue;
    } else if (!strncmp(command, u8"usermove", 8)) {
      --movesToGo[pos->side];
      if ((move = ce_parse_move(line + 9, pos)) == NOMOVE) {
        continue;
      }
      ce_move_make(pos, move);
      pos->ply = 0;
    }
  } while (!info->quit);
}
