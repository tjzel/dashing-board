#include <Communicators.hpp>
#include <DataFrame.hpp>
#include <DataLink.hpp>

struct Simulator {
  DataLink dataLink{};
  StdioDebugCommunicator debugCommunicator{};
  RequestHandlerDataLinkCommunicator requestHandlerCommunicator{dataLink};
  EcuMockDataLinkCommunicator ecuCommunicator{dataLink};
  EcuMock<EcuMockDataLinkCommunicator, StdioDebugCommunicator> ecuMock{ecuCommunicator,
                                                                       debugCommunicator};
  RequestHandler<RequestHandlerDataLinkCommunicator, StdioDebugCommunicator> requestHandler{
      requestHandlerCommunicator, debugCommunicator};
  Simulator() { requestHandler.loadAvailability(); }
};
