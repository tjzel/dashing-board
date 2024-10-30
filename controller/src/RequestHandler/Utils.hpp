#ifndef UTILS_HPP
#define UTILS_HPP

#include <DiagnosticCommands.hpp>
#include <ICommunicator.hpp>
#include <Message.hpp>
#include <span>

Byte calculateChecksum(Byte header, Byte target, Byte source, const std::span<const Byte> &data);
Byte calculateChecksum(const std::span<const Byte> &message);

bool isMessageValid(const std::span<const Byte> &message, Byte checksum);

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

#endif // UTILS_HPP
