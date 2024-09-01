#ifndef DIAGNOSTIC_COMMANDS_HPP
#define DIAGNOSTIC_COMMANDS_HPP
#include <iomanip>
#include <map>
#include <sstream>
#include <string>

typedef uint8_t byte;

struct CommandLiteral {
  // TODO: CLion has a problem with <=> operator here.
  auto operator<=>(const CommandLiteral &) const = default;

  operator std::string() const {
    std::stringstream ss;

    ss << std::hex << std::setfill('0') << std::setw(2)
       << static_cast<int>(mode);
    ss << std::hex << std::setfill('0') << std::setw(2)
       << static_cast<int>(pid);

    return "0x" + ss.str();
  }

  const byte mode;
  const byte pid;
};

namespace ParsingFormulas {
template <const byte TByteCount, typename TValueType> struct Identity {
  constexpr static byte byteCount = TByteCount;
  typedef TValueType ValueType;
};

template <const byte TByteCount, typename TValueType, const int TNominator,
          const int TDenominator>
struct MultiplyBy : Identity<TByteCount, TValueType> {
  constexpr static byte Nominator = TNominator;
  constexpr static byte Denominator = TDenominator;
};

template <const byte TByteCount>
struct Percentage : MultiplyBy<TByteCount, double, 100, 255> {};

struct CommandAvailability : Identity<4, const std::map<CommandLiteral, bool>> {
};
} // namespace ParsingFormulas

namespace DiagnosticCommands {
namespace internal {

template <const byte TMode, const byte TPid> struct FullCommand {
  constexpr static byte mode = TMode;
  constexpr static byte pid = TPid;
  constexpr static CommandLiteral value = {TMode, TPid};
};

template <const byte TMode, const byte TPid, typename TParsingFormula>
  requires std::is_base_of_v<
      ParsingFormulas::Identity<TParsingFormula::byteCount,
                                typename TParsingFormula::ValueType>,
      TParsingFormula>
struct FullCommandWithResponse : FullCommand<TMode, TPid> {
  typedef TParsingFormula ParsingFormula;
};

template <const byte TMode, const byte TPid>
struct PrefixedCommandAvailability
    : FullCommandWithResponse<TMode, TPid,
                              ParsingFormulas::CommandAvailability> {};
} // namespace internal

template <class TCommand>
concept Command = std::is_base_of_v<
    internal::FullCommandWithResponse<TCommand::mode, TCommand::pid,
                                      typename TCommand::ParsingFormula>,
    TCommand>;

template <class TCommand>
concept CommandAvailability = std::is_base_of_v<
    internal::FullCommandWithResponse<TCommand::mode, TCommand::pid,
                                      ParsingFormulas::CommandAvailability>,
    TCommand>;

struct COMMAND_AVAILABILITY_00_1F
    : internal::PrefixedCommandAvailability<0x01, 0x00> {};
struct ENGINE_LOAD
    : internal::FullCommandWithResponse<0x01, 0x04,
                                        ParsingFormulas::Percentage<1>> {};
struct ENGINE_RPM : internal::FullCommandWithResponse<
                        0x01, 0x0C, ParsingFormulas::MultiplyBy<2, int, 1, 4>> {
};

struct VEHICLE_SPEED
    : internal::FullCommandWithResponse<0x01, 0x0D,
                                        ParsingFormulas::Identity<1, int>> {};
struct THROTTLE_POSITION
    : internal::FullCommandWithResponse<0x01, 0x11,
                                        ParsingFormulas::Percentage<1>> {};
struct UPTIME
    : internal::FullCommandWithResponse<0x01, 0x1F,
                                        ParsingFormulas::Identity<2, int>> {};

struct COMMAND_AVAILABILITY_20_3F
    : internal::PrefixedCommandAvailability<0x01, 0x20> {};
struct FUEL_LEVEL
    : internal::FullCommandWithResponse<0x01, 0x2F,
                                        ParsingFormulas::Percentage<1>> {};

struct COMMAND_AVAILABILITY_40_5F
    : internal::PrefixedCommandAvailability<0x01, 0x40> {};
struct ABSOLUTE_LOAD
    : internal::FullCommandWithResponse<0x01, 0x43,
                                        ParsingFormulas::Percentage<2>> {};
struct RELATIVE_THROTTLE_POSITION
    : internal::FullCommandWithResponse<0x01, 0x45,
                                        ParsingFormulas::Percentage<1>> {};
struct ENGINE_FUEL_RATE
    : internal::FullCommandWithResponse<
          0x01, 0x5E, ParsingFormulas::MultiplyBy<2, int, 1, 20>> {};

struct COMMAND_AVAILABILITY_60_7F
    : internal::PrefixedCommandAvailability<0x01, 0x60> {};

struct COMMAND_AVAILABILITY_80_9F
    : internal::PrefixedCommandAvailability<0x01, 0x80> {};

struct COMMAND_AVAILABILITY_A0_BF
    : internal::PrefixedCommandAvailability<0x01, 0xA0> {};

struct COMMAND_AVAILABILITY_C0_DF
    : internal::PrefixedCommandAvailability<0x01, 0xC0> {};

constexpr static std::array<CommandLiteral, 1> DEFAULT_AVAILABILITY{
    COMMAND_AVAILABILITY_00_1F::value};

} // namespace DiagnosticCommands
#endif // DIAGNOSTIC_COMMANDS_HPP
