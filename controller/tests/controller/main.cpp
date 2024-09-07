#include <CommunicatorMock.hpp>
#include <DiagnosticCommands.hpp>
#include <RequestHandler.hpp>
#include <iostream>

int main() {
  // TODO: The pattern to create an explicit mock here is silly, refactor it.
  ControllerCommunicatorProxyMock comm;

  Message message{0xc2, 0x33, 0xf1, {0x01, 0x0c}};

  comm.write(std::vector<Byte>{message});

  while (comm.available()) {
    std::cout << std::hex << static_cast<int>(comm.read()) << " ";
  }
  std::cout << std::endl;

  // handler.loadAvailability();
  // handler.printAvailableCommands();
  // handler.printAvailableForDataFrame();

  // std::cout << static_cast<std::string>(handler.getDataFrame());

  // handler.sniff();
  // handler.sniff();

  return 0;
}
