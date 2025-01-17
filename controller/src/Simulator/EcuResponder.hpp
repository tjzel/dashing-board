#ifndef ECU_RESPONDER_HPP
#define ECU_RESPONDER_HPP

#include <DataProvider.hpp>
#include <DataSources.hpp>
#include <DiagnosticCommands.hpp>
#include <Message.hpp>
#include <Parser.hpp>
#include <cassert>
#include <type_traits>

struct EcuInternalResponse {
  bool hasResponse{};
  Message response;
};

template <IDataProviderManager TDataProviderManager> class EcuResponder {
public:
  EcuInternalResponse request(Message &message);
  EcuInternalResponse request(OBD2Message &message);
  explicit EcuResponder(TDataProviderManager &dataSource);

private:
  template <DiagnosticCommands::Command TCommand> Message respondTo(OBD2Message &message);

  // TODO: Require fast init as well.
  static Message respondToInit(Message &message);

  static Message createResponse(Byte target, Byte mode, Byte pid, const std::vector<Byte> &data);

  static Message createResponse(Byte target, const std::vector<Byte> &data);

  TDataProviderManager dataProviderManager_;

  std::map<CommandLiteral, bool> availability{
      {{DiagnosticCommands::ENGINE_RPM::value, true},
       {DiagnosticCommands::VEHICLE_SPEED::value, true},
       {DiagnosticCommands::ENGINE_LOAD::value, true},
       {DiagnosticCommands::THROTTLE_POSITION::value, true},
       {DiagnosticCommands::UPTIME::value, true},
       {DiagnosticCommands::FUEL_LEVEL::value, true},
       {DiagnosticCommands::ABSOLUTE_LOAD::value, true},
       {DiagnosticCommands::RELATIVE_THROTTLE_POSITION::value, true},
       {DiagnosticCommands::ENGINE_FUEL_RATE::value, true},
       {DiagnosticCommands::COMMAND_AVAILABILITY_20_3F::value, true},
       {DiagnosticCommands::COMMAND_AVAILABILITY_40_5F::value, true},
       {DiagnosticCommands::COMMAND_AVAILABILITY_60_7F::value, true},
       {DiagnosticCommands::COMMAND_AVAILABILITY_80_9F::value, true},
       {DiagnosticCommands::COMMAND_AVAILABILITY_A0_BF::value, true},
       {DiagnosticCommands::COMMAND_AVAILABILITY_C0_DF::value, true}}};

  template <DiagnosticCommands::Command TCommand> std::vector<Byte> getFromProvider() {

    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>) {
      return DiagnosticCodec<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>::encode(availability);
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::COMMAND_AVAILABILITY_20_3F>) {
      return DiagnosticCodec<DiagnosticCommands::COMMAND_AVAILABILITY_20_3F>::encode(availability);
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::COMMAND_AVAILABILITY_40_5F>) {
      return DiagnosticCodec<DiagnosticCommands::COMMAND_AVAILABILITY_40_5F>::encode(availability);
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::COMMAND_AVAILABILITY_60_7F>) {
      return DiagnosticCodec<DiagnosticCommands::COMMAND_AVAILABILITY_60_7F>::encode(availability);
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::COMMAND_AVAILABILITY_80_9F>) {
      return DiagnosticCodec<DiagnosticCommands::COMMAND_AVAILABILITY_80_9F>::encode(availability);
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::COMMAND_AVAILABILITY_A0_BF>) {
      return DiagnosticCodec<DiagnosticCommands::COMMAND_AVAILABILITY_A0_BF>::encode(availability);
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::COMMAND_AVAILABILITY_C0_DF>) {
      return DiagnosticCodec<DiagnosticCommands::COMMAND_AVAILABILITY_C0_DF>::encode(availability);
    }

    return DiagnosticCodec<TCommand>::encode(dataProviderManager_.template get<TCommand>());

    // throw std::runtime_error("Not implemented");
  }

  // TODO: Don't hardcode it here.
  DataProvider rpmProvider_{800, 100, 1, 800, 4000};
  DataProvider speedProvider_{10, 1, 1, 10, 140};
  DataProvider engineLoadProvider_{15, 2, 1, 0, 100};
  DataProvider throttlePositionProvider_{10, 5, 1, 0, 100};
  DataProvider uptimeProvider_{15, 1, 1, 5, 60};
  DataProvider fuelLevelProvider_{80, 2, -1, 5, 100};
  DataProvider absoluteLoadProvider_{10, 1, 1, 0, 20};
  DataProvider relativeThrottlePositionProvider_{20, 5, 1, 0, 90};
  DataProvider engineFuelRateProvider_{1200, 50, 1, 100, 2500};
};

/* #region Implementation */

template <IDataProviderManager TDataProviderManager>
template <DiagnosticCommands::Command TCommand>
Message EcuResponder<TDataProviderManager>::respondTo(OBD2Message &message) {
  const Byte target = message.source();
  const Byte mode = DiagnosticCommands::RESPONSE_MODE;
  const Byte pid = TCommand::pid;
  assert(pid == message.pid());
  const auto data = getFromProvider<TCommand>();
  assert(data.size() == TCommand::Encoding::byteCount);
  return createResponse(target, mode, pid, data);
}

template <IDataProviderManager TDataProviderManager>
EcuResponder<TDataProviderManager>::EcuResponder(TDataProviderManager &dataSource)
    : dataProviderManager_({dataSource}) {}

