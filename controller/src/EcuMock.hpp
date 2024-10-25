#ifndef ECU_MOCK_HPP
#define ECU_MOCK_HPP

#include <DataProvider.hpp>
#include <DiagnosticCommands.hpp>
#include <EcuCore.hpp>
#include <ICommunicator.hpp>
#include <Message.hpp>
#include <Parser.hpp>
#include <StateReader.hpp>
#include <Utils.hpp>
#include <cassert>

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator> class EcuMock {
public:
  void inputArrivedHandler() {
    while (_comm.available()) {
      handleInput();
    }
  };

  explicit EcuMock(TCommunicator &comm, TDebugCommunicator &debugComm) : _comm(comm), _debugComm(debugComm) {}

private:
  void handleInput() {
    auto byte = _comm.read();
    if (!_stateReader.feed(byte)) {
      return;
    };
    auto message = _stateReader.getMessage();
    printMessage(message, _debugComm);

    // TODO: We drop non-OBD2 messages, but we should handle at least init protocol.
    if (message.isOBD2Message()) {
      OBD2Message obd2Message(message);
      auto responseWrapper = _ecuCore.request(obd2Message);
      if (responseWrapper.hasResponse) {
        // TODO: DRY
        _comm.write(std::vector<Byte>{responseWrapper.response});
      }
    } else {
      auto responseWrapper = _ecuCore.request(message);
      if (responseWrapper.hasResponse) {
        // TODO: DRY
        _comm.write(std::vector<Byte>{responseWrapper.response});
      }
    }
  }

  // TODO: Use something better than references.
  TCommunicator &_comm;
  TDebugCommunicator &_debugComm;
  StateReader _stateReader{// TODO: Fix magic number.
                           [](int byte) { return (byte & REQUEST_HEADER_MODE_MASK) == 0xc0; },
                           [](int byte) { return byte == ECU_ADDRESS; }, [](int) { return true; }, []() { return 0; }};
  EcuCore _ecuCore{};
};

#endif // ECU_MOCK_HPP
