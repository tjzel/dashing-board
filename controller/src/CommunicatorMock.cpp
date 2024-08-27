#include "CommunicatorMock.hpp"
#include <RequestHandler.hpp>
#include <Utils.hpp>
#include <_types/_uint8_t.h>
#include <iostream>
#include <string>
#include <utility>

constexpr const uint8_t ECU_ADDRESS = 0x33;
constexpr const uint8_t REQUEST_HEADER = 0xC0;
constexpr const uint8_t REQUEST_HEADER_MODE_MASK = 0xF0;
constexpr const uint8_t REQUEST_HEADER_SIZE_MASK = 0x0F;
constexpr auto MINIMAL_MESSAGE_SIZE = 4;

/**
 * OBD2 message contains at least two bytes - mode and PID.
 */
bool isOBD2Message(const struct Message &message) {
  return message.dataSize > 1;
}

uint8_t CommunicatorMock::read() {
  if (_inputEnd == _inputBegin) {
    std::cout << "No more data to read." << std::endl;
    return 0x00;
  }
  return *_inputBegin++;
}

void CommunicatorMock::write(const uint8_t byte) {
  *_outputGuard = byte;
  ++_outputGuard;
}

void CommunicatorMock::write(const std::vector<uint8_t> message) {
  auto it = _outputGuard;
  for (auto byte : message) {
    *it = byte;
    ++it;
  }
  _outputGuard = it;
}

void CommunicatorMock::print(const uint8_t byte) { std::cout << byte; }

void CommunicatorMock::print(const std::string str) { std::cout << str; }

void CommunicatorMock::println(const uint8_t byte) {
  std::cout << std::hex << byte;
  println();
}

void CommunicatorMock::println(const std::string str) {
  std::cout << str;
  println();
}

void CommunicatorMock::println() { std::cout << "\n"; }

bool CommunicatorMock::hasValidInputMessage() {
  if (_hasValidInputMessage) {
    return true;
  };

  const auto startIt = _outputByteBuffer.begin();
  if (startIt + MINIMAL_MESSAGE_SIZE >= _outputGuard) {
    return false;
  }

  for (auto it = startIt; it != _outputGuard; ++it) {
    const auto header = *it++;
    if ((header & REQUEST_HEADER_MODE_MASK) != REQUEST_HEADER) {
      continue;
    };
    _currentInputMessage.header = header;

    const auto dataSize = header & REQUEST_HEADER_SIZE_MASK;
    if (startIt + MINIMAL_MESSAGE_SIZE + dataSize >= _outputGuard) {
      return false;
    }
    _currentInputMessage.dataSize = dataSize;

    const auto receiver = *it++;
    if (receiver != ECU_ADDRESS) {
      continue;
    }
    _currentInputMessage.receiver = receiver;

    const auto sender = *it++;
    _currentInputMessage.sender = sender;

    std::vector<uint8_t> data(dataSize);
    for (int i = 0; i < dataSize; i++) {
      data[i] = *it++;
    }
    _currentInputMessage.data = data;

    const auto checksum = *it++;
    if (!isMessageValid(data, checksum)) {
      continue;
    }
    _currentInputMessage.checksum = checksum;
    _hasValidInputMessage = true;

    return true;
  }

  return false;
}

void CommunicatorMock::handleInput() {
  if (!hasValidInputMessage()) {
    return;
  }

  if (!isOBD2Message(_currentInputMessage)) {
    // Ignore and drop the message.
    _hasValidInputMessage = false;
    return;
  }

  const OBD2Message obd2Message{
      _currentInputMessage.data[0], _currentInputMessage.data[1],
      static_cast<uint8_t>(_currentInputMessage.dataSize - 2),
      std::vector<uint8_t>(_currentInputMessage.data.begin() + 2,
                           _currentInputMessage.data.end())};

  react({obd2Message.mode, obd2Message.pid});
}

void CommunicatorMock::sendResponse(const OBD2Message &response) {

  const uint8_t header = 0x80 | response.dataSize + 2;
  const uint8_t receiver = _currentInputMessage.sender;
  const uint8_t sender = _currentInputMessage.receiver;

  std::vector<uint8_t> rawMessage{header, receiver, sender, response.mode,
                                  response.pid};
  rawMessage.insert(rawMessage.end(), response.data.begin(),
                    response.data.end());
  const uint8_t checksum = calculateChecksum(rawMessage);
  rawMessage.push_back(checksum);

  _inputBegin = _inputByteBuffer.begin();
  auto it = _inputBegin;
  for (auto byte : rawMessage) {
    // TODO: Range checking or copy from standard library.
    *it++ = byte;
  }
  _inputEnd = it;
}
