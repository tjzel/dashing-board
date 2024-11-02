#ifndef UTILS_HPP
#define UTILS_HPP

#include <DiagnosticCommands.hpp>
#include <ICommunicator.hpp>
#include <Message.hpp>
#include <span>

Byte calculateChecksum(Byte header, Byte target, Byte source, const std::span<const Byte> &data);
Byte calculateChecksum(const std::span<const Byte> &message);

bool isMessageValid(const std::span<const Byte> &message, Byte checksum);

#endif // UTILS_HPP
