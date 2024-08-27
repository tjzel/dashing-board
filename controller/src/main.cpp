#include <CommunicatorMock.hpp>
#include <DiagnosticCommands.hpp>
#include <RequestHandler.hpp>
#include <iostream>

int main() {
  CommunicatorMock communicator{};
  RequestHandler handler(communicator, communicator);

  handler.loadAvailability();
  handler.printAvailableCommands();
  handler.printAvailableForDataFrame();

  handler.get<DiagnosticCommands::ENGINE_RPM>();
  std::cout << static_cast<std::string>(handler.getDataFrame());

  return 0;
}
