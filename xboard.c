
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

