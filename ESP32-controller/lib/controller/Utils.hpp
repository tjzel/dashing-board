#ifndef UTILS_HPP
#define UTILS_HPP

#include <DiagnosticCommands.hpp>
#include <span>

uint8_t calculateChecksum(const std::span<const Byte> &message);

bool isMessageValid(const std::span<const Byte> &message, const Byte checksum);

#endif // UTILS_HPP
