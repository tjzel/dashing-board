
#ifndef DIAGNOSTIC_COMMANDS_HPP
#define DIAGNOSTIC_COMMANDS_HPP
#include <cstdint>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>

using Byte = uint8_t;

struct CommandLiteral {
  auto operator<=>(const CommandLiteral &) const = default;

  explicit operator std::string() const {
    std::stringstream stream;

    stream << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(mode) << std::dec;
    stream << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(pid) << std::dec;

    return "0x" + stream.str();
  }

  const Byte mode;
  const Byte pid;
};

namespace ParsingFormulas {
template <const Byte TByteCount, typename TValueType> struct Identity {
  constexpr static Byte byteCount = TByteCount;
  using ValueType = TValueType;
};

template <const Byte TByteCount, typename TValueType, const int TNominator, const int TDenominator>
struct MultiplyBy : Identity<TByteCount, TValueType> {
  constexpr static Byte Nominator = TNominator;
  constexpr static Byte Denominator = TDenominator;
};

constexpr Byte percentageNominator = 100;
constexpr Byte percentageDenominator = 255;

template <const Byte TByteCount>
struct Percentage : MultiplyBy<TByteCount, double, percentageNominator, percentageDenominator> {};

struct CommandAvailability : Identity<4, const std::map<CommandLiteral, bool>> {};
} // namespace ParsingFormulas

namespace DiagnosticCommands {
namespace internal {

template <const Byte TMode, const Byte TPid, typename TParsingFormula>
  requires std::is_base_of_v<ParsingFormulas::Identity<TParsingFormula::byteCount, typename TParsingFormula::ValueType>,
                             TParsingFormula>
struct CommandWithFormula {
  constexpr static Byte mode = TMode;
  constexpr static Byte pid = TPid;
  constexpr static CommandLiteral value = {.mode = TMode, .pid = TPid};
  using ParsingFormula = TParsingFormula;
};

template <const Byte TMode, const Byte TPid>
struct PrefixedCommandAvailability : CommandWithFormula<TMode, TPid, ParsingFormulas::CommandAvailability> {};
} // namespace internal

template <class TCommand>
concept Command =
    std::is_base_of_v<internal::CommandWithFormula<TCommand::mode, TCommand::pid, typename TCommand::ParsingFormula>, TCommand>;

template <class TCommand>
concept CommandAvailability =
    std::is_base_of_v<internal::CommandWithFormula<TCommand::mode, TCommand::pid, ParsingFormulas::CommandAvailability>,
                      TCommand>;

struct COMMAND_AVAILABILITY_00_1F : internal::PrefixedCommandAvailability<0x01, 0x00> {};
struct ENGINE_LOAD : internal::CommandWithFormula<0x01, 0x04, ParsingFormulas::Percentage<1>> {};
struct ENGINE_RPM : internal::CommandWithFormula<0x01, 0x0C, ParsingFormulas::MultiplyBy<2, int, 1, 4>> {};

struct VEHICLE_SPEED : internal::CommandWithFormula<0x01, 0x0D, ParsingFormulas::Identity<1, int>> {};
struct THROTTLE_POSITION : internal::CommandWithFormula<0x01, 0x11, ParsingFormulas::Percentage<1>> {};
struct UPTIME : internal::CommandWithFormula<0x01, 0x1F, ParsingFormulas::Identity<2, int>> {};

struct COMMAND_AVAILABILITY_20_3F : internal::PrefixedCommandAvailability<0x01, 0x20> {};
struct FUEL_LEVEL : internal::CommandWithFormula<0x01, 0x2F, ParsingFormulas::Percentage<1>> {};

struct COMMAND_AVAILABILITY_40_5F : internal::PrefixedCommandAvailability<0x01, 0x40> {};
struct ABSOLUTE_LOAD : internal::CommandWithFormula<0x01, 0x43, ParsingFormulas::Percentage<2>> {};
struct RELATIVE_THROTTLE_POSITION : internal::CommandWithFormula<0x01, 0x45, ParsingFormulas::Percentage<1>> {};
struct ENGINE_FUEL_RATE : internal::CommandWithFormula<0x01, 0x5E, ParsingFormulas::MultiplyBy<2, double, 1, 20>> {};

struct COMMAND_AVAILABILITY_60_7F : internal::PrefixedCommandAvailability<0x01, 0x60> {};

struct COMMAND_AVAILABILITY_80_9F : internal::PrefixedCommandAvailability<0x01, 0x80> {};

struct COMMAND_AVAILABILITY_A0_BF : internal::PrefixedCommandAvailability<0x01, 0xA0> {};

struct COMMAND_AVAILABILITY_C0_DF : internal::PrefixedCommandAvailability<0x01, 0xC0> {};

} // namespace DiagnosticCommands
#endif // DIAGNOSTIC_COMMANDS_HPP
