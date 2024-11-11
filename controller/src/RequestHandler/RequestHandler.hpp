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
  template <DiagnosticCommands::Command TCommand>
  auto get() -> typename TCommand::Encoding::ValueType;
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

  TCommunicator &comm_;
  TDebugCommunicator &debugComm_;
  StateReader _stateReader;
  std::map<CommandLiteral, bool> availableCommands_;
  bool isCommunicationInitialized_{};
};

/* #region Implementation */

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
template <DiagnosticCommands::Command TCommand>
auto RequestHandler<TCommunicator, TDebugCommunicator>::get() ->
    typename TCommand::Encoding::ValueType {

  const auto command = TCommand::value;
  if (!availableCommands_.contains(command)) {
    if constexpr (!std::is_assignable_v<typename TCommand::Encoding::ValueType, int>) {
      return {};
    } else {
      return -1;
    }
  }
  const auto response = request(command);
  const auto result = DiagnosticCodec<TCommand>::decode(response);
  return result;
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void RequestHandler<TCommunicator, TDebugCommunicator>::loadAvailability() {
  // debugComm_.println("Loading x001f");
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
  // debugComm_.println("Done loading availability");
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void RequestHandler<TCommunicator, TDebugCommunicator>::printAvailableCommands() const {
  debugComm_.println("Available commands:");
  for (const auto &[key, isAvailable] : availableCommands_) {
    debugComm_.println("    " + static_cast<std::string>(key) + " : " + (isAvailable ? "1" : "0"));
  }
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void RequestHandler<TCommunicator, TDebugCommunicator>::printAvailableForDataFrame() const {
  debugComm_.println("Available for data frame:");
  debugComm_.print("    Engine load: ");
  debugComm_.println(isCommandAvailable(DiagnosticCommands::ENGINE_LOAD::value));
  debugComm_.print("    Engine RPM: ");
  debugComm_.println(isCommandAvailable(DiagnosticCommands::ENGINE_RPM::value));
  debugComm_.print("    Vehicle speed: ");
  debugComm_.println(isCommandAvailable(DiagnosticCommands::VEHICLE_SPEED::value));
  debugComm_.print("    Throttle position: ");
  debugComm_.println(isCommandAvailable(DiagnosticCommands::THROTTLE_POSITION::value));
  debugComm_.print("    Uptime: ");
  debugComm_.println(isCommandAvailable(DiagnosticCommands::UPTIME::value));
  debugComm_.print("    Fuel level: ");
  debugComm_.println(isCommandAvailable(DiagnosticCommands::FUEL_LEVEL::value));
  debugComm_.print("    Absolute load: ");
  debugComm_.println(isCommandAvailable(DiagnosticCommands::ABSOLUTE_LOAD::value));
  debugComm_.print("    Relative throttle position: ");
  debugComm_.println(isCommandAvailable(DiagnosticCommands::RELATIVE_THROTTLE_POSITION::value));
  debugComm_.print("    Engine fuel rate: ");
  debugComm_.println(isCommandAvailable(DiagnosticCommands::ENGINE_FUEL_RATE::value));
  debugComm_.println();
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
DataFrame RequestHandler<TCommunicator, TDebugCommunicator>::getDataFrame() {
  return DataFrame{.engineLoad = get<DiagnosticCommands::ENGINE_LOAD>(),
                   .engineRPM = get<DiagnosticCommands::ENGINE_RPM>(),
                   .vehicleSpeed = get<DiagnosticCommands::VEHICLE_SPEED>(),
                   .throttlePosition = get<DiagnosticCommands::THROTTLE_POSITION>(),
                   .uptime = get<DiagnosticCommands::UPTIME>(),
                   .fuelLevel = get<DiagnosticCommands::FUEL_LEVEL>(),
                   .absoluteLoad = get<DiagnosticCommands::ABSOLUTE_LOAD>(),
                   .relativeThrottlePosition =
                       get<DiagnosticCommands::RELATIVE_THROTTLE_POSITION>(),
                   .engineFuelRate = get<DiagnosticCommands::ENGINE_FUEL_RATE>()};
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void RequestHandler<TCommunicator, TDebugCommunicator>::sniff() {
  while (!_stateReader.feed(comm_.read())) {
    ;
  }
  auto message = _stateReader.getMessage();
  printMessage(message, debugComm_);
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
bool RequestHandler<TCommunicator, TDebugCommunicator>::initializeCommunication() {
  if (isCommunicationInitialized_) {
    return true;
  }
  comm_.init();
  debugComm_.println("Fast init done");
  // TODO: Fix magic numbers.
  Message message{0xc1, 0x33, 0xf1, {0x81}};
  comm_.write(std::vector<Byte>{message});
  const auto maxAttempts = 32;
  int attempts = 0;
  while (!_stateReader.feed(comm_.read()) && attempts < maxAttempts) {
    attempts++;
  };

  if (attempts >= maxAttempts) {
    debugComm_.println("Communication initialization failed");
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
    debugComm_.println("Invalid init response received:");
    printMessage(response, debugComm_);
    return false;
  }
  debugComm_.println("Communication initialized");
  isCommunicationInitialized_ = true;

  /* #region Timing */
  // TODO: Remove it.
  // Temporary. Wait for the ECU to be ready.
  // delay(50);

  debugComm_.println("Requesting timing data");

  Message timingCurrentRequest{0xc2, 0x33, 0xf1, {0x02}};

  comm_.write(std::vector<Byte>{timingCurrentRequest});

  while (!_stateReader.feed(comm_.read())) {
    ;
  }
  auto timingCurrentResponse = _stateReader.getMessage();

  debugComm_.println("Timing data:");

  printMessage(timingCurrentResponse, debugComm_);

  // delay(50);

  Message timingLimitsRequest{0xc2, 0x33, 0xf1, {0x00}};

  comm_.write(std::vector<Byte>{timingLimitsRequest});

  while (!_stateReader.feed(comm_.read())) {
    ;
  }
  auto timingLimitsResponse = _stateReader.getMessage();
  printMessage(timingLimitsResponse, debugComm_);

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
std::vector<Byte>
RequestHandler<TCommunicator, TDebugCommunicator>::request(CommandLiteral command) {
  // TODO: Fix magic numbers.
  Message message{0xc2, 0x33, 0xf1, {command.mode, command.pid}};
  comm_.write(std::vector<Byte>{message});

  // debugComm_.println("Request sent:");
  // printMessage(message, debugComm_);

  while (!_stateReader.feed(comm_.read())) {
    ;
  }
  auto response = _stateReader.getMessage();
  // debugComm_.println("Response received:");
  // printMessage(response, debugComm_);

  return response.data;
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
bool RequestHandler<TCommunicator, TDebugCommunicator>::isCommandAvailable(
    CommandLiteral command) const {
  return availableCommands_.contains(command);
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
bool RequestHandler<TCommunicator, TDebugCommunicator>::isCommunicationInitialized() const {
  return isCommunicationInitialized_;
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
RequestHandler<TCommunicator, TDebugCommunicator>::RequestHandler(TCommunicator &comm,
                                                                  TDebugCommunicator &debugComm)
    : comm_(comm), debugComm_(debugComm),
      // TODO: Fix conditions here.
      _stateReader({// [](Byte byte) { return (byte & REQUEST_HEADER_MODE_MASK) == 0xc0; },
                    .isHeaderValid = [](const Byte) { return true; },
                    .isTargetValid = [](const Byte) { return true; },
                    // [](const Byte byte) { return byte == 0xf1; },
                    .isSourceValid = [](const Byte) { return true; },
                    // TODO: Fix this with a proper timestamp provider injection.
                    // millis,
                    .getTimestamp = []() { return 0; }}),
      availableCommands_({{DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::value, true}}) {}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
template <DiagnosticCommands::CommandAvailability TCommand>
void RequestHandler<TCommunicator, TDebugCommunicator>::loadAvailabilityForCommand() {
  if (!isCommandAvailable(TCommand::value)) {
    return;
  }

  const auto availability = get<TCommand>();

  for (const auto [key, value] : availability) {
    availableCommands_[key] = value;
  }
}

/* #endregion Implementation */

#endif // REQUEST_HANDLER_HPP
