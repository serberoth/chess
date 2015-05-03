
#ifndef WIN32
#include <unistd.h>
// #include <time.h>
#include <sys/time.h>
#else
#include <windows.h>
#endif

#include <string.h>
#include "defs.h"

/**
 * System function to obtain the current time in ms.
 * @return The current system time in ms.
 */
int sys_time_ms() {
#ifndef WIN32
  struct timeval t = { 0 };

  gettimeofday(&t, NULL);

  return t.tv_sec * 1000 + t.tv_usec / 1000;
#else
  return GetTickCount();
#endif
}

/**
 * System input function to determine if there is
 * currently input waiting on the current process
 * standard input stream.
 * @return Integer value indicating the status of input
 *   on the standard input stream (1 - input waiting) or
 *   (0 - no input waiting)
 */
// http://home.acor.de/dreamlike/chess/
int sys_input_waiting() {
#ifndef WIN32
  struct timeval t = { 0 };
  fd_set readfds;

  FD_ZERO(&readfds);
  FD_SET(fileno(stdin), &readfds);
  t.tv_sec = 0;
  t.tv_usec = 0;
  select(16, &readfds, 0, 0, &t);

  return (FD_ISSET(fileno(stdin), &readfds));
#else
  static int init = 0, pipe;
  static HANDLE inh;
  DWORD dw;

  if (!init) {
    init = 1;
    inh = GetStdHandle(STD_INPUT_HANDLE);
    pipe = !GetConsoleMode(inh, &dw);
    if (!pipe) {
      SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
      FlushConsoleInputBuffer(inh);
    }
  }

  if (pipe) {
    if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) {
      return 1;
    }
  } else {
    GetNumberOfConsoleInputEvents(inh, &dw);
    return dw <= 1 ? 0 : dw;
  }
#endif
}

/**
 * System read function that reads input from the current
 * process standard input stream and configures the provided
 * search info structure according to that input.
 * @param info A pointer to the current search parameter
 *    information structure.
 */
void sys_read_input(struct search_info_s *info) {
  int bytes;
  char input[256] = "", *endc;

  if (sys_input_waiting()) {
    info->stopped = TRUE;

    do {
      bytes = read(fileno(stdin), input, 256);
    } while (bytes < 0);

    if ((endc = strchr(input, '\n'))) {
      *endc = 0;
    }

    if (strlen(input) > 0) {
      if (!strncmp(input, "quit", 4)) {
        info->quit = TRUE;
      }
    }
  }
}

