
#include "defs.h"

const int hash_table_size = 16;

/**
 * Chess Engine pvtable function that initializes the provided pv table structure.
 * @param table A pointer to the table instance to initialize.
 */
void ce_pvtable_init(struct pvtable_s *table, size_t size) {
  ce_pvtable_free(table);

  table->count = (0x1000000 * size) / sizeof(struct pventry_s);
  table->entries = (struct pventry_s *) calloc(table->count, sizeof(struct pventry_s));
  // Waste the last two entries as overrun padding
  table->count -= 2;
  ce_pvtable_clear(table);
  // printf(u8"PVTable init complete with %zu entries\n", table->count);
}

/**
 * Chess Engine pvtable function that releases the resources used by the provided
 * pvtable structure instance.
 * @param A pointer to the table instance to release.
 */
void ce_pvtable_free(struct pvtable_s *table) {
  int32_t count = table->count;
  if (table->entries != NULL) {
    free(table->entries);
  }
  memset((void *) table, 0, sizeof(struct pvtable_s));
  // printf(u8"PVTable free complete with %d entries\n", count);
}

/**
 * Chess Engine pvtable function that clears the provided pvtable structure instance.
 * @param table A pointer to the table instance to clear.
 */
void ce_pvtable_clear(struct pvtable_s *table) {
  struct pventry_s *entry = table->entries;
  for ( ; entry < (table->entries + table->count); ++entry) {
    entry->positionKey = 0ull;
    entry->move.val = NOMOVE;
    entry->depth = 0;
    entry->score = 0;
    entry->flags = 0;
  }
  table->newWrite = 0;
}

/**
 * Chess Engine pvtable function to store the provided move in the
 * pvtable for the provided board position.
 * @param pos A pointer to the current board position.
 * @param move The current move beign evaluated.
 * @param score The score of the current move.
 * @param flags Flag indicator determining the confidence of the moves score.
 * @param depth The search depth of the move.
 */
void ce_pvtable_store(struct board_s *pos, const uint32_t move, int32_t score, const int32_t flags, const int32_t depth) {
  size_t index = pos->positionKey % pos->pvtable.count;

  ASSERT(index >= 0 && index <= pos->pvtable.count - 1);
  ASSERT(depth >= 1 && depth <= MAX_DEPTH);
  ASSERT(flags >= HFALPHA && flags <= HFEXACT);
  ASSERT(score >= -INFINITY && score <= INFINITY);
  ASSERT(pos->ply >= 0 && pos->ply <= MAX_DEPTH);

  if (pos->pvtable.entries[index].positionKey == 0) {
    ++pos->pvtable.newWrite;
  } else {
    ++pos->pvtable.overWrite;
  }

  // Reset the hash score back to the MATE value
  if (score > MATE) {
    score += pos->ply;
  } else if (score < -MATE) {
    score -= pos->ply;
  }

  pos->pvtable.entries[index].positionKey = pos->positionKey;
  pos->pvtable.entries[index].move.val = move;
  pos->pvtable.entries[index].score = score;
  pos->pvtable.entries[index].flags = flags;
  pos->pvtable.entries[index].depth = depth;
}

bool ce_pvtable_probe(struct board_s *pos, uint32_t *move, int32_t *score, int32_t alpha, int32_t beta, int32_t depth) {
  size_t index = pos->positionKey % pos->pvtable.count;

  ASSERT(index >= 0 && index <= pos->pvtable.count - 1);
  ASSERT(depth >= 1 && depth < MAX_DEPTH);
  ASSERT(alpha < beta);
  ASSERT(alpha >= -INFINITY && alpha <= INFINITY);
  ASSERT(beta >= -INFINITY && beta <= INFINITY);
  ASSERT(pos->ply >= 0 && pos->ply < MAX_DEPTH);

  struct pventry_s *entry = &pos->pvtable.entries[index];
  if (entry->positionKey == pos->positionKey) {
    *move = entry->move.val;
    if (entry->depth >= depth) {
      ++pos->pvtable.hit;

      ASSERT(entry->depth >= 1 && entry->depth < MAX_DEPTH);
      ASSERT(entry->flags >= HFALPHA && entry->flags <= HFEXACT);

      // Adjust the mate score with the ply again 
      *score = entry->score;
      if (*score > MATE) {
        *score -= pos->ply;
      } else if (*score < -MATE) {
        *score += pos->ply;
      }

      ASSERT(entry->score == *score);
      ASSERT(*score >= -INFINITY && *score <= INFINITY);
      switch (entry->flags) {
      case HFALPHA: if (*score <= alpha) { *score = alpha; return true; } break;
      case HFBETA:  if (*score <= beta) { *score = beta; return true; } break;
      case HFEXACT: return true;
      default:
        ASSERT(false);
        break;
      }
    }
  }
  return false;
}

/**
 * Chess Engine pvtable function to probe the pvtable of the provided
 * board position.
 * @param pos A pointer to the current board position.
 * @return The move value of the probe into the pvtable.
 */
uint32_t ce_pvtable_pvprobe(const struct board_s *pos) {
  size_t index = pos->positionKey % pos->pvtable.count;
  ASSERT(index >= 0 && index <= pos->pvtable.count);

  if (pos->pvtable.entries[index].positionKey == pos->positionKey) {
    return pos->pvtable.entries[index].move.val;
  }

  return NOMOVE;
}

/**
 * Chess Engine pvtable function to get the line of play to the stored
 * move in the pvtable from the provided board position to the indicated
 * play depth.
 * @param depth The play depth for the desired move.
 * @param pos A pointer to the current board position.
 * @return The number of moves in the line of play stored in the pvtable.
 */
size_t ce_pvtable_get_line(const int32_t depth, struct board_s *pos) {
  size_t count = 0ull;

  for (uint32_t move = ce_pvtable_pvprobe(pos);
      move != NOMOVE && count < depth;
      move = ce_pvtable_pvprobe(pos)) {
    ASSERT(count < MAX_DEPTH);

    if (!ce_move_exists(pos, move)) {
      break;
    }

    ce_move_make(pos, move);
    pos->pvarray[count++] = move;
  }

  while (pos->ply > 0) {
    ce_move_take(pos);
  }

  return count;
}
