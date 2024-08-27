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
    return DataFrame{
        -1, get<DiagnosticCommands::ENGINE_RPM>(), -1, -1, -1, -1, -1, -1,
        -1
        // get<DiagnosticCommands::ENGINE_LOAD>(8),
        // get<DiagnosticCommands::ENGINE_RPM>(),
        // get<DiagnosticCommands::VEHICLE_SPEED>(),
        // get<DiagnosticCommands::THROTTLE_POSITION>(),
        // get<DiagnosticCommands::UPTIME>(),
        // get<DiagnosticCommands::FUEL_LEVEL>(),
        // get<DiagnosticCommands::ABSOLUTE_LOAD>(),
        // get<DiagnosticCommands::RELATIVE_THROTTLE_POSITION>(),
        // get<DiagnosticCommands::ENGINE_FUEL_RATE>(),
    };
  }

  bool sniff() {
    uint8_t header;
    do {
      while (_comm.available() == 0)
        ;
      header = _comm.read();
      if (header != 0) {
        _debugComm.print("Header: ");
        _debugComm.println(header);
      }
    } while (!((header >= 0x80 && header <= 0x8f) ||
               (header >= 0xc0 && header <= 0xcf)));

    uint8_t dataLength = header % 0x10;
    while (_comm.available() == 0)
      ;
    const uint8_t receiver = _comm.read();
    while (_comm.available() == 0)
      ;
    const uint8_t sender = _comm.read();

    uint8_t data[dataLength];
    for (int i = 0; i < dataLength; i++) {
      while (_comm.available() == 0)
        ;
      data[i] = _comm.read();
    }

    while (_comm.available() == 0)
      ;
    const uint8_t checksum = _comm.read();

    if (header >= 0x80 && header <= 0x8f) {
      _debugComm.println("Echo read");
      return true;
    }

    _debugComm.println("Frame received:");
    _debugComm.print("    Header: ");
    _debugComm.writeByteLn(header);
    _debugComm.print("    Data length: ");
    _debugComm.writeByteLn(dataLength);
    _debugComm.print("    Receiver: ");
    _debugComm.writeByteLn(receiver);
    _debugComm.print("    Sender: ");
    _debugComm.writeByteLn(sender);
    // _debugComm.print("    Mode: ");
    // _debugComm.println(mode, HEX);
    // _debugComm.print("    PID: ");
    // _debugComm.println(PID, HEX);
    _debugComm.print("    Data: ");
    for (int i = 0; i < dataLength; i++) {
      _debugComm.writeByteLn(data[i]);
      _debugComm.print(" ");
    }
    _debugComm.println();
    _debugComm.print("    Checksum: ");
    _debugComm.writeByteLn(checksum);

    // C1 33 F1 81 66
    // if (header == 0xc1 && receiver == 0x33 && sender == 0xf1 &&
    //     dataLength > 0 && data[0] == 0x81 && checksum == 0x66) {
    //   // 0x83 0xF1 0x11 0xC1 0x8F 0xEF 0xC4
    //   _comm.write(0x83);
    //   _comm.write(0xF1);
    //   _comm.write(0x11);
    //   _comm.write(0xC1);
    //   _comm.write(0x8F);
    //   _comm.write(0xEF);
    //   _comm.write(0xC4);
    //   _debugComm.println("ACK sent");
    //   return true;
    // }

    return false;
  }

  std::vector<uint8_t> request(CommandLiteral command) {

    std::vector<uint8_t> packet{0xc2,         0x33,        0xf1,
                                command.mode, command.pid, 0x00};
    packet.back() = calculateChecksum(packet);
    _comm.write(packet);

    _debugComm.println("Request sent:");
    for (uint i = 0; i < packet.size(); i++) {
      _debugComm.print(packet[i]);
      _debugComm.print(" ");
    }
    _debugComm.println();

    uint8_t header;
    do {
      header = _comm.read();
      if (header != 0) {
        _debugComm.print("Header: ");
        _debugComm.println(header);
      }
    } while (!((header >= 0x80 && header <= 0x8f) ||
               (header >= 0xc0 && header <= 0xcf)));

    uint8_t dataLength = header % 0x10;

    const uint8_t receiver = _comm.read();

    const uint8_t sender = _comm.read();

    std::vector<uint8_t> data(dataLength);
    for (int i = 0; i < dataLength; i++) {
      data[i] = _comm.read();
    }

    const uint8_t checksum = _comm.read();

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
