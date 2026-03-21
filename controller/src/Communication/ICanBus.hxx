#ifndef ICAN_BUS_HPP
#define ICAN_BUS_HPP

#include <array>
#include <concepts>
#include <cstdint>

static constexpr std::size_t CAN_FRAME_MAX_BYTES = 8;

struct CanFrame {
  uint32_t id = 0;
  uint8_t dlc = 0;
  std::array<uint8_t, CAN_FRAME_MAX_BYTES> data = {};
};

template <class T>
concept ICanBus = requires(T bus, const CanFrame &frame, uint32_t identifier, uint32_t timeoutMs) {
  { bus.send(frame) } -> std::same_as<bool>;
  { bus.receive(timeoutMs) } -> std::same_as<CanFrame>;
  { bus.receiveFrom(identifier, timeoutMs) } -> std::same_as<CanFrame>;
};

#endif // ICAN_BUS_HPP
