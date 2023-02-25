
#include "defs.h"

/**
 * Chess Engine pvtable function that initializes the provided pv table structure.
 * @param table A pointer to the table instance to initialize.
 */
void ce_pvtable_init(struct pvtable_s *table) {
  const int32_t count = 0x200000 / sizeof(struct pventry_s);

  ce_pvtable_free(table);

  table->count = count;
  table->entries = (struct pventry_s *) calloc(1, sizeof(struct pventry_s) * table->count);
  ce_pvtable_clear(table);
  // Waste the last two entries as overrun padding
  table->count -= 2;

  printf(u8"PVTable init complete with %zu entries\n", table->count);
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
    table->entries = NULL;

    printf(u8"PVTable free complete with %d entries\n", count);
  }

  table->count = 0;
}

/**
 * Chess Engine pvtable function that clears the provided pvtable structure instance.
 * @param table A pointer to the table instance to clear.
 */
void ce_pvtable_clear(struct pvtable_s *table) {
  struct pventry_s *entry = table->entries;

  for ( ; entry < table->entries + table->count; ++entry) {
    entry->positionKey = 0ULL;
    entry->move.val = NOMOVE;
  }
}

/**
 * Chess Engine pvtable function to store the provided move in the
 * pvtable for the provided board position.
 * @param pos A pointer to the current board position.
 * @param move The current move beign evaluated.
 */
void ce_pvtable_store(const struct board_s *pos, const uint32_t move) {
  int32_t index = pos->positionKey % pos->pvtable.count;

  ASSERT(index >= 0 && index <= pos->pvtable.count);

  pos->pvtable.entries[index].move.val = move;
  pos->pvtable.entries[index].positionKey = pos->positionKey;
}

/**
 * Chess Engine pvtable function to probe the pvtable of the provided
 * board position.
 * @param pos A pointer to the current board position.
 * @return The move value of the probe into the pvtable.
 */
uint32_t ce_pvtable_probe(const struct board_s *pos) {
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

  ASSERT(depth < MAX_DEPTH);

  for (uint32_t move = ce_pvtable_probe(pos);
      move != NOMOVE && count < depth;
      move = ce_pvtable_probe(pos)) {
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
