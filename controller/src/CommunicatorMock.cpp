#include "DiagnosticCommands.hpp"
#include <CommunicatorMock.hpp>
#include <RequestHandler.hpp>
#include <Utils.hpp>
#include <iostream>
#include <string>

/**
 * OBD2 message contains at least two bytes - mode and PID.
 */
bool isOBD2Message(Message &message) { return message.dataSize() > 1; }

byte CommunicatorMock::read() {
  // TODO: use views in the future.
  // const auto outputView = std::span(_outputBegin, _outputEnd);
  if (_outputEnd == _outputBegin) {
    _outputBegin = _outputBuffer.begin();
    _outputEnd = _outputBegin;
    std::cout << "No more data to read.\n Sending random data.\n" << std::endl;
    respond<ENGINE_RPM>();
    respond<VEHICLE_SPEED>();
  }
  const auto byte = *_outputBegin++;
  if (_outputBegin == _outputEnd) {
    _outputBegin = _outputBuffer.begin();
    _outputEnd = _outputBegin;
  }
  return byte;
}

void CommunicatorMock::write(const byte byte) {
  *_inputEnd = byte;
  ++_inputEnd;
  handleInput();
}

void CommunicatorMock::write(const std::vector<byte> &message) {
  auto it = _inputEnd;
  for (auto byte : message) {
    *it = byte;
    ++it;
  }
  _inputEnd = it;
  handleInput();
}

void CommunicatorMock::print(const byte byte) {
  std::cout << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(byte);
}

void CommunicatorMock::print(const std::string &str) { std::cout << str; }

void CommunicatorMock::println(const byte byte) {
  std::cout << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(byte);
  println();
}

void CommunicatorMock::println(const std::string &str) {
  std::cout << str;
  println();
}

void CommunicatorMock::println() { std::cout << "\n"; }

bool CommunicatorMock::hasValidInputMessage() {
  if (_hasValidInputMessage) {
    return true;
  };

  auto startIt = _inputBuffer.begin();
  if (startIt + Message::MINIMAL_MESSAGE_SIZE > _inputEnd) {
    return false;
  }

  for (; startIt < _inputEnd; ++startIt) {
    auto it = startIt;
    const auto header = *it++;
    if ((header & REQUEST_HEADER_MODE_MASK) != REQUEST_HEADER) {
      continue;
    };
    const auto dataSize = header & Message::REQUEST_HEADER_SIZE_MASK;
    if (startIt + Message::MINIMAL_MESSAGE_SIZE + dataSize > _inputEnd) {
      // The whole message hasn't arrived yet.
      return false;
    }

    const auto receiver = *it++;
    if (receiver != ECU_ADDRESS) {
      continue;
    }

    // We don't really do anything with the sender right now.
    const auto sender = *it++;

    std::vector<byte> data(dataSize);
    for (int i = 0; i < dataSize; i++) {
      data[i] = *it++;
    }

    const auto checksum = *it;

    const Message message(header, receiver, sender, data);

    if (checksum != message.checksum) {
      continue;
    };

    _hasValidInputMessage = true;
    _inputMessage = Message(message);
    _inputEnd = _inputBuffer.begin();

    return true;
  }

  return false;
}

void CommunicatorMock::handleInput() {
  if (!hasValidInputMessage()) {
    return;
  }

  _hasValidInputMessage = false;

  if (!_inputMessage.isOBD2Message()) {
    // Ignore and drop the message.
    return;
  }

  const OBD2Message obd2Message(_inputMessage);
  handleDiagnosticRequest(obd2Message.command());
}

void CommunicatorMock::sendResponse(const byte mode, const byte pid,
                                    const std::span<const byte> obd2Payload) {
  const byte obd2PayloadSize = obd2Payload.size() + OBD2_HEADER_SIZE;
  const byte header = RESPONSE_HEADER | obd2Payload.size() + OBD2_HEADER_SIZE;
  const byte receiver = _inputMessage.source;
  const byte sender = ECU_ADDRESS;
  std::vector<byte> payload(obd2PayloadSize);
  payload[0] = mode;
  payload[1] = pid;
  std::copy(obd2Payload.begin(), obd2Payload.end(), payload.begin() + 2);

  const Message response(header, receiver, sender, payload);
  send(response);
}

void CommunicatorMock::send(const Message &message) {
  *_outputEnd++ = message.format;
  *_outputEnd++ = message.target;
  *_outputEnd++ = message.source;
  std::copy(message.data.begin(), message.data.end(), _outputEnd);
  _outputEnd += message.data.size();
  *_outputEnd++ = message.checksum;
}

void CommunicatorMock::handleDiagnosticRequest(const CommandLiteral pid) {
  if (pid == COMMAND_AVAILABILITY_00_1F::value) {
    respond<COMMAND_AVAILABILITY_00_1F>();
  } else if (pid == ENGINE_RPM::value) {
    respond<ENGINE_RPM>();
  } else if (pid == VEHICLE_SPEED::value) {
    respond<VEHICLE_SPEED>();
  } else {
    // Ignore unknown pids.
    // TODO:  Compare this behavior to real ECU.
  }
}

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

DataProvider::DataProvider(int initialValue, int step, int sign,
                           int bottomThreshold, int topThreshold)
    : _value(initialValue), _step(step), _sign(sign),
      _bottomThreshold(bottomThreshold), _topThreshold(topThreshold) {}
