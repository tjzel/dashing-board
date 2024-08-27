#ifndef ICOMMUNICATOR_HPP
#define ICOMMUNICATOR_HPP
#include <vector>

template <class TCommunicator>
concept ICommunicator =
    requires(TCommunicator comm, const uint8_t byte,
             const std::vector<uint8_t> message, const std::string str) {
      { comm.read() } -> std::same_as<uint8_t>;
      { comm.write(byte) } -> std::same_as<void>;
      { comm.write(message) } -> std::same_as<void>;
    };

template <class TDebugCommunicator>
concept IDebugCommunicator =
    requires(TDebugCommunicator comm, const uint8_t byte,
             const std::vector<uint8_t> message, const std::string str) {
      { comm.print(byte) } -> std::same_as<void>;
      { comm.print(str) } -> std::same_as<void>;
      { comm.println(byte) } -> std::same_as<void>;
      { comm.println(str) } -> std::same_as<void>;
      { comm.println() } -> std::same_as<void>;
    };

#endif // ICOMMUNICATOR_HPP