#include <cstdint>

class TimeProvider {
private:
  uint64_t time_ = 0;

public:
  uint64_t get() { return time_++; }
};
