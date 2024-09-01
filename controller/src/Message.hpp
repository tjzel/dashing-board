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
  static byte calculateChecksum(const byte format, const byte target,
                                const byte source,
                                const std::span<const byte> data);

  byte dataSize() const;

  bool isOBD2Message() const;

  Message(const byte format, const byte target, const byte source,
          std::vector<byte> data);

  operator std::vector<byte>() const;

  static constexpr byte MINIMAL_MESSAGE_SIZE = 4;
  static constexpr byte REQUEST_HEADER_SIZE_MASK = 0x0f;

  byte format;
  byte target;
  byte source;
  std::vector<byte> data;
  byte checksum;
};

struct OBD2Message {
  byte mode() const;
  byte pid() const;
  byte obd2DataSize() const;
  const std::span<const byte> obd2Data() const;
  OBD2Message(Message &message);
  CommandLiteral command() const;

  Message &message;
};

#endif // MESSAGE_HPP
