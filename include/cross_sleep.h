#ifndef CROSS_SLEEP_H
#define CROSS_SLEEP_H

#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <errno.h>
#include <time.h>
#endif

static inline void sleep_ms(uint32_t ms) {
#ifdef _WIN32
  Sleep(ms);
#else
  struct timespec req, rem;
  req.tv_sec = ms / 1000;
  req.tv_nsec = (ms % 1000) * 1000000L;
  while (nanosleep(&req, &rem) == -1 && errno == EINTR) {
    req = rem;
  }
#endif
}

static inline void sleep_seconds(unsigned int s) {
  sleep_ms((uint32_t)s * 1000u);
}

static inline uint64_t now_ms(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

#endif // CROSS_SLEEP_H
