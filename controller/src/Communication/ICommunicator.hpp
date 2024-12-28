#ifndef ICOMMUNICATOR_HPP
#define ICOMMUNICATOR_HPP
#include <DiagnosticCommands.hpp>
#include <concepts>
#include <functional>
#include <vector>

static constexpr Byte ADDRESSING_MASK = 0b11000000;
static constexpr Byte PHYSICAL_ADDRESSING = 0b10000000;
static constexpr Byte FUNCTIONAL_ADDRESSING = 0b11000000;
static constexpr Byte LENGTH_MASK = 0b00111111;

static constexpr Byte ECU_ADDRESS = 0x33;
static constexpr Byte CONTROLLER_ADDRESS = 0xf1;

template <class TCommunicator>
concept ICommunicator =
    requires(TCommunicator comm, const Byte dataByte, const std::vector<Byte> message,
             const std::string str, std::function<void()> onNewData) {
      { comm.read() } -> std::same_as<int>;
      { comm.write(dataByte) } -> std::same_as<void>;
      { comm.write(message) } -> std::same_as<void>;
      { comm.available() } -> std::same_as<bool>;
      { comm.init() } -> std::same_as<void>;
      { comm.setOnNewData(onNewData) } -> std::same_as<void>;
    };

template <class TDebugCommunicator>
concept IDebugCommunicator = requires(TDebugCommunicator comm, const Byte dataByte,
                                      const std::vector<Byte> message, const std::string str) {
  { comm.print(dataByte) } -> std::same_as<void>;
  { comm.print(str) } -> std::same_as<void>;
  { comm.println(dataByte) } -> std::same_as<void>;
  { comm.println(str) } -> std::same_as<void>;
  { comm.println() } -> std::same_as<void>;
};

#endif // ICOMMUNICATOR_HPP
