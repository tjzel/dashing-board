#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>

uint8_t calculateChecksum(const std::vector<uint8_t> &message);

bool isMessageValid(const std::vector<uint8_t> &message, uint8_t checksum);

#endif // UTILS_HPP