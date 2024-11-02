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
    if (!isHeaderValid_(byte) || !(byte & 0xC0)) {
      reset();
      return false;
    }
    header_ = byte;
    dataSize_ = header_ & Message::REQUEST_HEADER_SIZE_MASK;
    state_ = WAITING_FOR_TARGET;
    timestamp_ = getTimestamp_();
    break;

  case WAITING_FOR_TARGET:
    if (getTimestamp_() - timestamp_ > DropoutTime) {
      reset();
      return false;
    }
    target_ = byte;
    state_ = WAITING_FOR_SOURCE;
    break;

  case WAITING_FOR_SOURCE:
    if (getTimestamp_() - timestamp_ > DropoutTime) {
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
    if (getTimestamp_() - timestamp_ > DropoutTime) {
      reset();
      return false;
    }
    *dataIter_++ = byte;
    if (dataIter_ == data_.end()) {
      state_ = WAITING_FOR_CHECKSUM;
    };
    break;

  case WAITING_FOR_CHECKSUM:
    if (getTimestamp_() - timestamp_ > DropoutTime) {
      reset();
      return false;
    }
    checksum_ = byte;
    if (!isHeaderValid_(header_) || !isSourceValid_(source_) || !isTargetValid_(target_) || !isChecksumValid()) {
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

StateReader::StateReader(Validator isHeaderValid, Validator isTargetValid, Validator isSourceValid,
                         TimestampProvider getTimestamp)
    : isHeaderValid_(std::move(isHeaderValid)), isTargetValid_(std::move(isTargetValid)),
      isSourceValid_(std::move(isSourceValid)), getTimestamp_(std::move(getTimestamp)) {}

void StateReader::reset() {
  state_ = WAITING_FOR_HEADER;
  hasMessage_ = false;
}

bool StateReader::isChecksumValid() {
  const auto checksum = calculateChecksum(header_, target_, source_, data_);
  return checksum == checksum_;
}
