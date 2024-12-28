
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

    stream << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(pid) << std::dec;

    return "0x" + stream.str();
  }

  const Byte pid;
};

namespace Encodings {
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
} // namespace Encodings

namespace DiagnosticCommands {

constexpr Byte REQUEST_MODE = 0x01;
constexpr Byte RESPONSE_MODE = 0x41;

namespace internal {

template <const Byte TPid, typename TEncoding>
  requires std::is_base_of_v<
      Encodings::Identity<TEncoding::byteCount, typename TEncoding::ValueType>, TEncoding>
struct CommandWithEncoding {
  constexpr static Byte pid = TPid;
  constexpr static CommandLiteral value = {.pid = TPid};
  using Encoding = TEncoding;
};

template <const Byte TPid>
struct PrefixedCommandAvailability : CommandWithEncoding<TPid, Encodings::CommandAvailability> {};
} // namespace internal

template <class TCommand>
concept Command =
    std::is_base_of_v<internal::CommandWithEncoding<TCommand::pid, typename TCommand::Encoding>,
                      TCommand>;

template <class TCommand>
concept CommandAvailability =
    std::is_base_of_v<internal::CommandWithEncoding<TCommand::pid, Encodings::CommandAvailability>,
                      TCommand>;

struct COMMAND_AVAILABILITY_00_1F : internal::PrefixedCommandAvailability<0x00> {};
struct ENGINE_LOAD : internal::CommandWithEncoding<0x04, Encodings::Percentage<1>> {};
struct ENGINE_RPM : internal::CommandWithEncoding<0x0C, Encodings::MultiplyBy<2, int, 1, 4>> {};
struct VEHICLE_SPEED : internal::CommandWithEncoding<0x0D, Encodings::Identity<1, int>> {};
struct THROTTLE_POSITION : internal::CommandWithEncoding<0x11, Encodings::Percentage<1>> {};
struct UPTIME : internal::CommandWithEncoding<0x1F, Encodings::Identity<2, int>> {};

struct COMMAND_AVAILABILITY_20_3F : internal::PrefixedCommandAvailability<0x20> {};
struct FUEL_LEVEL : internal::CommandWithEncoding<0x2F, Encodings::Percentage<1>> {};

struct COMMAND_AVAILABILITY_40_5F : internal::PrefixedCommandAvailability<0x40> {};
struct ABSOLUTE_LOAD : internal::CommandWithEncoding<0x43, Encodings::Percentage<2>> {};
struct RELATIVE_THROTTLE_POSITION : internal::CommandWithEncoding<0x45, Encodings::Percentage<1>> {
};
struct ENGINE_FUEL_RATE
    : internal::CommandWithEncoding<0x5E, Encodings::MultiplyBy<2, double, 1, 20>> {};

struct COMMAND_AVAILABILITY_60_7F : internal::PrefixedCommandAvailability<0x60> {};

struct COMMAND_AVAILABILITY_80_9F : internal::PrefixedCommandAvailability<0x80> {};

struct COMMAND_AVAILABILITY_A0_BF : internal::PrefixedCommandAvailability<0xA0> {};

struct COMMAND_AVAILABILITY_C0_DF : internal::PrefixedCommandAvailability<0xC0> {};

} // namespace DiagnosticCommands
#endif // DIAGNOSTIC_COMMANDS_HPP
