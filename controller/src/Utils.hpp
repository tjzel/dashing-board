#ifndef UTILS_HPP
#define UTILS_HPP

#include <DiagnosticCommands.hpp>
#include <span>

Byte calculateChecksum(const Byte header, const Byte target, const Byte source, const std::span<const Byte> &data);
Byte calculateChecksum(const std::span<const Byte> &message);

bool isMessageValid(const std::span<const Byte> &message, const Byte checksum);

#endif // UTILS_HPP