template <IDataProviderManager TDataProviderManager>
EcuInternalResponse EcuResponder<TDataProviderManager>::request(Message &message) {
  if (message.isInitMessage()) {
    return {.hasResponse = true, .response = respondToInit(message)};
  }
  // TODO: Use mock message instead.
  return {.hasResponse = false, .response = {0x00, 0x00, 0x00, {}}};
}

template <IDataProviderManager TDataProviderManager>
EcuInternalResponse EcuResponder<TDataProviderManager>::request(OBD2Message &message) {
  if (message.command() == DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::value) {
    return {.hasResponse = true,
            .response = respondTo<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>(message)};
  }
  if (message.command() == DiagnosticCommands::COMMAND_AVAILABILITY_20_3F::value) {
    return {.hasResponse = true,
            .response = respondTo<DiagnosticCommands::COMMAND_AVAILABILITY_20_3F>(message)};
  }
  if (message.command() == DiagnosticCommands::COMMAND_AVAILABILITY_40_5F::value) {
    return {.hasResponse = true,
            .response = respondTo<DiagnosticCommands::COMMAND_AVAILABILITY_40_5F>(message)};
  }
  if (message.command() == DiagnosticCommands::COMMAND_AVAILABILITY_60_7F::value) {
    return {.hasResponse = true,
            .response = respondTo<DiagnosticCommands::COMMAND_AVAILABILITY_60_7F>(message)};
  }
  if (message.command() == DiagnosticCommands::COMMAND_AVAILABILITY_80_9F::value) {
    return {.hasResponse = true,
            .response = respondTo<DiagnosticCommands::COMMAND_AVAILABILITY_80_9F>(message)};
  }
  if (message.command() == DiagnosticCommands::COMMAND_AVAILABILITY_A0_BF::value) {
    return {.hasResponse = true,
            .response = respondTo<DiagnosticCommands::COMMAND_AVAILABILITY_A0_BF>(message)};
  }
  if (message.command() == DiagnosticCommands::COMMAND_AVAILABILITY_C0_DF::value) {
    return {.hasResponse = true,
            .response = respondTo<DiagnosticCommands::COMMAND_AVAILABILITY_C0_DF>(message)};
  }
  if (message.command() == DiagnosticCommands::ENGINE_RPM::value) {
    return {.hasResponse = true, .response = respondTo<DiagnosticCommands::ENGINE_RPM>(message)};
  }
  if (message.command() == DiagnosticCommands::VEHICLE_SPEED::value) {
    return {.hasResponse = true, .response = respondTo<DiagnosticCommands::VEHICLE_SPEED>(message)};
  }
  if (message.command() == DiagnosticCommands::ENGINE_LOAD::value) {
    return {.hasResponse = true, .response = respondTo<DiagnosticCommands::ENGINE_LOAD>(message)};
  }
  if (message.command() == DiagnosticCommands::THROTTLE_POSITION::value) {
    return {.hasResponse = true,
            .response = respondTo<DiagnosticCommands::THROTTLE_POSITION>(message)};
  }
  if (message.command() == DiagnosticCommands::UPTIME::value) {
    return {.hasResponse = true, .response = respondTo<DiagnosticCommands::UPTIME>(message)};
  }
  if (message.command() == DiagnosticCommands::FUEL_LEVEL::value) {
    return {.hasResponse = true, .response = respondTo<DiagnosticCommands::FUEL_LEVEL>(message)};
  }
  if (message.command() == DiagnosticCommands::ABSOLUTE_LOAD::value) {
    return {.hasResponse = true, .response = respondTo<DiagnosticCommands::ABSOLUTE_LOAD>(message)};
  }
  if (message.command() == DiagnosticCommands::RELATIVE_THROTTLE_POSITION::value) {
    return {.hasResponse = true,
            .response = respondTo<DiagnosticCommands::RELATIVE_THROTTLE_POSITION>(message)};
  }
  if (message.command() == DiagnosticCommands::ENGINE_FUEL_RATE::value) {
    return {.hasResponse = true,
            .response = respondTo<DiagnosticCommands::ENGINE_FUEL_RATE>(message)};
  }

  // TODO: Use mock message instead.
  return {.hasResponse = false, .response = {0x00, 0x00, 0x00, {}}};
}

template <IDataProviderManager TDataProviderManager>
Message EcuResponder<TDataProviderManager>::respondToInit(Message &message) {
  // TODO: Don't hardcode it here.
  // 0x83 0xf1 0x11 0xc1 0x8f 0xef 0xc4
  const std::vector<Byte> initOkData{0xc1, 0x8f, 0xef};
  return createResponse(message.source, initOkData);
}

template <IDataProviderManager TDataProviderManager>
Message EcuResponder<TDataProviderManager>::createResponse(const Byte target, const Byte mode,
                                                           const Byte pid,
                                                           const std::vector<Byte> &obd2Data) {
  const Byte modeAndPidSize = 2;
  const Byte size = obd2Data.size() + modeAndPidSize;
  const Byte format = PHYSICAL_ADDRESSING | (size & LENGTH_MASK);
  const Byte source = ECU_ADDRESS;

  std::vector<Byte> data{mode, pid};
  data.insert(data.end(), obd2Data.begin(), obd2Data.end());
  assert(data.size() == size);

  return {format, target, source, data};
}

template <IDataProviderManager TDataProviderManager>
Message EcuResponder<TDataProviderManager>::createResponse(const Byte target,
                                                           const std::vector<Byte> &data) {
  const Byte size = data.size();
  const Byte format = PHYSICAL_ADDRESSING | (size & LENGTH_MASK);
  const Byte source = ECU_ADDRESS;

  return {format, target, source, data};
}

/* #endregion Implementation */

#endif // ECU_RESPONDER_HPP
