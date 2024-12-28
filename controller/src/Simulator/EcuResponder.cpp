#include "DiagnosticCommands.hpp"
#include "ICommunicator.hpp"
#include "Parser.hpp"
#include <EcuResponder.hpp>
#include <cassert>
#include <iostream>

EcuInternalResponse EcuResponder::request(Message &message) {
  if (message.isInitMessage()) {
    return {.hasResponse = true, .response = respondToInit(message)};
  }
  // TODO: Use mock message instead.
  return {.hasResponse = false, .response = {0x00, 0x00, 0x00, {}}};
}

EcuInternalResponse EcuResponder::request(OBD2Message &message) {
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

Message EcuResponder::respondToInit(Message &message) {
  // TODO: Don't hardcode it here.
  // 0x83 0xf1 0x11 0xc1 0x8f 0xef 0xc4
  const std::vector<Byte> initOkData{0xc1, 0x8f, 0xef};
  return createResponse(message.source, initOkData);
}

Message EcuResponder::createResponse(const Byte target, const Byte mode, const Byte pid,
                                     const std::vector<Byte> &obd2Data) {
  // TODO: DRY
  const Byte modeAndPidSize = 2;
  const Byte size = obd2Data.size() + modeAndPidSize;
  const Byte format = RESPONSE_HEADER | size;
  const Byte source = ECU_ADDRESS;

  std::vector<Byte> data{mode, pid};
  data.insert(data.end(), obd2Data.begin(), obd2Data.end());
  assert(data.size() == size);

  return {format, target, source, data};
}

Message EcuResponder::createResponse(const Byte target, const std::vector<Byte> &data) {
  // TODO: DRY
  const Byte modeAndPidSize = 2;
  const Byte size = data.size() + modeAndPidSize;
  const Byte format = RESPONSE_HEADER | size;
  const Byte source = ECU_ADDRESS;

  // TODO: Fix this.
  // assert(data.size() == size);

  return {format, target, source, data};
}
