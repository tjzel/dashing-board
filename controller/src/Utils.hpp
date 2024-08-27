#ifndef UTILS_HPP
#define UTILS_HPP

#include <limits>
#include <vector>

uint8_t calculateChecksum(const std::vector<uint8_t> message);

bool isMessageValid(const std::vector<uint8_t> message, const uint8_t checksum);

#endif // UTILS_HPP