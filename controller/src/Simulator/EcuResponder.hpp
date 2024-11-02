#ifndef ECU_RESPONDER_HPP
#define ECU_RESPONDER_HPP

#include <DataProvider.hpp>
#include <DiagnosticCommands.hpp>
#include <Message.hpp>

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
  Message respondToInit(Message &message);

  Message createResponse(Byte target, Byte mode, Byte pid, const std::vector<Byte> &data);

  Message createResponse(Byte target, const std::vector<Byte> &data);

  // TODO: Don't hardcode it here.
  DataProvider rpmProvider_{800, 100, 1, 800, 4000};
  DataProvider speedProvider_{10, 1, 1, 10, 100};
};

/* #region Specialization */

template <> Message EcuResponder::respondTo<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>(OBD2Message &message);
template <> Message EcuResponder::respondTo<DiagnosticCommands::ENGINE_RPM>(OBD2Message &message);
template <> Message EcuResponder::respondTo<DiagnosticCommands::VEHICLE_SPEED>(OBD2Message &message);

/* #endregion Specializations */

/* #region Implementation */

template <DiagnosticCommands::Command TCommand> Message EcuResponder::respondTo(OBD2Message & /*message*/) {
  throw std::runtime_error("Not implemented");
}

/* #endregion Implementation */

#endif // ECU_RESPONDER_HPP
