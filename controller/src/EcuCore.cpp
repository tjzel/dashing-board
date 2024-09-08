#include "DiagnosticCommands.hpp"
#include "ICommunicator.hpp"
#include "Parser.hpp"
#include <EcuCore.hpp>
#include <cassert>

EcuInternalResponse EcuCore::request(Message &message) {
  if (message.isInitMessage()) {
    return {true, respondToInit(message)};
  }
  // TODO: Use mock message instead.
  return {false, {0x00, 0x00, 0x00, {}}};
}

EcuInternalResponse EcuCore::request(OBD2Message &message) {
  if (message.command() == DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::value) {
    return {true, respondTo<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>(message)};
  }
  if (message.command() == DiagnosticCommands::ENGINE_RPM::value) {
    return {true, respondTo<DiagnosticCommands::ENGINE_RPM>(message)};
  }
  if (message.command() == DiagnosticCommands::VEHICLE_SPEED::value) {
    return {true, respondTo<DiagnosticCommands::VEHICLE_SPEED>(message)};
  }
  // TODO: Use mock message instead.
  return {false, {0x00, 0x00, 0x00, {}}};
}

template <> Message EcuCore::respondTo<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>(OBD2Message &message) {
  // TODO: DRY
  const Byte target = message.source();
  // TODO: Don't hardcode + 0x40 here.
  const Byte mode = DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::mode + 0x40;
  const Byte pid = DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::pid;
  assert(pid == message.pid());
  // 0x18 - only RPM and SPEED available.
  // TODO: Use reverseParse from a map.
  const std::vector<Byte> data{0x00, 0x18, 0x00, 0x00};
  assert(data.size() == DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::ParsingFormula::byteCount);
  return createResponse(target, mode, pid, data);
}

template <> Message EcuCore::respondTo<DiagnosticCommands::ENGINE_RPM>(OBD2Message &message) {
  // TODO: DRY
  const Byte target = message.source();
  // TODO: Don't hardcode + 0x40 here.
  const Byte mode = DiagnosticCommands::ENGINE_RPM::mode + 0x40;
  const Byte pid = DiagnosticCommands::ENGINE_RPM::pid;
  assert(pid == message.pid());
  const auto data = Parser<DiagnosticCommands::ENGINE_RPM>::reverseParse(_rpmProvider.get());
  assert(data.size() == DiagnosticCommands::ENGINE_RPM::ParsingFormula::byteCount);
  return createResponse(target, mode, pid, data);
}

template <> Message EcuCore::respondTo<DiagnosticCommands::VEHICLE_SPEED>(OBD2Message &message) {
  // TODO: DRY
  const Byte target = message.source();
  // TODO: Don't hardcode + 0x40 here.
  const Byte mode = DiagnosticCommands::VEHICLE_SPEED::mode + 0x40;
  const Byte pid = DiagnosticCommands::VEHICLE_SPEED::pid;
  assert(pid == message.pid());
  const auto data = Parser<DiagnosticCommands::VEHICLE_SPEED>::reverseParse(_speedProvider.get());
  assert(data.size() == DiagnosticCommands::VEHICLE_SPEED::ParsingFormula::byteCount);
  return createResponse(target, mode, pid, data);
}

Message EcuCore::respondToInit(Message &message) {
  // TODO: Don't hardcode it here.
  // 0x83 0xf1 0x11 0xc1 0x8f 0xef 0xc4
  const std::vector<Byte> initOkData{0xc1, 0x8f, 0xef};
  return createResponse(message.source, initOkData);
}

Message EcuCore::createResponse(const Byte target, const Byte mode, const Byte pid, const std::vector<Byte> &obd2Data) {
  // TODO: DRY
  const Byte size = obd2Data.size() + OBD2_MIN_HEADER_SIZE;
  const Byte format = RESPONSE_HEADER | size;
  const Byte source = ECU_ADDRESS;

  std::vector<Byte> data{mode, pid};
  data.insert(data.end(), obd2Data.begin(), obd2Data.end());
  assert(data.size() == size);

  return {format, target, source, data};
}

Message EcuCore::createResponse(const Byte target, const std::vector<Byte> &data) {
  // TODO: DRY
  const Byte size = data.size() + OBD2_MIN_HEADER_SIZE;
  const Byte format = RESPONSE_HEADER | size;
  const Byte source = ECU_ADDRESS;

  // TODO: Fix this.
  // assert(data.size() == size);

  return {format, target, source, data};
}
