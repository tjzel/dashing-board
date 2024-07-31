#ifndef REQUESTS_HPP
#define REQUESTS_HPP
#include <string>
#include <vector>

namespace Literals {
  constexpr char RESET[] = "ATZ";
  constexpr char ECHO_OFF[] = "ATE0";
  constexpr char ECHO_ON[] = "ATE1";
  constexpr char LINEFEED_OFF[] = "ATL0";
  constexpr char LINEFEED_ON[] = "ATL1";
  constexpr char SPACES_OFF[] = "ATS0";
  constexpr char SPACES_ON[] = "ATS1";
  constexpr char HEADER_OFF[] = "ATH0";
  constexpr char HEADER_ON[] = "ATH1";

  constexpr char AVAILABLE_DATA_COMMANDS_00_1F[] = "0100";
  constexpr char ENGINE_LOAD[] = "0104";
  constexpr char ENGINE_RPM[] = "010C";
  constexpr char VEHICLE_SPEED[] = "010D";
  constexpr char THROTTLE_POSITION[] = "0111";
  constexpr char UPTIME[] = "011F";

  constexpr char AVAILABLE_DATA_COMMANDS_20_3F[] = "0120";
  constexpr char FUEL_LEVEL[] = "012F";

  constexpr char AVAILABLE_DATA_COMMANDS_40_5F[] = "0140";
  constexpr char ABSOLUTE_LOAD[] = "0143";
  constexpr char RELATIVE_THROTTLE_POSITION[] = "0145";
  constexpr char ENGINE_FUEL_RATE[] = "015E";

  constexpr char AVAILABLE_DATA_COMMANDS_60_7F[] = "0160";

  constexpr char AVAILABLE_DATA_COMMANDS_80_9F[] = "0180";

  constexpr char AVAILABLE_DATA_COMMANDS_A0_BF[] = "01A0";

  constexpr char AVAILABLE_DATA_COMMANDS_C0_DF[] = "01C0";
}

namespace Requests {
  template <const char TCommandLiteral[]>
  struct BaseCommand {
    typedef void Type;
    static constexpr auto value = TCommandLiteral;
    static constexpr std::string toString() {
      return { value };
    }
  };

  template <const char* const TCommandLiteral>
  struct IntegerCommand : BaseCommand<TCommandLiteral> {
    typedef int Type;
  };

  template <const char* const TCommandLiteral>
  struct FloatCommand : BaseCommand<TCommandLiteral> {
    typedef double Type;
  };

  struct RESET : IntegerCommand<Literals::RESET> { };
  struct ECHO_OFF : IntegerCommand<Literals::ECHO_OFF> { };
  struct ECHO_ON : IntegerCommand<Literals::ECHO_ON> { };
  struct LINEFEED_OFF : IntegerCommand<Literals::LINEFEED_OFF> { };
  struct LINEFEED_ON : IntegerCommand<Literals::LINEFEED_ON> { };
  struct SPACE_OFF : IntegerCommand<Literals::SPACES_OFF> { };
  struct SPACE_ON : IntegerCommand<Literals::SPACES_ON> { };
  struct HEADER_OFF : IntegerCommand<Literals::HEADER_OFF> { };
  struct HEADER_ON : IntegerCommand<Literals::HEADER_ON> { };

  struct AVAILABLE_DATA_COMMANDS_00_1F : IntegerCommand<Literals::AVAILABLE_DATA_COMMANDS_00_1F> { };
  struct ENGINE_LOAD : FloatCommand<Literals::ENGINE_LOAD> { };

  struct ENGINE_RPM : IntegerCommand<Literals::ENGINE_RPM> { };
  struct VEHICLE_SPEED : IntegerCommand<Literals::VEHICLE_SPEED> { };
  struct THROTTLE_POSITION : FloatCommand<Literals::THROTTLE_POSITION> { };
  struct UPTIME : IntegerCommand<Literals::UPTIME> { };

  struct AVAILABLE_DATA_COMMANDS_20_3F : IntegerCommand<Literals::AVAILABLE_DATA_COMMANDS_20_3F> { };
  struct FUEL_LEVEL : FloatCommand<Literals::FUEL_LEVEL> { };

  struct AVAILABLE_DATA_COMMANDS_40_5F : IntegerCommand<Literals::AVAILABLE_DATA_COMMANDS_40_5F> { };
  struct ABSOLUTE_LOAD : FloatCommand<Literals::ABSOLUTE_LOAD> { };
  struct RELATIVE_THROTTLE_POSITION : FloatCommand<Literals::RELATIVE_THROTTLE_POSITION> { };
  struct ENGINE_FUEL_RATE : FloatCommand<Literals::ENGINE_FUEL_RATE> { };

  struct AVAILABLE_DATA_COMMANDS_60_7F : IntegerCommand<Literals::AVAILABLE_DATA_COMMANDS_60_7F> { };

  struct AVAILABLE_DATA_COMMANDS_80_9F : IntegerCommand<Literals::AVAILABLE_DATA_COMMANDS_80_9F> { };

  struct AVAILABLE_DATA_COMMANDS_A0_BF : IntegerCommand<Literals::AVAILABLE_DATA_COMMANDS_A0_BF> { };

  struct AVAILABLE_DATA_COMMANDS_C0_DF : IntegerCommand<Literals::AVAILABLE_DATA_COMMANDS_C0_DF> { };

  const std::vector<const std::string>
    AVAILABLE_DATA_COMMANDS{
      AVAILABLE_DATA_COMMANDS_00_1F::toString(), AVAILABLE_DATA_COMMANDS_20_3F::toString(), AVAILABLE_DATA_COMMANDS_40_5F::toString(), AVAILABLE_DATA_COMMANDS_60_7F::toString(), AVAILABLE_DATA_COMMANDS_80_9F::toString(), AVAILABLE_DATA_COMMANDS_A0_BF::toString(), AVAILABLE_DATA_COMMANDS_C0_DF::toString()
  };
} // namespace Requests
#endif // REQUESTS_HPP
