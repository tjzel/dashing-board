#include <Utils.hpp>

using Byte = uint8_t;

Byte calculateChecksum(const std::span<const Byte> &message) {
  Byte checksum = 0;
  for (const auto readByte : message) {
    // checksum = (checksum + readByte) % std::numeric_limits<Byte>::max();
    // TODO: Fix magic number.
    checksum = (checksum + readByte) % 0x100;
  }
  return checksum;
}

bool isMessageValid(const std::span<const Byte> &message, const Byte checksum) {
  return checksum == calculateChecksum(message);
};
