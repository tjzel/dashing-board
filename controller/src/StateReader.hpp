#ifndef STATE_READER_HPP
#define STATE_READER_HPP

#include <Message.hpp>
#include <functional>
#include <vector>

enum State {
  WAITING_FOR_HEADER,
  WAITING_FOR_TARGET,
  WAITING_FOR_SOURCE,
  WAITING_FOR_DATA,
  WAITING_FOR_CHECKSUM,
};

class StateReader {
  using IsValidFn = std::function<bool(Byte)>;

public:
  bool feed(Byte byte);
  Message getMessage();
  explicit StateReader(IsValidFn isHeaderValid, IsValidFn isTargetValid, IsValidFn isSourceValid);

private:
  void reset();
  bool isChecksumValid();

  std::function<bool(Byte)> _isHeaderValid;
  std::function<bool(Byte)> _isTargetValid;
  std::function<bool(Byte)> _isSourceValid;

  Byte _header{};
  Byte _dataSize{};
  Byte _target{};
  Byte _source{};
  std::vector<Byte> _data{};
  std::vector<Byte>::iterator _dataIter{_data.begin()};
  Byte _checksum{};

  bool _hasMessage{false};
  State _state{WAITING_FOR_HEADER};
};

#endif // STATE_READER_HPP
