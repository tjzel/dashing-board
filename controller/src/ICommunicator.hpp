#ifndef ICOMMUNICATOR_HPP
#define ICOMMUNICATOR_HPP
#include <DiagnosticCommands.hpp>
#include <concepts>
#include <vector>

static constexpr Byte ECU_ADDRESS = 0x33;
static constexpr Byte CONTROLLER_ADDRESS = 0xf1;
static constexpr Byte REQUEST_HEADER = 0xc0;
static constexpr Byte RESPONSE_HEADER = 0x80;
static constexpr Byte REQUEST_HEADER_MODE_MASK = 0xf0;
// TODO: OBD2_MIN_HEADER_SIZE isn't really a thing.
static constexpr Byte OBD2_MIN_HEADER_SIZE = 0x02;

template <class TCommunicator>
concept ICommunicator =
    requires(TCommunicator comm, const Byte dataByte, const std::vector<Byte> message, const std::string str) {
      { comm.read() } -> std::same_as<int>;
      { comm.write(dataByte) } -> std::same_as<void>;
      { comm.write(message) } -> std::same_as<void>;
      { comm.available() } -> std::same_as<bool>;
      { comm.fastInit() } -> std::same_as<void>;
    };

template <class TDebugCommunicator>
concept IDebugCommunicator =
    requires(TDebugCommunicator comm, const Byte dataByte, const std::vector<Byte> message, const std::string str) {
      { comm.print(dataByte) } -> std::same_as<void>;
      { comm.print(str) } -> std::same_as<void>;
      { comm.println(dataByte) } -> std::same_as<void>;
      { comm.println(str) } -> std::same_as<void>;
      { comm.println() } -> std::same_as<void>;
    };

#endif // ICOMMUNICATOR_HPP
