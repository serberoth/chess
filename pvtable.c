
#include "defs.h"

void ce_pvtable_init(struct pvtable_s *table) {
  const int count = 0x200000 / sizeof(struct pventry_s);

  ce_pvtable_free(table);

  table->count = count;
  table->entries = (struct pventry_s *) calloc(1, sizeof(struct pventry_s) * table->count);
  ce_pvtable_clear(table);
  // Waste the last two entries as overrun padding
  table->count -= 2;

  printf("PVTable init complete with %d entries\n", table->count);
}

void ce_pvtable_free(struct pvtable_s *table) {
  int count = table->count;

  if (table->entries != NULL) {
    free(table->entries);
    table->entries = NULL;

    printf("PVTable free complete with %d entries\n", count);
  }

  table->count = 0;
}

void ce_pvtable_clear(struct pvtable_s *table) {
  struct pventry_s *entry = table->entries;

  for ( ; entry < table->entries + table->count; ++entry) {
    entry->positionKey = 0ULL;
    entry->move.val = NOMOVE;
  }
}

void ce_pvtable_store(const struct board_s *pos, const int move) {
  int index = pos->positionKey % pos->pvtable.count;

  ASSERT(index >= 0 && index <= pos->pvtable.count);

  pos->pvtable.entries[index].move.val = move;
  pos->pvtable.entries[index].positionKey = pos->positionKey;
}

int ce_pvtable_probe(const struct board_s *pos) {
  int index = pos->positionKey % pos->pvtable.count;

  ASSERT(index >= 0 && index <= pos->pvtable.count);

  if (pos->pvtable.entries[index].positionKey == pos->positionKey) {
    return pos->pvtable.entries[index].move.val;
  }

  return NOMOVE;
}

int ce_pvtable_get_line(const int depth, struct board_s *pos) {
  int move, count = 0;

  ASSERT(depth < MAX_DEPTH);

  for (move = ce_pvtable_probe(pos);
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

