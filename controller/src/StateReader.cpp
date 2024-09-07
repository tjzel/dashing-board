#include "Message.hpp"
#include "Utils.hpp"
#include <ICommunicator.hpp>
#include <StateReader.hpp>
#include <cassert>
#include <utility>

// TODO: Split it into multiple functions.
bool StateReader::feed(const Byte byte) {
  switch (_state) {
  case WAITING_FOR_HEADER:
    if (!_isHeaderValid(byte)) {
      reset();
      return false;
    }
    _header = byte;
    _dataSize = _header & Message::REQUEST_HEADER_SIZE_MASK;
    _state = WAITING_FOR_TARGET;
    break;

  case WAITING_FOR_TARGET:
    if (!_isTargetValid(byte)) {
      reset();
      return false;
    }
    _target = byte;
    _state = WAITING_FOR_SOURCE;
    break;

  case WAITING_FOR_SOURCE:
    if (!_isSourceValid(byte)) {
      reset();
      return false;
    }
    _source = byte;
    _data = std::vector<Byte>(_dataSize);
    _dataIter = _data.begin();
    if (_dataSize > 0) {
      _state = WAITING_FOR_DATA;
    } else {
      _state = WAITING_FOR_CHECKSUM;
    }
    break;

  case WAITING_FOR_DATA:
    *_dataIter++ = byte;
    if (_dataIter == _data.end()) {
      _state = WAITING_FOR_CHECKSUM;
    };
    break;

  case WAITING_FOR_CHECKSUM:
    _checksum = byte;
    if (!_isHeaderValid(_header) || !_isSourceValid(_source) || !_isTargetValid(_target) || !isChecksumValid()) {
      reset();
      return false;
    }
    _hasMessage = true;
    return true;
  }
  return false;
}

Message StateReader::getMessage() {
  assert(_hasMessage);
  Message message{_header, _target, _source, _data};
  reset();
  return message;
}

StateReader::StateReader(IsValidFn isHeaderValid, IsValidFn isTargetValid, IsValidFn isSourceValid)
    : _isHeaderValid(std::move(isHeaderValid)), _isTargetValid(std::move(isTargetValid)),
      _isSourceValid(std::move(isSourceValid)) {}

void StateReader::reset() {
  _state = WAITING_FOR_HEADER;
  _hasMessage = false;
}

bool StateReader::isChecksumValid() {
  const auto checksum = calculateChecksum(_header, _target, _source, _data);
  return checksum == _checksum;
}
