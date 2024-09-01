#ifndef UTILS_HPP
#define UTILS_HPP

#include <DiagnosticCommands.hpp>
#include <span>

uint8_t calculateChecksum(const std::span<const byte> &message);

bool isMessageValid(const std::span<const byte> &message, const byte checksum);

#endif // UTILS_HPP
