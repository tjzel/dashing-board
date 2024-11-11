#include "StateReader.hpp"
#include "Message.hpp"
#include "Utils.hpp"
#include <ICommunicator.hpp>
#include <cassert>
#include <utility>

// TODO: Split it into multiple functions.
bool StateReader::feed(const int byte) {
  if (byte == -1) {
    reset();
    return false;
  };
  switch (state_) {
  case WAITING_FOR_HEADER:
    // TODO: Add own wrappers with default behavior.
    if (!validators_.isHeaderValid(byte) || !(byte & 0xC0)) {
      reset();
      return false;
    }
    header_ = byte;
    dataSize_ = header_ & Message::REQUEST_HEADER_SIZE_MASK;
    state_ = WAITING_FOR_TARGET;
    timestamp_ = validators_.getTimestamp();
    break;

  case WAITING_FOR_TARGET:
    if (validators_.getTimestamp() - timestamp_ > DropoutTime) {
      reset();
      return false;
    }
    target_ = byte;
    state_ = WAITING_FOR_SOURCE;
    break;

  case WAITING_FOR_SOURCE:
    if (validators_.getTimestamp() - timestamp_ > DropoutTime) {
      reset();
      return false;
    }
    source_ = byte;
    data_ = std::vector<Byte>(dataSize_);
    dataIter_ = data_.begin();
    if (dataSize_ > 0) {
      state_ = WAITING_FOR_DATA;
    } else {
      state_ = WAITING_FOR_CHECKSUM;
    }
    break;

  case WAITING_FOR_DATA:
    if (validators_.getTimestamp() - timestamp_ > DropoutTime) {
      reset();
      return false;
    }
    *dataIter_++ = byte;
    if (dataIter_ == data_.end()) {
      state_ = WAITING_FOR_CHECKSUM;
    };
    break;

  case WAITING_FOR_CHECKSUM:
    if (validators_.getTimestamp() - timestamp_ > DropoutTime) {
      reset();
      return false;
    }
    checksum_ = byte;
    if (!validators_.isHeaderValid(header_) || !validators_.isSourceValid(source_) ||
        !validators_.isTargetValid(target_) || !isChecksumValid()) {
      reset();
      return false;
    }
    hasMessage_ = true;
    return true;
  }
  return false;
}

Message StateReader::getMessage() {
  assert(hasMessage_);
  Message message{header_, target_, source_, data_};
  reset();
  return message;
}

StateReader::StateReader(StateReaderValidators validators) : validators_(std::move(validators)) {}

void StateReader::reset() {
  state_ = WAITING_FOR_HEADER;
  hasMessage_ = false;
}

bool StateReader::isChecksumValid() {
  const auto checksum = calculateChecksum(header_, target_, source_, data_);
  return checksum == checksum_;
}
