#ifndef ECU_RESPONDER_HPP
#define ECU_RESPONDER_HPP

#include <DataProvider.hpp>
#include <DiagnosticCommands.hpp>
#include <Message.hpp>
#include <Parser.hpp>
#include <cassert>
#include <type_traits>

struct EcuInternalResponse {
  bool hasResponse{};
  Message response;
};

class EcuResponder {
public:
  EcuInternalResponse request(Message &message);
  EcuInternalResponse request(OBD2Message &message);

private:
  template <DiagnosticCommands::Command TCommand> Message respondTo(OBD2Message &message);

  // TODO: Require fast init as well.
  static Message respondToInit(Message &message);

  static Message createResponse(Byte target, Byte mode, Byte pid, const std::vector<Byte> &data);

  static Message createResponse(Byte target, const std::vector<Byte> &data);

  template <DiagnosticCommands::Command TCommand> std::vector<Byte> getFromProvider() {
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>) {
      // 0x18 - only RPM and SPEED available.
      // TODO: Use encode from a map.
      return std::vector<Byte>{0x00, 0x18, 0x00, 0x00};
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::ENGINE_RPM>) {
      return DiagnosticCodec<DiagnosticCommands::ENGINE_RPM>::encode(rpmProvider_.get());
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::VEHICLE_SPEED>) {
      return DiagnosticCodec<DiagnosticCommands::VEHICLE_SPEED>::encode(speedProvider_.get());
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::ENGINE_LOAD>) {
      return DiagnosticCodec<DiagnosticCommands::ENGINE_LOAD>::encode(engineLoadProvider_.get());
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::THROTTLE_POSITION>) {
      return DiagnosticCodec<DiagnosticCommands::THROTTLE_POSITION>::encode(
          throttlePositionProvider_.get());
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::UPTIME>) {
      return DiagnosticCodec<DiagnosticCommands::UPTIME>::encode(uptimeProvider_.get());
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::FUEL_LEVEL>) {
      return DiagnosticCodec<DiagnosticCommands::FUEL_LEVEL>::encode(fuelLevelProvider_.get());
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::ABSOLUTE_LOAD>) {
      return DiagnosticCodec<DiagnosticCommands::ABSOLUTE_LOAD>::encode(
          absoluteLoadProvider_.get());
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::RELATIVE_THROTTLE_POSITION>) {
      return DiagnosticCodec<DiagnosticCommands::RELATIVE_THROTTLE_POSITION>::encode(
          relativeThrottlePositionProvider_.get());
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::ENGINE_FUEL_RATE>) {
      return DiagnosticCodec<DiagnosticCommands::ENGINE_FUEL_RATE>::encode(
          engineFuelRateProvider_.get());
    }
    throw std::runtime_error("Not implemented");
  }

  // TODO: Don't hardcode it here.
  DataProvider rpmProvider_{800, 100, 1, 800, 4000};
  DataProvider speedProvider_{10, 1, 1, 10, 100};
  DataProvider engineLoadProvider_{15, 2, 1, 0, 100};
  DataProvider throttlePositionProvider_{10, 5, 1, 0, 100};
  DataProvider uptimeProvider_{15, 1, 1, 5, 60};
  DataProvider fuelLevelProvider_{80, 2, -1, 5, 100};
  DataProvider absoluteLoadProvider_{10, 1, 1, 0, 20};
  DataProvider relativeThrottlePositionProvider_{20, 5, 1, 0, 90};
  DataProvider engineFuelRateProvider_{1200, 50, 1, 100, 2500};
};

/* #region Implementation */

template <DiagnosticCommands::Command TCommand>
Message EcuResponder::respondTo(OBD2Message &message) {
  const Byte target = message.source();
  const Byte mode = DiagnosticCommands::RESPONSE_MODE;
  const Byte pid = TCommand::pid;
  assert(pid == message.pid());
  const auto data = getFromProvider<TCommand>();
  assert(data.size() == TCommand::Encoding::byteCount);
  return createResponse(target, mode, pid, data);
}

/* #endregion Implementation */

#endif // ECU_RESPONDER_HPP
