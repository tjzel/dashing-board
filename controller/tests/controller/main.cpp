#include "EcuMock.hpp"
#include <CommunicatorMock.hpp>
#include <DiagnosticCommands.hpp>
#include <RequestHandler.hpp>
#include <iostream>

int main() {
  ControllerCommunicatorProxyMock comm;

  Message message{0xc2, 0x33, 0xf1, {0x01, 0x0c}};

  while (comm.available()) {
    comm.write(message);
  }

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
