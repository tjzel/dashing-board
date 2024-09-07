#include <Utils.hpp>

using Byte = uint8_t;

constexpr Byte BYTE_MAX = 0xff;

Byte calculateChecksum(const Byte header, const Byte target, const Byte source, const std::span<const Byte> &data) {
  uint checksum = 0;
  checksum = (checksum + header);
  checksum = (checksum + target);
  checksum = (checksum + source);
  for (const auto readByte : data) {
    checksum += readByte;
  }
  checksum = checksum % BYTE_MAX;
  return checksum;
}

Byte calculateChecksum(const std::span<const Byte> &message) {
  uint checksum = 0;
  for (const auto readByte : message) {
    checksum += readByte;
  }
  checksum = checksum % BYTE_MAX;
  return checksum;
}

bool isMessageValid(const std::span<const Byte> &message, const Byte checksum) {
  return checksum == calculateChecksum(message);
};
