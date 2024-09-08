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

// TODO: Add a method for "auto-feeding" bytes.
class StateReader {
  using IsValidFn = std::function<bool(Byte)>;
  using TimestampProvider = std::function<uint64_t()>;

public:
  bool feed(const int byte);
  Message getMessage();
  explicit StateReader(IsValidFn isHeaderValid, IsValidFn isTargetValid, IsValidFn isSourceValid,
                       TimestampProvider getTimestamp);

private:
  void reset();
  bool isChecksumValid();

  std::function<bool(Byte)> _isHeaderValid;
  std::function<bool(Byte)> _isTargetValid;
  std::function<bool(Byte)> _isSourceValid;
  std::function<uint64_t()> _getTimestamp;

  Byte _header{};
  Byte _dataSize{};
  Byte _target{};
  Byte _source{};
  std::vector<Byte> _data{};
  std::vector<Byte>::iterator _dataIter{_data.begin()};
  Byte _checksum{};

  static constexpr uint64_t DropoutTime = 100;

  bool _hasMessage{false};
  State _state{WAITING_FOR_HEADER};
  uint64_t _timestamp;
};

#endif // STATE_READER_HPP
