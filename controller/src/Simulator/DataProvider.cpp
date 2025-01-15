#include <DataProvider.hpp>

int DataProvider::get() {
  const auto value = value_;
  if (value_ <= bottomThreshold_) {
    sign_ = 1;
  } else if (value_ >= topThreshold_) {
    sign_ = -1;
  }

  value_ += sign_ * step_;
  return value;
}

DataProvider::DataProvider(int initialValue, int step, int sign, int bottomThreshold,
                           int topThreshold)
    : value_(initialValue), step_(step), sign_(sign), bottomThreshold_(bottomThreshold),
      topThreshold_(topThreshold) {}
