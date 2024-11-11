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
  if (message.command() == DiagnosticCommands::ENGINE_RPM::value) {
    return {.hasResponse = true, .response = respondTo<DiagnosticCommands::ENGINE_RPM>(message)};
  }
  if (message.command() == DiagnosticCommands::VEHICLE_SPEED::value) {
    return {.hasResponse = true, .response = respondTo<DiagnosticCommands::VEHICLE_SPEED>(message)};
  }
  // TODO: Use mock message instead.
  return {.hasResponse = false, .response = {0x00, 0x00, 0x00, {}}};
}

template <>
Message
EcuResponder::respondTo<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>(OBD2Message &message) {
  // TODO: DRY
  const Byte target = message.source();
  // TODO: Don't hardcode + 0x40 here.
  const Byte mode = DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::mode + 0x40;
  const Byte pid = DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::pid;
  assert(pid == message.pid());
  // 0x18 - only RPM and SPEED available.
  // TODO: Use encode from a map.
  const std::vector<Byte> data{0x00, 0x18, 0x00, 0x00};
  assert(data.size() == DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::Encoding::byteCount);
  return createResponse(target, mode, pid, data);
}

template <> Message EcuResponder::respondTo<DiagnosticCommands::ENGINE_RPM>(OBD2Message &message) {
  // TODO: DRY
  const Byte target = message.source();
  // TODO: Don't hardcode + 0x40 here.
  const Byte mode = DiagnosticCommands::ENGINE_RPM::mode + 0x40;
  const Byte pid = DiagnosticCommands::ENGINE_RPM::pid;
  assert(pid == message.pid());
  auto rpm = rpmProvider_.get();
  // std::cout << "wartosc: " << rpm << std::endl;
  const auto data = DiagnosticCodec<DiagnosticCommands::ENGINE_RPM>::encode(rpm);
  assert(data.size() == DiagnosticCommands::ENGINE_RPM::Encoding::byteCount);
  return createResponse(target, mode, pid, data);
}

template <>
Message EcuResponder::respondTo<DiagnosticCommands::VEHICLE_SPEED>(OBD2Message &message) {
  // TODO: DRY
  const Byte target = message.source();
  // TODO: Don't hardcode + 0x40 here.
  const Byte mode = DiagnosticCommands::VEHICLE_SPEED::mode + 0x40;
  const Byte pid = DiagnosticCommands::VEHICLE_SPEED::pid;
  assert(pid == message.pid());
  const auto data =
      DiagnosticCodec<DiagnosticCommands::VEHICLE_SPEED>::encode(speedProvider_.get());
  assert(data.size() == DiagnosticCommands::VEHICLE_SPEED::Encoding::byteCount);
  return createResponse(target, mode, pid, data);
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
  const Byte size = obd2Data.size() + OBD2_MIN_HEADER_SIZE;
  const Byte format = RESPONSE_HEADER | size;
  const Byte source = ECU_ADDRESS;

  std::vector<Byte> data{mode, pid};
  data.insert(data.end(), obd2Data.begin(), obd2Data.end());
  assert(data.size() == size);

  return {format, target, source, data};
}

Message EcuResponder::createResponse(const Byte target, const std::vector<Byte> &data) {
  // TODO: DRY
  const Byte size = data.size() + OBD2_MIN_HEADER_SIZE;
  const Byte format = RESPONSE_HEADER | size;
  const Byte source = ECU_ADDRESS;

  // TODO: Fix this.
  // assert(data.size() == size);

  return {format, target, source, data};
}
