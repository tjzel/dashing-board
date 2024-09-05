#ifndef ECU_CORE_HPP
#define ECU_CORE_HPP

#include "DataProvider.hpp"
#include <DiagnosticCommands.hpp>
#include <Message.hpp>

struct EcuInternalResponse {
  bool hasResponse{};
  Message response;
};

class EcuCore {
public:
  EcuInternalResponse request(Message &message);
  EcuInternalResponse request(OBD2Message &message);

private:
  /* #region respondTo */
  template <DiagnosticCommands::Command TCommand> Message respondTo(OBD2Message &) {
    throw std::runtime_error("Not implemented");
  }

  Message respondToInit(Message &message);
  /* #endregion respondTo */

  Message createResponse(const Byte target, const Byte mode, const Byte pid, const std::vector<Byte> &data);

  Message createResponse(const Byte target, const std::vector<Byte> &data);

  // TODO: Don't hardcode it here.
  DataProvider _rpmProvider{800, 100, 1, 800, 4000};
  DataProvider _speedProvider{10, 1, 1, 10, 100};
};

template <> Message EcuCore::respondTo<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>(OBD2Message &message);
template <> Message EcuCore::respondTo<DiagnosticCommands::ENGINE_RPM>(OBD2Message &message);
template <> Message EcuCore::respondTo<DiagnosticCommands::VEHICLE_SPEED>(OBD2Message &message);

#endif // ECU_CORE_HPP
