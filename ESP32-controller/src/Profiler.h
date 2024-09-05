#ifndef PROFILER_H
#define PROFILER_H

#include <Arduino.h>
#include <cstdint>

struct Profiler {
  uint64_t totalTime = 0;
  uint64_t functionCalls = 0;
  uint64_t ecuTime = 0;

  static const uint64_t functionCallThreshold = 10;
};

#endif // PROFILER_H
