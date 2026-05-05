#include <Arduino.h>

#include <limits.h>

#include "functional.h"

unsigned long duration(unsigned long start, unsigned long end) {
  return (end < start)
    ? (ULONG_MAX - start) + end // overrun happens
    : end - start;
}

unsigned long duration_micros(unsigned long start) {
  return duration(start, micros());
}

