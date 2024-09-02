#include <CommunicatorMock.hpp>
#include <DiagnosticCommands.hpp>
#include <RequestHandler.hpp>
#include <iostream>

int main() {
  ControllerCommunicatorProxyMock comm;
  DebugCommunicatorMock debugComm;
  RequestHandler handler(comm, debugComm);

  handler.loadAvailability();
  handler.printAvailableCommands();
  handler.printAvailableForDataFrame();

  std::cout << static_cast<std::string>(handler.getDataFrame());

  handler.sniff();
  handler.sniff();

  return 0;
}
