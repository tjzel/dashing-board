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

  std::cout << static_cast<std::string>(handler.getDataFrame());

  handler.sniff();
  handler.sniff();

  return 0;
}
