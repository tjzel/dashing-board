#ifndef STATE_READER_HPP
#define STATE_READER_HPP

#include <Message.hpp>
#include <cstdint>
#include <functional>
#include <vector>

struct StateReaderValidators {
  std::function<bool(Byte)> isHeaderValid;
  std::function<bool(Byte)> isTargetValid;
  std::function<bool(Byte)> isSourceValid;
  std::function<uint64_t()> getTimestamp;
};

enum State : std::uint8_t {
  WAITING_FOR_HEADER,
  WAITING_FOR_TARGET,
  WAITING_FOR_SOURCE,
  WAITING_FOR_DATA,
  WAITING_FOR_CHECKSUM,
};

// TODO: Add a method for "auto-feeding" bytes.
class StateReader {
  using Validator = std::function<bool(Byte)>;
  using TimestampProvider = std::function<uint64_t()>;

public:
  bool feed(int byte);
  Message getMessage();
  explicit StateReader(StateReaderValidators validators);

private:
  void reset();
  bool isChecksumValid();

  StateReaderValidators validators_;

  Byte header_{};
  Byte dataSize_{};
  Byte target_{};
  Byte source_{};
  std::vector<Byte> data_;
  std::vector<Byte>::iterator dataIter_{data_.begin()};
  Byte checksum_{};

  static constexpr uint64_t DropoutTime = 100;

  bool hasMessage_{false};
  State state_{WAITING_FOR_HEADER};
  uint64_t timestamp_{};
};

#endif // STATE_READER_HPP
