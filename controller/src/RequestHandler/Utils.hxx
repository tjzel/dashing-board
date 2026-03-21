#ifndef UTILS_HPP
#define UTILS_HPP

#include <DiagnosticCommands.hxx>
#include <ICommunicator.hxx>
#include <Message.hxx>
#include <span>

Byte calculateChecksum(Byte header, Byte target, Byte source, const std::span<const Byte> &data);
Byte calculateChecksum(const std::span<const Byte> &message);

bool isMessageValid(const std::span<const Byte> &message, Byte checksum);

#endif // UTILS_HPP
