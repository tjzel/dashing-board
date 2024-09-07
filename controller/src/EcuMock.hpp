#ifndef ECU_MOCK_HPP
#define ECU_MOCK_HPP

#include <DataProvider.hpp>
#include <DiagnosticCommands.hpp>
#include <EcuCore.hpp>
#include <ICommunicator.hpp>
#include <Message.hpp>
#include <Parser.hpp>
#include <StateReader.hpp>
#include <cassert>

template <ICommunicator TCommunicator> class EcuMock {
public:
  void inputArrivedHandler() {
    while (_comm.available()) {
      handleInput();
    }
  };

  explicit EcuMock(TCommunicator &comm) : _comm(comm) {}

private:
  void handleInput() {
    auto byte = _comm.read();
    if (!_stateReader.feed(byte)) {
      return;
    };

    auto message = _stateReader.getMessage();

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
  StateReader _stateReader{
      // TODO: Fix magic number.
      [](Byte byte) { return (byte & REQUEST_HEADER_MODE_MASK) == 0xc0; },
      [](Byte byte) { return byte == ECU_ADDRESS; },
      [](Byte) { return true; },
  };
  EcuCore _ecuCore{};
};

#endif // ECU_MOCK_HPP
