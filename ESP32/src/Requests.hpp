#ifndef REQUESTS_HPP
#define REQUESTS_HPP
#include <map>
#include <string>
// #include <type_traits>
#include <vector>

namespace Requests {

template <const uint8_t TPrefix, const uint8_t TCommand>
struct PrefixedCommand {
  constexpr static const uint8_t prefix = TPrefix;
  constexpr static const uint8_t command = TCommand;
  constexpr static const uint8_t value[] = {TPrefix, TCommand};
  static std::vector<uint8_t> getValue() { return {TPrefix, TCommand}; }
};

template <const uint8_t TPrefix, const uint8_t TCommand, typename TParsedType>
struct PrefixedCommandWithResponse : PrefixedCommand<TPrefix, TCommand> {
  typedef TParsedType ParsedType;
};

// template <typename TPrefixedCommand, const uint8_t TPrefix,
//           const uint8_t TCommand>
// concept IsPrefixedCommand = std::is_base_of<PrefixedCommand<TPrefix,
// TCommand>,
//                                             TPrefixedCommand>::value;

template <const uint8_t TPrefix, const uint8_t TCommand>
struct IntegerCommand : PrefixedCommandWithResponse<TPrefix, TCommand, int> {};

template <const uint8_t TPrefix, const uint8_t TCommand>
struct FloatCommand : PrefixedCommandWithResponse<TPrefix, TCommand, double> {};

struct AVAILABLE_DATA_COMMANDS_00_1F : IntegerCommand<0x01, 0x00> {};
struct ENGINE_LOAD : FloatCommand<0x01, 0x04> {};
struct ENGINE_RPM : IntegerCommand<0x01, 0x0C> {};
struct VEHICLE_SPEED : IntegerCommand<0x01, 0x0D> {};
struct THROTTLE_POSITION : FloatCommand<0x01, 0x11> {};
struct UPTIME : IntegerCommand<0x01, 0x1F> {};

struct AVAILABLE_DATA_COMMANDS_20_3F : IntegerCommand<0x01, 0x20> {};
struct FUEL_LEVEL : FloatCommand<0x01, 0x2F> {};

struct AVAILABLE_DATA_COMMANDS_40_5F : IntegerCommand<0x01, 0x40> {};
struct ABSOLUTE_LOAD : FloatCommand<0x01, 0x43> {};
struct RELATIVE_THROTTLE_POSITION : FloatCommand<0x01, 0x45> {};
struct ENGINE_FUEL_RATE : FloatCommand<0x01, 0x5E> {};

struct AVAILABLE_DATA_COMMANDS_60_7F : IntegerCommand<0x01, 0x60> {};

struct AVAILABLE_DATA_COMMANDS_80_9F : IntegerCommand<0x01, 0x80> {};

struct AVAILABLE_DATA_COMMANDS_A0_BF : IntegerCommand<0x01, 0xA0> {};

struct AVAILABLE_DATA_COMMANDS_C0_DF : IntegerCommand<0x01, 0xC0> {};

static const std::vector<const uint8_t[2]> AVAILABLE_DATA_COMMANDS{
    AVAILABLE_DATA_COMMANDS_00_1F::getValue(),
    AVAILABLE_DATA_COMMANDS_20_3F::getValue(),
    AVAILABLE_DATA_COMMANDS_40_5F::getValue(),
    AVAILABLE_DATA_COMMANDS_60_7F::getValue(),
    AVAILABLE_DATA_COMMANDS_80_9F::getValue(),
    AVAILABLE_DATA_COMMANDS_A0_BF::getValue(),
    AVAILABLE_DATA_COMMANDS_C0_DF::getValue()};

static const std::vector<const uint8_t[2]> defaultAvailable{
    AVAILABLE_DATA_COMMANDS_00_1F::getValue()};

} // namespace Requests
#endif // REQUESTS_HPP
