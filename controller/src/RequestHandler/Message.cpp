#include <Message.hpp>
#include <Utils.hpp>
#include <algorithm>

Byte Message::calculateChecksum(const Byte format, const Byte target, const Byte source,
                                const std::span<const Byte> data) {
  std::vector<Byte> rawMessage{format, target, source};
  std::ranges::copy(data, std::back_inserter(rawMessage));
  const auto checksum = ::calculateChecksum(rawMessage);
  return checksum;
}

Byte Message::dataSize() const { return format & REQUEST_HEADER_SIZE_MASK; }

bool Message::isOBD2Message() const { return dataSize() > 1; }

bool Message::isInitMessage() const {
  const Byte initByte = 0x81;
  return dataSize() == 1 && data[0] == initByte;
}

Message::Message(const Byte format, const Byte target, const Byte source, std::vector<Byte> data)
    : format(format), target(target), source(source), data(data),
      checksum(calculateChecksum(format, target, source, data)) {
  if (dataSize() == data.size()) {
    data = std::vector<Byte>(dataSize());
  }
}

Message::operator std::vector<Byte>() const {
  std::vector<Byte> message(dataSize() + MINIMAL_MESSAGE_SIZE);
  message[0] = format;
  message[1] = target;
  message[2] = source;
  std::ranges::copy(data, message.begin() + 3);
  message.back() = checksum;
  return message;
}

Byte OBD2Message::format() const { return message.format; }

Byte OBD2Message::target() const { return message.target; }

Byte OBD2Message::source() const { return message.source; }

Byte OBD2Message::mode() const { return message.data[0]; }

Byte OBD2Message::pid() const { return message.data[1]; }

Byte OBD2Message::obd2DataSize() const { return message.dataSize(); }

std::span<const Byte> OBD2Message::obd2Data() const {
  return std::span(message.data.begin() + 2, obd2DataSize());
}

OBD2Message::OBD2Message(Message &message) : message(message) {}

CommandLiteral OBD2Message::command() const { return CommandLiteral{.mode = mode(), .pid = pid()}; }
