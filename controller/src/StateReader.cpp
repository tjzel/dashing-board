#include <ICommunicator.hpp>
#include <Message.hpp>
#include <StateReader.hpp>
#include <Utils.hpp>
#include <cassert>
#include <iostream>
#include <utility>

// TODO: Split it into multiple functions.
bool StateReader::feed(const int byte) {
  if (byte == -1) {
    reset();
    return false;
  };
  switch (_state) {
  case WAITING_FOR_HEADER:
    // TODO: Add own wrappers with default behavior.
    if (!_isHeaderValid(byte) || !(byte & 0xC0)) {
      reset();
      return false;
    }
    _header = byte;
    _dataSize = _header & Message::REQUEST_HEADER_SIZE_MASK;
    _state = WAITING_FOR_TARGET;
    _timestamp = _getTimestamp();
    break;

  case WAITING_FOR_TARGET:
    if (_getTimestamp() - _timestamp > DropoutTime) {
      reset();
      return false;
    }
    _target = byte;
    _state = WAITING_FOR_SOURCE;
    break;

  case WAITING_FOR_SOURCE:
    if (_getTimestamp() - _timestamp > DropoutTime) {
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
    if (_getTimestamp() - _timestamp > DropoutTime) {
      reset();
      return false;
    }
    *_dataIter++ = byte;
    if (_dataIter == _data.end()) {
      _state = WAITING_FOR_CHECKSUM;
    };
    break;

  case WAITING_FOR_CHECKSUM:
    if (_getTimestamp() - _timestamp > DropoutTime) {
      reset();
      return false;
    }
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

StateReader::StateReader(IsValidFn isHeaderValid, IsValidFn isTargetValid, IsValidFn isSourceValid,
                         TimestampProvider getTimestamp)
    : _isHeaderValid(std::move(isHeaderValid)), _isTargetValid(std::move(isTargetValid)),
      _isSourceValid(std::move(isSourceValid)), _getTimestamp(std::move(getTimestamp)) {}

void StateReader::reset() {
  _state = WAITING_FOR_HEADER;
  _hasMessage = false;
}

bool StateReader::isChecksumValid() {
  const auto checksum = calculateChecksum(_header, _target, _source, _data);
  return checksum == _checksum;
}
