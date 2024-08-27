#include <Utils.hpp>

uint8_t calculateChecksum(const std::vector<uint8_t> &message) {
  uint8_t checksum = 0;
  for (const auto byte : message) {
    checksum = (checksum + byte) % std::numeric_limits<uint8_t>::max();
  }
  return checksum;
}

bool isMessageValid(const std::vector<uint8_t> &message,
                    const uint8_t checksum) {
  return checksum == calculateChecksum(message);
};
