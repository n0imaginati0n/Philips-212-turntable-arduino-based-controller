#include <Arduino.h>

#include <limits.h>

#include "../functional.h"

unsigned long duration(unsigned long start, unsigned long end) {
  return (end >= start)
    ? end - start
    : (ULONG_MAX - start) + end; // overrun happens
}

