#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP
#include "Utils.hpp"
#include <DataFrame.hpp>
#include <DiagnosticCommands.hpp>
#include <ICommunicator.hpp>
#include <Parser.hpp>
#include <StateReader.hpp>
#include <map>
#include <vector>

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator> class RequestHandler {
public:
  template <DiagnosticCommands::Command TCommand> typename TCommand::ParsingFormula::ValueType get() {
    const auto command = TCommand::value;
    const auto response = request(command);
    const auto result = Parser<TCommand>::parse(response);
    return result;
  }

  void loadAvailability() {
    loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>();
    loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_20_3F>();
    loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_40_5F>();
    loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_60_7F>();
    loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_80_9F>();
    loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_A0_BF>();
    loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_C0_DF>();
  }

  void printAvailableCommands() const {
    _debugComm.println("Available commands:");
    for (const auto &[key, isAvailable] : _availableCommands) {
      _debugComm.println("    " + static_cast<std::string>(key) + " : " + (isAvailable ? "1" : "0"));
    }
  }

  void printAvailableForDataFrame() const {
    _debugComm.println("Available for data frame:");
    _debugComm.print("    Engine load: ");
    _debugComm.println(isCommandAvailable(DiagnosticCommands::ENGINE_LOAD::value));
    _debugComm.print("    Engine RPM: ");
    _debugComm.println(isCommandAvailable(DiagnosticCommands::ENGINE_RPM::value));
    _debugComm.print("    Vehicle speed: ");
    _debugComm.println(isCommandAvailable(DiagnosticCommands::VEHICLE_SPEED::value));
    _debugComm.print("   Throttle position: ");
    _debugComm.println(isCommandAvailable(DiagnosticCommands::THROTTLE_POSITION::value));
    _debugComm.print("    Uptime: ");
    _debugComm.println(isCommandAvailable(DiagnosticCommands::UPTIME::value));
    _debugComm.print("    Fuel level: ");
    _debugComm.println(isCommandAvailable(DiagnosticCommands::FUEL_LEVEL::value));
    _debugComm.print("    Absolute load: ");
    _debugComm.println(isCommandAvailable(DiagnosticCommands::ABSOLUTE_LOAD::value));
    _debugComm.print("    Relative throttle position: ");
    _debugComm.println(isCommandAvailable(DiagnosticCommands::RELATIVE_THROTTLE_POSITION::value));
    _debugComm.print("    Engine fuel rate: ");
    _debugComm.println(isCommandAvailable(DiagnosticCommands::ENGINE_FUEL_RATE::value));
    _debugComm.println();
  }

  DataFrame getDataFrame() {
    return DataFrame{-1, get<DiagnosticCommands::ENGINE_RPM>(), get<DiagnosticCommands::VEHICLE_SPEED>(), -1, -1, -1, -1, -1,
                     -1};
  }

  bool sniff() {
    while (!_stateReader.feed(_comm.read())) {
      ;
    }
    auto message = _stateReader.getMessage();
    printMessage(message, _debugComm);
  }

  std::vector<Byte> request(CommandLiteral command) {
    Message message{0xc2, 0x33, 0xf1, {command.mode, command.pid}};
    _comm.write(message);

    _debugComm.println("Request sent:");
    printMessage(message, _debugComm);

    while (!_stateReader.feed(_comm.read())) {
      ;
    }
    auto response = _stateReader.getMessage();
    _debugComm.println("Response received:");
    printMessage(response, _debugComm);

    return message.data;
  }

  [[nodiscard]] bool isCommandAvailable(CommandLiteral command) const { return _availableCommands.contains(command); }

  explicit RequestHandler(TCommunicator &comm, TDebugCommunicator &debugComm) : _comm(comm), _debugComm(debugComm) {
    _availableCommands[DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::value] = true;
  }

private:
  template <DiagnosticCommands::CommandAvailability TCommand> void loadAvailabilityForCommand() {
    if (!isCommandAvailable(TCommand::value)) {
      return;
    }

    const auto availability = get<TCommand>();

    for (const auto [key, value] : availability) {
      _availableCommands[key] = value;
    }
  }

  TCommunicator &_comm;
  TDebugCommunicator &_debugComm;
  // TODO: These conditions are far from ideal.
  StateReader _stateReader{
      [](Byte byte) { return (byte & REQUEST_HEADER_MODE_MASK) == 0xc0; },
      [](Byte) { return true; },
      [](Byte byte) { return byte == 0xf1; },
  };
  std::map<CommandLiteral, bool> _availableCommands;
};
#define REQUESTHANDLER_HPP

#endif // REQUEST_HANDLER_HPP
