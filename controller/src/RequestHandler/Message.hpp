#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <DiagnosticCommands.hpp>
#include <ICommunicator.hpp>
#include <span>
#include <vector>

struct Message {
  static Byte calculateChecksum(Byte format, Byte target, Byte source, std::span<const Byte> data);
  [[nodiscard]] Byte dataSize() const;
  [[nodiscard]] bool isOBD2Message() const;
  [[nodiscard]] bool isInitMessage() const;
  Message(Byte format, Byte target, Byte source, std::vector<Byte> data);
  explicit operator std::vector<Byte>() const;

  static constexpr Byte MINIMAL_MESSAGE_SIZE = 4;
  static constexpr Byte REQUEST_HEADER_SIZE_MASK = 0x0f;
  Byte format;
  Byte target;
  Byte source;
  std::vector<Byte> data;
  Byte checksum;
};

struct OBD2Message {
  [[nodiscard]] Byte format() const;
  [[nodiscard]] Byte target() const;
  [[nodiscard]] Byte source() const;
  [[nodiscard]] Byte mode() const;
  [[nodiscard]] Byte pid() const;
  [[nodiscard]] Byte obd2DataSize() const;
  [[nodiscard]] std::span<const Byte> obd2Data() const;
  [[nodiscard]] CommandLiteral command() const;
  explicit OBD2Message(Message &message);

  Message &message;
};

template <IDebugCommunicator TDebugCommunicator> void printMessage(const Message &message, TDebugCommunicator &debugComm) {
  debugComm.print("    Format: ");
  debugComm.println(message.format);
  debugComm.print("    Target: ");
  debugComm.println(message.target);
  debugComm.print("    Source: ");
  debugComm.println(message.source);
  debugComm.print("    Data: ");
  for (Byte byte : message.data) {
    debugComm.print(byte);
    debugComm.print(" ");
  }
  debugComm.println();
  debugComm.print("    Checksum: ");
  debugComm.println(message.checksum);
}

#endif // MESSAGE_HPP
