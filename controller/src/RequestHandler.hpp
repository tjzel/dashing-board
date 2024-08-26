#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP
#include <DataFrame.hpp>
#include <DiagnosticCommands.hpp>
#include <Parser.hpp>
#include <map>
#include <string>
#include <vector>

template <class ICommunication>
concept CommunicationInterface =
    requires(ICommunication comm, const uint8_t byte,
             const std::vector<uint8_t> message, const std::string str) {
      { comm.readByte() } -> std::convertible_to<uint8_t>;
      { comm.write(message) } -> std::same_as<void>;
      { comm.writeLn(str) } -> std::same_as<void>;
      { comm.writeByte(byte) } -> std::same_as<void>;
      { comm.writeByteLn(byte) } -> std::same_as<void>;
      { comm.print(str) } -> std::same_as<void>;
      { comm.println(str) } -> std::same_as<void>;
    };

template <CommunicationInterface ICommunication> class RequestHandler {
public:
  explicit RequestHandler(ICommunication &comm, ICommunication &debugComm)
      : _comm(comm), _debugComm(debugComm) {
    for (auto &command : DiagnosticCommands::DEFAULT_AVAILABILITY) {
      _availableCommands[command] = true;
    }
  }

  template <DiagnosticCommands::Command TCommand>
  typename TCommand::Type get(const int responseExpected) {
    const auto command = TCommand::value;
    // const auto clock = std::chrono::high_resolution_clock::now();
    auto response = request(command, responseExpected);
    // const auto requestTime = std::chrono::high_resolution_clock::now();
    // const auto requestDuration =
    // std::chrono::duration_cast<std::chrono::milliseconds>(requestTime -
    // clock);
    const auto result = Parser<TCommand>::parse(response);
    // const auto parseTime = std::chrono::high_resolution_clock::now();
    // const auto parseDuration =
    // std::chrono::duration_cast<std::chrono::milliseconds>(parseTime -
    // requestTime); if(requestDuration.count() > 0){ std::cout << "Request
    // took: " << requestDuration.count() << " ms" << std::endl; std::cout <<
    // "Parse took: " << parseDuration.count() << " ms" << std::endl;
    // }

    return result;
    // return Parser::parse<TCommand>(response);
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

  template <DiagnosticCommands::CommandAvailability TCommand>
  void loadAvailabilityForCommand() {
    if (isCommandAvailable(TCommand::value)) {
      return;
    }

    const auto response = get<TCommand>();
    const auto availability = Parser<TCommand>::parse(response);

    for (const auto [key, value] : availability) {
      _availableCommands[key] = value;
    }
  }

  void printAvailableCommands() const {
    _debugComm.println("Available commands:");
    for (const auto &[key, isAvailable] : _availableCommands) {
      _debugComm.print("    ");
      for (int i = 0; i < 4; i++) {
        _debugComm.writeByteLn(key[i]);
      }
      _debugComm.print(": ");
      _debugComm.println(isAvailable);
      _debugComm.println();
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
    if (header == 0xc1 && receiver == 0x33 && sender == 0xf1 &&
        dataLength > 0 && data[0] == 0x81 && checksum == 0x66) {
      // 0x83 0xF1 0x11 0xC1 0x8F 0xEF 0xC4
      _comm.write(0x83);
      _comm.write(0xF1);
      _comm.write(0x11);
      _comm.write(0xC1);
      _comm.write(0x8F);
      _comm.write(0xEF);
      _comm.write(0xC4);
      _debugComm.println("ACK sent");
      return true;
    }

    // if (PID == 0x0c && data.size() >= 2) {
    //     Parser::parse<DiagnosticCommands::ENGINE_RPM>(data);
    //     _debugComm.print("Engine RPM: ");
    //     _debugComm.println(Parser::parse<DiagnosticCommands::ENGINE_RPM>(data));
    //     return true;
    // }
    // else if (PID == 0x0d && data.size() >= 1) {
    //     Parser::parse<DiagnosticCommands::VEHICLE_SPEED>(data);
    //     _debugComm.print("Vehicle speed: ");
    //     _debugComm.println(Parser::parse<DiagnosticCommands::VEHICLE_SPEED>(data));
    //     return true;
    // }

    return false;
  }

  std::vector<uint8_t> request(const uint8_t command[]) {

    const uint8_t packet[] = {0xc2,
                              0x33,
                              0xF1,
                              command[0],
                              command[1],
                              static_cast<uint8_t>(0xc2 + 0x33 + 0xf1 +
                                                   command[0] +
                                                   command[1] % 0x100)};
    _comm.write(packet, sizeof(packet));

    _debugComm.println("Request sent:");
    for (uint i = 0; i < sizeof(packet); i++) {
      _debugComm.writeByteLn(packet[i]);
      _debugComm.print(" ");
    }
    _debugComm.println();

    auto attemptsLeft = 128;

    while (attemptsLeft > 0) {
      while (_comm.available() < 3)
        ;
      const uint8_t header = _comm.read();
      if (!(header >= 0x82 && header <= 0x8f)) {
        --attemptsLeft;
        continue;
      }
      const uint8_t dataLength = header % 0x10 - 2;
      if (dataLength < 2) {
        --attemptsLeft;
        continue;
      }
      const uint8_t receiver = _comm.read();
      if (receiver != 0xf1) {
        --attemptsLeft;
        continue;
      }
      const uint8_t sender = _comm.read();
      // Let's ignore sender for now.

      while (_comm.available() < dataLength)
        ;
      const uint8_t mode = _comm.read();
      if (mode != 0x01) {
        --attemptsLeft;
        continue;
      }
      const uint8_t PID = _comm.read();
      if (PID != 0x0c && PID != 0x0d) {
        --attemptsLeft;
        continue;
      }

      const auto obdDataLength = dataLength - 2;

      std::vector<uint8_t> data(obdDataLength);
      for (int i = 0; i < dataLength; i++) {
        data[i] = _comm.read();
      }

      while (_comm.available() == 0)
        ;
      const uint8_t checksum = _comm.read();

      _debugComm.println("Frame received:");
      _debugComm.print("    Header: ");
      _debugComm.writeByteLn(header);
      _debugComm.print("    Data length: ");
      _debugComm.writeByteLn(dataLength);
      _debugComm.print("    Receiver: ");
      _debugComm.writeByteLn(receiver);
      _debugComm.print("    Sender: ");
      _debugComm.writeByteLn(sender);
      _debugComm.print("    Mode: ");
      _debugComm.writeByteLn(mode);
      _debugComm.print("    PID: ");
      _debugComm.writeByteLn(PID);
      _debugComm.print("    Data: ");
      for (int i = 0; i < dataLength; i++) {
        _debugComm.writeByte(data[i]);
        _debugComm.print(" ");
      }
      _debugComm.println();
      _debugComm.print("    Checksum: ");
      _debugComm.writeByteLn(checksum);
      return data;
    }

    _debugComm.println("Maximum attempts reached");
    return {};
  }

  bool isCommandAvailable(const uint8_t command[]) const {
    return _availableCommands.find(command) != _availableCommands.end() &&
           _availableCommands.at(command);
  }

private:
  ICommunication &_comm;
  ICommunication &_debugComm;
  uint8_t buffer[512];
  std::map<const uint8_t *, bool> _availableCommands;
};
#define REQUESTHANDLER_HPP

#endif // REQUESTHANDLER_HPP
