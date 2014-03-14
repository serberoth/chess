
#include <sys/time.h>
#include "defs.h"

int sys_time_ms() {
  struct timeval t = { 0 };

  gettimeofday(&t, NULL);

  return t.tv_sec * 1000 + t.tv_usec / 1000;
}

