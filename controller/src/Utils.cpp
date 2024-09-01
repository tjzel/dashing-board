#include <Utils.hpp>

byte calculateChecksum(const std::span<const byte> &message) {
  byte checksum = 0;
  for (const auto readByte : message) {
    checksum = (checksum + readByte) % std::numeric_limits<byte>::max();
  }
  return checksum;
}

bool isMessageValid(const std::span<const byte> &message, const byte checksum) {
  return checksum == calculateChecksum(message);
};
