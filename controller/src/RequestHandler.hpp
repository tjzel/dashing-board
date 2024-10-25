#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP
#include <DataFrame.hpp>
#include <DiagnosticCommands.hpp>
#include <ICommunicator.hpp>
#include <Parser.hpp>
#include <StateReader.hpp>
#include <Utils.hpp>
#include <map>
#include <vector>

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator> class RequestHandler {
public:
  template <DiagnosticCommands::Command TCommand> auto get();
  void loadAvailability();
  void printAvailableCommands() const;
  void printAvailableForDataFrame() const;
  DataFrame getDataFrame();
  void sniff();
  bool initializeCommunication();
  std::vector<Byte> request(CommandLiteral command);
  [[nodiscard]] bool isCommandAvailable(CommandLiteral command) const;
  [[nodiscard]] bool isCommunicationInitialized() const;
  explicit RequestHandler(TCommunicator &comm, TDebugCommunicator &debugComm);

private:
  template <DiagnosticCommands::CommandAvailability TCommand> void loadAvailabilityForCommand();

  // TODO: Fix references here.
  TCommunicator &_comm;
  TDebugCommunicator &_debugComm;
  // TODO: These conditions are far from ideal.
  StateReader _stateReader{// [](Byte byte) { return (byte & REQUEST_HEADER_MODE_MASK) == 0xc0; },
                           [](const Byte) { return true; }, [](const Byte) { return true; },
                           // [](const Byte byte) { return byte == 0xf1; },
                           [](const Byte) { return true; },
                           // TODO: Fix this with a proper timestamp provider injection.
                           // millis,
                           []() { return 0; }};
  std::map<CommandLiteral, bool> _availableCommands{{DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::value, true},
                                                    {DiagnosticCommands::ENGINE_RPM::value, true},
                                                    {DiagnosticCommands::VEHICLE_SPEED::value, true}};
  bool _communicationInitialized{false};
};

