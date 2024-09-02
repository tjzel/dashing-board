#include <DataProvider.hpp>

int DataProvider::get() {
  const auto value = _value;
  if (_value <= _bottomThreshold) {
    _sign = 1;
  } else if (_value >= _topThreshold) {
    _sign = -1;
  }

  _value += _sign * _step;
  return value;
}

DataProvider::DataProvider(int initialValue, int step, int sign, int bottomThreshold, int topThreshold)
    : _value(initialValue), _step(step), _sign(sign), _bottomThreshold(bottomThreshold), _topThreshold(topThreshold) {}
