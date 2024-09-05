#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <DiagnosticCommands.hpp>
#include <span>
#include <vector>

/**
 * K-Line (OBD2) message has the following structure:
 *
 * FORMAT | TARGET | SOURCE | DATA (OBD2 MESSAGE) | CHECKSUM
 *
 * OBD2 MESSAGE (PAYLOAD) has the following structure:
 *
 * MODE | PID | DATA
 */
struct Message {
  static Byte calculateChecksum(const Byte format, const Byte target, const Byte source, const std::span<const Byte> data);

  Byte dataSize() const;

  bool isOBD2Message() const;

  bool isInitMessage() const;

  Message(const Byte format, const Byte target, const Byte source, std::vector<Byte> data);

  operator std::vector<Byte>() const;

  static constexpr Byte MINIMAL_MESSAGE_SIZE = 4;
  static constexpr Byte REQUEST_HEADER_SIZE_MASK = 0x0f;

  Byte format;
  Byte target;
  Byte source;
  std::vector<Byte> data;
  Byte checksum;
};

struct OBD2Message {
  Byte format() const;
  Byte target() const;
  Byte source() const;
  Byte mode() const;
  Byte pid() const;
  Byte obd2DataSize() const;
  const std::span<const Byte> obd2Data() const;
  OBD2Message(Message &message);
  CommandLiteral command() const;

  Message &message;
};

#endif // MESSAGE_HPP
