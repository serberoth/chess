
#include "defs.h"

void ce_pvtable_init(struct pvtable_s *table) {
  table->count = 0x200000 / sizeof(struct pventry_s);

  if (table->entries != NULL) {
    free(table->entries);
    table->entries = NULL;
  }

  table->entries = (struct pventry_s *) calloc(1, sizeof(struct pventry_s) * table->count);
  ce_pvtable_clear(table);
  table->count -= 2;

  printf("PVTable init complete with %d entries\n", table->count);
}

void ce_pvtable_clear(struct pvtable_s *table) {
  struct pventry_s *entry = table->entries;

  for ( ; entry < table->entries + table->count; ++entry) {
    entry->positionKey = 0ULL;
    entry->move.val = NOMOVE;
  }
}

