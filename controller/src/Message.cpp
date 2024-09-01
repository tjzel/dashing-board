#include <Message.hpp>
#include <Utils.hpp>

byte Message::calculateChecksum(const byte format, const byte target,
                                const byte source,
                                const std::span<const byte> data) {
  std::vector<byte> rawMessage{format, target, source};
  std::copy(data.begin(), data.end(), std::back_inserter(rawMessage));
  const auto checksum = ::calculateChecksum(rawMessage);
  return checksum;
}

byte Message::dataSize() const { return format & REQUEST_HEADER_SIZE_MASK; }

bool Message::isOBD2Message() const { return dataSize() > 1; }

Message::Message(const byte format, const byte target, const byte source,
                 std::vector<byte> data)
    : format(format), target(target), source(source), data(data),
      checksum(calculateChecksum(format, target, source, data)) {
  if (dataSize() == data.size()) {
    data = std::vector<byte>(dataSize());
  }
}

Message::operator std::vector<byte>() const {
  std::vector<byte> message(dataSize() + MINIMAL_MESSAGE_SIZE);
  message[0] = format;
  message[1] = target;
  message[2] = source;
  std::copy(data.begin(), data.end(), message.begin() + 3);
  message.back() = checksum;
  return message;
}

byte OBD2Message::mode() const { return message.data[0]; }

byte OBD2Message::pid() const { return message.data[1]; }

byte OBD2Message::obd2DataSize() const { return message.dataSize(); }

const std::span<const byte> OBD2Message::obd2Data() const {
  return std::span(message.data.begin() + 2, obd2DataSize());
}

OBD2Message::OBD2Message(Message &message) : message(message) {}

CommandLiteral OBD2Message::command() const {
  return CommandLiteral{mode(), pid()};
}
