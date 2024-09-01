#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP
#include "ICommunicator.hpp"
#include "Utils.hpp"
#include <DataFrame.hpp>
#include <DiagnosticCommands.hpp>
#include <Parser.hpp>
#include <map>
#include <vector>

template <ICommunicator ICommunicator> class RequestHandler {
public:
  explicit RequestHandler(ICommunicator &comm, ICommunicator &debugComm)
      : _comm(comm), _debugComm(debugComm) {
    for (auto &command : DiagnosticCommands::DEFAULT_AVAILABILITY) {
      _availableCommands[command] = true;
    }
  }

  template <DiagnosticCommands::Command TCommand>
  typename TCommand::ParsingFormula::ValueType get() {
    const auto command = TCommand::value;
    const auto response = request(command);
    const auto result = Parser<TCommand>::parse(response);
    return result;
  }

  void loadAvailability() {
    loadAvailabilityForCommand<
        DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>();
    loadAvailabilityForCommand<
        DiagnosticCommands::COMMAND_AVAILABILITY_20_3F>();
    loadAvailabilityForCommand<
        DiagnosticCommands::COMMAND_AVAILABILITY_40_5F>();
    loadAvailabilityForCommand<
        DiagnosticCommands::COMMAND_AVAILABILITY_60_7F>();
    loadAvailabilityForCommand<
        DiagnosticCommands::COMMAND_AVAILABILITY_80_9F>();
    loadAvailabilityForCommand<
        DiagnosticCommands::COMMAND_AVAILABILITY_A0_BF>();
    loadAvailabilityForCommand<
        DiagnosticCommands::COMMAND_AVAILABILITY_C0_DF>();
  }

  void printAvailableCommands() const {
    _debugComm.println("Available commands:");
    for (const auto &[key, isAvailable] : _availableCommands) {
      _debugComm.println("    " + static_cast<std::string>(key) + " : " +
                         (isAvailable ? "1" : "0"));
    }
  }

  void printAvailableForDataFrame() const {
    _debugComm.println("Available for data frame:");
    _debugComm.print("    Engine load: ");
    _debugComm.println(
        isCommandAvailable(DiagnosticCommands::ENGINE_LOAD::value));
    _debugComm.print("    Engine RPM: ");
    _debugComm.println(
        isCommandAvailable(DiagnosticCommands::ENGINE_RPM::value));
    _debugComm.print("    Vehicle speed: ");
    _debugComm.println(
        isCommandAvailable(DiagnosticCommands::VEHICLE_SPEED::value));
    _debugComm.print("   Throttle position: ");
    _debugComm.println(
        isCommandAvailable(DiagnosticCommands::THROTTLE_POSITION::value));
    _debugComm.print("    Uptime: ");
    _debugComm.println(isCommandAvailable(DiagnosticCommands::UPTIME::value));
    _debugComm.print("    Fuel level: ");
    _debugComm.println(
        isCommandAvailable(DiagnosticCommands::FUEL_LEVEL::value));
    _debugComm.print("    Absolute load: ");
    _debugComm.println(
        isCommandAvailable(DiagnosticCommands::ABSOLUTE_LOAD::value));
    _debugComm.print("    Relative throttle position: ");
    _debugComm.println(isCommandAvailable(
        DiagnosticCommands::RELATIVE_THROTTLE_POSITION::value));
    _debugComm.print("    Engine fuel rate: ");
    _debugComm.println(
        isCommandAvailable(DiagnosticCommands::ENGINE_FUEL_RATE::value));
    _debugComm.println();
  }

  DataFrame getDataFrame() {
    return DataFrame{-1,
                     get<DiagnosticCommands::ENGINE_RPM>(),
                     get<DiagnosticCommands::VEHICLE_SPEED>(),
                     -1,
                     -1,
                     -1,
                     -1,
                     -1,
                     -1};
  }

  bool sniff() {
    byte header;
    do {
      header = _comm.read();
      if (header != 0) {
        _debugComm.print("Header: ");
        _debugComm.println(header);
      }
    } while (!((header >= 0x80 && header <= 0x8f) ||
               (header >= 0xc0 && header <= 0xcf)));

    byte dataLength = header % 0x10;

    const byte receiver = _comm.read();

    const byte sender = _comm.read();

    std::vector<byte> data(dataLength);
    for (int i = 0; i < dataLength; i++) {
      data[i] = _comm.read();
    }

    const byte checksum = _comm.read();

    // if (header >= 0x80 && header <= 0x8f) {
    //   _debugComm.println("Echo read");
    //   return true;
    // }

    _debugComm.println("Frame received:");
    _debugComm.print("    Header: ");
    _debugComm.println(header);
    _debugComm.print("    Data length: ");
    _debugComm.println(dataLength);
    _debugComm.print("    Receiver: ");
    _debugComm.println(receiver);
    _debugComm.print("    Sender: ");
    _debugComm.println(sender);
    _debugComm.print("    Data: ");
    for (int i = 0; i < dataLength; i++) {
      _debugComm.print(data[i]);
      _debugComm.print(" ");
    }
    _debugComm.println();
    _debugComm.print("    Checksum: ");
    _debugComm.println(checksum);

    return false;
  }

  std::vector<byte> request(CommandLiteral command) {

    std::vector<byte> packet{0xc2, 0x33, 0xf1, command.mode, command.pid, 0x00};
    packet.back() = calculateChecksum(packet);
    _comm.write(packet);

    _debugComm.println("Request sent:");
    for (uint i = 0; i < packet.size(); i++) {
      _debugComm.print(packet[i]);
      _debugComm.print(" ");
    }
    _debugComm.println();

    byte header;
    do {
      header = _comm.read();
      if (header != 0) {
        _debugComm.print("Header: ");
        _debugComm.println(header);
      }
    } while (!((header >= 0x80 && header <= 0x8f) ||
               (header >= 0xc0 && header <= 0xcf)));

    byte dataLength = header % 0x10;

    const byte receiver = _comm.read();

    const byte sender = _comm.read();

    std::vector<byte> data(dataLength);
    for (int i = 0; i < dataLength; i++) {
      data[i] = _comm.read();
    }

    const byte checksum = _comm.read();

    _debugComm.println("Frame received:");
    _debugComm.print("    Header: ");
    _debugComm.println(header);
    _debugComm.print("    Data length: ");
    _debugComm.println(dataLength);
    _debugComm.print("    Receiver: ");
    _debugComm.println(receiver);
    _debugComm.print("    Sender: ");
    _debugComm.println(sender);
    _debugComm.print("    Data: ");
    for (int i = 0; i < dataLength; i++) {

      _debugComm.print(data[i]);
      _debugComm.print(" ");
    }
    _debugComm.println();
    _debugComm.print("    Checksum: ");
    _debugComm.println(checksum);

    return data;
  }

  bool isCommandAvailable(CommandLiteral command) const {
    return _availableCommands.contains(command);
  }

private:
  template <DiagnosticCommands::CommandAvailability TCommand>
  void loadAvailabilityForCommand() {
    if (!isCommandAvailable(TCommand::value)) {
      return;
    }

    const auto availability = get<TCommand>();

    // static_assert(
    //     std::is_same_v<
    //         DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::ParsingFormula::
    //             ValueType,
    //         decltype(get<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>())>);

    for (const auto [key, value] : availability) {
      // static_assert(std::is_same_v<decltype(key), CommandLiteral>);
      _availableCommands[key] = value;
    }
  }

  ICommunicator &_comm;
  ICommunicator &_debugComm;
  std::map<CommandLiteral, bool> _availableCommands;
};
#define REQUESTHANDLER_HPP

#endif // REQUEST_HANDLER_HPP
