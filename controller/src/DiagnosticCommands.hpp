#ifndef DIAGNOSTICCOMMANDS_HPP
#define DIAGNOSTICCOMMANDS_HPP
#include <map>
#include <string>
#include <vector>

namespace ParsingFormulas {
template <const uint8_t TByteCount, typename TValueType> struct Identity {
  constexpr static const uint8_t byteCount = TByteCount;
  typedef TValueType ValueType;
};

template <const uint8_t TByteCount, typename TValueType,
          const uint8_t Multiplier>
struct MultiplyBy : Identity<TByteCount, TValueType> {};

template <const uint8_t TByteCount, typename TValueType, const int Nominator,
          const int Denominator>
struct DivideBy : Identity<TByteCount, TValueType> {};

template <const uint8_t TByteCount>
struct Percentage : DivideBy<TByteCount, double, 255, 100> {};

struct CommandAvailability : Identity<4, const std::vector<bool>> {};
} // namespace ParsingFormulas

namespace DiagnosticCommands {
namespace internal {

template <const uint8_t TPrefix, const uint8_t TCommand>
struct PrefixedCommand {
  constexpr static const uint8_t prefix = TPrefix;
  constexpr static const uint8_t command = TCommand;
  constexpr static const std::array<uint8_t, 2> value = {TPrefix, TCommand};
};

template <const uint8_t TPrefix, const uint8_t TCommand,
          typename TParsingAlgorithm>
  requires std::is_base_of_v<
      ParsingFormulas::Identity<TParsingAlgorithm::byteCount,
                                typename TParsingAlgorithm::ValueType>,
      TParsingAlgorithm>
struct PrefixedCommandWithResponse : PrefixedCommand<TPrefix, TCommand> {
  typedef TParsingAlgorithm ParsingAlgorithm;
};

template <const uint8_t TPrefix, const uint8_t TCommand>
struct PrefixedCommandAvailability
    : PrefixedCommandWithResponse<TPrefix, TCommand,
                                  ParsingFormulas::CommandAvailability> {};
} // namespace internal

template <class TCommand>
concept Command = std::is_base_of_v<
    internal::PrefixedCommandWithResponse<TCommand::prefix, TCommand::command,
                                          typename TCommand::ParsingAlgorithm>,
    TCommand>;

template <class TCommand>
concept CommandAvailability = std::is_base_of_v<
    internal::PrefixedCommandWithResponse<TCommand::prefix, TCommand::command,
                                          ParsingFormulas::CommandAvailability>,
    TCommand>;

struct COMMAND_AVAILABILITY_00_1F
    : internal::PrefixedCommandAvailability<0x01, 0x00> {};
struct ENGINE_LOAD
    : internal::PrefixedCommandWithResponse<0x01, 0x04,
                                            ParsingFormulas::Percentage<1>> {};
struct ENGINE_RPM : internal::PrefixedCommandWithResponse<
                        0x01, 0x0C, ParsingFormulas::DivideBy<2, int, 1, 4>> {};

struct VEHICLE_SPEED
    : internal::PrefixedCommandWithResponse<0x01, 0x0D,
                                            ParsingFormulas::Identity<1, int>> {
};
struct THROTTLE_POSITION
    : internal::PrefixedCommandWithResponse<0x01, 0x11,
                                            ParsingFormulas::Percentage<1>> {};
struct UPTIME
    : internal::PrefixedCommandWithResponse<0x01, 0x1F,
                                            ParsingFormulas::Identity<2, int>> {
};

struct COMMAND_AVAILABILITY_20_3F
    : internal::PrefixedCommandAvailability<0x01, 0x20> {};
struct FUEL_LEVEL
    : internal::PrefixedCommandWithResponse<0x01, 0x2F,
                                            ParsingFormulas::Percentage<1>> {};

struct COMMAND_AVAILABILITY_40_5F
    : internal::PrefixedCommandAvailability<0x01, 0x40> {};
struct ABSOLUTE_LOAD
    : internal::PrefixedCommandWithResponse<0x01, 0x43,
                                            ParsingFormulas::Percentage<2>> {};
struct RELATIVE_THROTTLE_POSITION
    : internal::PrefixedCommandWithResponse<0x01, 0x45,
                                            ParsingFormulas::Percentage<1>> {};
struct ENGINE_FUEL_RATE
    : internal::PrefixedCommandWithResponse<
          0x01, 0x5E, ParsingFormulas::DivideBy<2, int, 1, 20>> {};

struct COMMAND_AVAILABILITY_60_7F
    : internal::PrefixedCommandAvailability<0x01, 0x60> {};

struct COMMAND_AVAILABILITY_80_9F
    : internal::PrefixedCommandAvailability<0x01, 0x80> {};

struct COMMAND_AVAILABILITY_A0_BF
    : internal::PrefixedCommandAvailability<0x01, 0xA0> {};

struct COMMAND_AVAILABILITY_C0_DF
    : internal::PrefixedCommandAvailability<0x01, 0xC0> {};

constexpr static const std::array<std::array<uint8_t, 2>, 1>
    DEFAULT_AVAILABILITY{COMMAND_AVAILABILITY_00_1F::value};

} // namespace DiagnosticCommands
#endif // DIAGNOSTICCOMMANDS_HPP
