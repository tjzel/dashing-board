#include <Utils.hpp>

Byte calculateChecksum(const std::span<const Byte> &message) {
  Byte checksum = 0;
  for (const auto readByte : message) {
    checksum = (checksum + readByte) % std::numeric_limits<Byte>::max();
  }
  return checksum;
}

bool isMessageValid(const std::span<const Byte> &message, const Byte checksum) {
  return checksum == calculateChecksum(message);
};
