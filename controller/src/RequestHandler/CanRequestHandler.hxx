#ifndef CAN_REQUEST_HANDLER_HPP
#define CAN_REQUEST_HANDLER_HPP

#include <DataFrame.hxx>
#include <DiagnosticCommands.hxx>
#include <ICanBus.hxx>
#include <ICommunicator.hxx>
#include <Parser.hxx>

#include <map>
#include <span>
#include <vector>

// ISO 15765-4 OBD2 over CAN.
namespace CanObd2 {
// Functional (broadcast) request address.
static constexpr uint32_t REQUEST_ID = 0x7DF;
// First ECU physical response address.
static constexpr uint32_t RESPONSE_ID = 0x7E8;
// Default timeout for a response frame (ms).
static constexpr uint32_t RESPONSE_TIMEOUT_MS = 150;
} // namespace CanObd2

template <ICanBus TCanBus, IDebugCommunicator TDebugCommunicator> class CanRequestHandler {
public:
  template <DiagnosticCommands::Command TCommand>
  auto get() -> typename TCommand::Encoding::ValueType;

  void loadAvailability();
  void printAvailableCommands() const;
  void printAvailableForDataFrame() const;
  DataFrame getDataFrame();
  std::vector<Byte> request(CommandLiteral command);
  [[nodiscard]] bool isCommandAvailable(CommandLiteral command) const;

  explicit CanRequestHandler(TCanBus &bus, TDebugCommunicator &debugComm)
      : bus_(bus), debugComm_(debugComm),
        availableCommands_({{DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::value, true}}) {}

private:
  template <DiagnosticCommands::CommandAvailability TCommand> void loadAvailabilityForCommand();

  TCanBus &bus_;
  TDebugCommunicator &debugComm_;
  std::map<CommandLiteral, bool> availableCommands_;
};

/* #region Implementation */

template <ICanBus TCanBus, IDebugCommunicator TDebugCommunicator>
template <DiagnosticCommands::Command TCommand>
auto CanRequestHandler<TCanBus, TDebugCommunicator>::get() ->
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
  if (response.size() < 2) {
    if constexpr (!std::is_assignable_v<typename TCommand::Encoding::ValueType, int>) {
      return {};
    } else {
      return -1;
    }
  }
  const auto data = std::span(response).subspan(2);
  return DiagnosticCodec<TCommand>::decode(data);
}

template <ICanBus TCanBus, IDebugCommunicator TDebugCommunicator>
void CanRequestHandler<TCanBus, TDebugCommunicator>::loadAvailability() {
  debugComm_.println("Loading x001f");
  loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>();
  debugComm_.println("Loading x203f");
  loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_20_3F>();
  debugComm_.println("Loading x405f");
  loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_40_5F>();
  debugComm_.println("Loading x607f");
  loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_60_7F>();
  debugComm_.println("Loading x809f");
  loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_80_9F>();
  debugComm_.println("Loading xa0bf");
  loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_A0_BF>();
  debugComm_.println("Loading xc0df");
  loadAvailabilityForCommand<DiagnosticCommands::COMMAND_AVAILABILITY_C0_DF>();
  debugComm_.println("Done loading availability");
}

template <ICanBus TCanBus, IDebugCommunicator TDebugCommunicator>
void CanRequestHandler<TCanBus, TDebugCommunicator>::printAvailableCommands() const {
  debugComm_.println("Available commands:");
  for (const auto &[key, isAvailable] : availableCommands_) {
    debugComm_.println("    " + static_cast<std::string>(key) + " : " + (isAvailable ? "1" : "0"));
  }
}

template <ICanBus TCanBus, IDebugCommunicator TDebugCommunicator>
void CanRequestHandler<TCanBus, TDebugCommunicator>::printAvailableForDataFrame() const {
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

template <ICanBus TCanBus, IDebugCommunicator TDebugCommunicator>
DataFrame CanRequestHandler<TCanBus, TDebugCommunicator>::getDataFrame() {
  return DataFrame{
      .engineLoad = get<DiagnosticCommands::ENGINE_LOAD>(),
      .engineRPM = get<DiagnosticCommands::ENGINE_RPM>(),
      .vehicleSpeed = get<DiagnosticCommands::VEHICLE_SPEED>(),
      .throttlePosition = get<DiagnosticCommands::THROTTLE_POSITION>(),
      .uptime = get<DiagnosticCommands::UPTIME>(),
      .fuelLevel = get<DiagnosticCommands::FUEL_LEVEL>(),
      .absoluteLoad = get<DiagnosticCommands::ABSOLUTE_LOAD>(),
      .relativeThrottlePosition = get<DiagnosticCommands::RELATIVE_THROTTLE_POSITION>(),
      .engineFuelRate = get<DiagnosticCommands::ENGINE_FUEL_RATE>(),
  };
}

template <ICanBus TCanBus, IDebugCommunicator TDebugCommunicator>
std::vector<Byte> CanRequestHandler<TCanBus, TDebugCommunicator>::request(CommandLiteral command) {
  // Build single-frame OBD2 request: [PCI=02] [mode] [PID] [pad...]
  CanFrame request{};
  request.id = CanObd2::REQUEST_ID;
  request.dlc = 8;
  request.data[0] = 0x02; // PCI: single frame, 2 data bytes
  request.data[1] = DiagnosticCommands::REQUEST_MODE;
  request.data[2] = command.pid;
  // data[3..7] stay 0 (padding)

  if (!bus_.send(request)) {
    debugComm_.println("CAN send failed");
    return {};
  }

  // Wait for response from ECU (CAN ID 0x7E8).
  CanFrame response = bus_.receiveFrom(CanObd2::RESPONSE_ID, CanObd2::RESPONSE_TIMEOUT_MS);

  if (response.id != CanObd2::RESPONSE_ID) {
    debugComm_.println("No CAN response");
    return {};
  }

  // PCI byte: lower nibble = data length for single frames.
  uint8_t pci = response.data[0];
  uint8_t frameType = (pci >> 4) & 0x0F;

  if (frameType != 0x00) {
    // Multi-frame (ISO-TP) not yet supported.
    debugComm_.println("Multi-frame response not supported");
    return {};
  }

  uint8_t dataLen = pci & 0x0F;
  if (dataLen < 2 || response.data[1] != DiagnosticCommands::RESPONSE_MODE) {
    debugComm_.println("Invalid CAN response");
    return {};
  }

  // Return payload starting from mode byte (matching K-line RequestHandler format).
  // data[1] = response mode (0x41), data[2] = PID, data[3..] = value bytes.
  std::vector<Byte> result(response.data.begin() + 1, response.data.begin() + 1 + dataLen);
  return result;
}

template <ICanBus TCanBus, IDebugCommunicator TDebugCommunicator>
bool CanRequestHandler<TCanBus, TDebugCommunicator>::isCommandAvailable(
    CommandLiteral command) const {
  return availableCommands_.contains(command);
}

template <ICanBus TCanBus, IDebugCommunicator TDebugCommunicator>
template <DiagnosticCommands::CommandAvailability TCommand>
void CanRequestHandler<TCanBus, TDebugCommunicator>::loadAvailabilityForCommand() {
  if (!isCommandAvailable(TCommand::value)) {
    return;
  }
  const auto availability = get<TCommand>();
  for (const auto [key, value] : availability) {
    availableCommands_[key] = value;
  }
}

/* #endregion Implementation */

#endif // CAN_REQUEST_HANDLER_HPP