/* #region Implementation */

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
template <DiagnosticCommands::Command TCommand>
auto RequestHandler<TCommunicator, TDebugCommunicator>::get() {
  const auto command = TCommand::value;
  const auto response = request(command);
  const auto result = Parser<TCommand>::parse(response);
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void RequestHandler<TCommunicator, TDebugCommunicator>::loadAvailability() {
  _debugComm.println("Loading x001f");
  loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>();
  // Something is off for the rest of those...
  // Probably asking ECU too fast.
  // _debugComm.println("Loading x203f");
  // loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_20_3F>();
  // _debugComm.println("Loading x405f");
  // loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_40_5F>();
  // _debugComm.println("Loading x607f");
  // loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_60_7F>();
  // _debugComm.println("Loading x809f");
  // loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_80_9F>();
  // _debugComm.println("Loading xa0bf");
  // loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_A0_BF>();
  // _debugComm.println("Loading xc0df");
  // loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_C0_DF>();
  _debugComm.println("Done loading availability");
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void RequestHandler<TCommunicator, TDebugCommunicator>::printAvailableCommands() const {
  _debugComm.println("Available commands:");
  for (const auto &[key, isAvailable] : _availableCommands) {
    _debugComm.println("    " + static_cast<std::string>(key) + " : " + (isAvailable ? "1" : "0"));
  }
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void RequestHandler<TCommunicator, TDebugCommunicator>::printAvailableForDataFrame() const {
  _debugComm.println("Available for data frame:");
  _debugComm.print("    Engine load: ");
  _debugComm.println(isCommandAvailable(DiagnosticCommands::ENGINE_LOAD::value));
  _debugComm.print("    Engine RPM: ");
  _debugComm.println(isCommandAvailable(DiagnosticCommands::ENGINE_RPM::value));
  _debugComm.print("    Vehicle speed: ");
  _debugComm.println(isCommandAvailable(DiagnosticCommands::VEHICLE_SPEED::value));
  _debugComm.print("    Throttle position: ");
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

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
DataFrame RequestHandler<TCommunicator, TDebugCommunicator>::getDataFrame() {
  return DataFrame{-1, get<DiagnosticCommands::ENGINE_RPM>(),
                   // get<DiagnosticCommands::VEHICLE_SPEED>()
                   -1, -1, -1, -1, -1, -1, -1};
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void RequestHandler<TCommunicator, TDebugCommunicator>::sniff() {
  while (!_stateReader.feed(_comm.read())) {
    ;
  }
  auto message = _stateReader.getMessage();
  printMessage(message, _debugComm);
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
bool RequestHandler<TCommunicator, TDebugCommunicator>::initializeCommunication() {
  if (_communicationInitialized) {
    return true;
  }
  _comm.fastInit();
  _debugComm.println("Fast init done");
  // TODO: Fix magic numbers.
  Message message{0xc1, 0x33, 0xf1, {0x81}};
  _comm.write(std::vector<Byte>{message});
  const auto maxAttempts = 32;
  int attempts = 0;
  while (!_stateReader.feed(_comm.read()) && attempts < maxAttempts) {
    attempts++;
  };

  if (attempts >= maxAttempts) {
    _debugComm.println("Communication initialization failed");
    return false;
  };
  // while (!_stateReader.feed(_comm.read())) {
  //   ;
  // }
  // OG Car response:
  //   Format: 83
  //   Target: F1
  //   Source: 11
  //   Data: C1 E9 8F
  //   Checksum: BE
  auto response = _stateReader.getMessage();
  if (response.dataSize() < 1 || response.data[0] != 0xc1) {
    _debugComm.println("Invalid init response received:");
    printMessage(response, _debugComm);
    return false;
  }
  _debugComm.println("Communication initialized");
  _communicationInitialized = true;

  /* #region Timing */
  // TODO: Remove it.
  // Temporary. Wait for the ECU to be ready.
  // delay(50);

  _debugComm.println("Requesting timing data");

  Message timingCurrentRequest{0xc2, 0x33, 0xf1, {0x02}};

  _comm.write(std::vector<Byte>{timingCurrentRequest});

  while (!_stateReader.feed(_comm.read())) {
    ;
  }
  auto timingCurrentResponse = _stateReader.getMessage();

  _debugComm.println("Timing data:");

  printMessage(timingCurrentResponse, _debugComm);

  // delay(50);

  Message timingLimitsRequest{0xc2, 0x33, 0xf1, {0x00}};

  _comm.write(std::vector<Byte>{timingLimitsRequest});

  while (!_stateReader.feed(_comm.read())) {
    ;
  }
  auto timingLimitsResponse = _stateReader.getMessage();
  printMessage(timingLimitsResponse, _debugComm);

  // OG Car response:
  // Format: 83
  // Target: F1
  // Source: 11
  // Data: 7F 2 12
  // Checksum: 18
  // Format: 83
  // Target: F1
  // Source: 11
  // Data: 7F 0 11
  // Checksum: 15

  // delay(50);

  /* #endregion Timing */

  return true;
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
std::vector<Byte> RequestHandler<TCommunicator, TDebugCommunicator>::request(CommandLiteral command) {
  // TODO: Fix magic numbers.
  Message message{0xc2, 0x33, 0xf1, {command.mode, command.pid}};
  _comm.write(std::vector<Byte>{message});

  _debugComm.println("Request sent:");
  printMessage(message, _debugComm);

  while (!_stateReader.feed(_comm.read())) {
    ;
  }
  auto response = _stateReader.getMessage();
  _debugComm.println("Response received:");
  printMessage(response, _debugComm);

  return response.data;
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
bool RequestHandler<TCommunicator, TDebugCommunicator>::isCommandAvailable(CommandLiteral command) const {
  return _availableCommands.contains(command);
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
bool RequestHandler<TCommunicator, TDebugCommunicator>::isCommunicationInitialized() const {
  return _communicationInitialized;
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
RequestHandler<TCommunicator, TDebugCommunicator>::RequestHandler(TCommunicator &comm, TDebugCommunicator &debugComm)
    : _comm(comm), _debugComm(debugComm) {}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
template <DiagnosticCommands::CommandAvailability TCommand>
void RequestHandler<TCommunicator, TDebugCommunicator>::loadAvailabilityForCommand() {
  if (!isCommandAvailable(TCommand::value)) {
    return;
  }

  const auto availability = get<TCommand>();

  for (const auto [key, value] : availability) {
    _availableCommands[key] = value;
  }
}

/* #endregion Implementation */

#endif // REQUEST_HANDLER_HPP
