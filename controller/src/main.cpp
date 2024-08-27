#include <CommunicatorMock.hpp>
#include <DiagnosticCommands.hpp>
#include <RequestHandler.hpp>
#include <unordered_map>
// #include <iostream>

int main() {
  CommunicatorMock communicator{};
  RequestHandler<CommunicatorMock> handler(communicator, communicator);

  handler.loadAvailability();
  handler.printAvailableCommands();
  handler.printAvailableForDataFrame();

  handler.get<DiagnosticCommands::ENGINE_RPM>();

  return 0;
}
