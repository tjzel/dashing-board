#include <Communicators.hpp>
#include <DiagnosticCommands.hpp>
#include <EcuMock.hpp>
#include <RequestHandler.hpp>
#include <iostream>

// TODO: Move Simulator to a separate file.
struct Simulator {
  DataLink dataLink{};
  StdioDebugCommunicator debugCommunicator{};
  RequestHandlerDataLinkCommunicator requestHandlerCommunicator{dataLink};
  EcuMockDataLinkCommunicator ecuCommunicator{dataLink};
  EcuMock<EcuMockDataLinkCommunicator, StdioDebugCommunicator> ecuMock{ecuCommunicator, debugCommunicator};
  RequestHandler<RequestHandlerDataLinkCommunicator, StdioDebugCommunicator> requestHandler{requestHandlerCommunicator,
                                                                                            debugCommunicator};
};

int main() {
  Simulator simulator{};

  std::cout << simulator.requestHandler.get<DiagnosticCommands::ENGINE_RPM>() << std::endl;

  return 0;
}
