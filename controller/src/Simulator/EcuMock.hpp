#ifndef ECU_MOCK_HPP
#define ECU_MOCK_HPP

#include <DataProvider.hpp>
#include <DiagnosticCommands.hpp>
#include <EcuResponder.hpp>
#include <ICommunicator.hpp>
#include <Message.hpp>
#include <Parser.hpp>
#include <StateReader.hpp>
#include <Utils.hpp>
#include <cassert>

/* #region Declaration */
template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator> class EcuMock {
public:
  void inputArrivedHandler();

  explicit EcuMock(TCommunicator &comm, TDebugCommunicator &debugComm);

private:
  void handleInput();

  TCommunicator &comm_;
  TDebugCommunicator &debugComm_;
  StateReader stateReader_{// TODO: Fix magic number.
                           [](int byte) { return (byte & REQUEST_HEADER_MODE_MASK) == 0xc0; },
                           [](int byte) { return byte == ECU_ADDRESS; }, [](int) { return true; }, []() { return 0; }};
  EcuResponder ecuResponder_{};
};
/* #endregion Definition */
/* #region Implementation */

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void EcuMock<TCommunicator, TDebugCommunicator>::inputArrivedHandler() {
  while (comm_.available()) {
    handleInput();
  }
};

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
EcuMock<TCommunicator, TDebugCommunicator>::EcuMock(TCommunicator &comm, TDebugCommunicator &debugComm)
    : comm_(comm), debugComm_(debugComm) {
  comm_.setOnNewData([&]() { inputArrivedHandler(); });
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void EcuMock<TCommunicator, TDebugCommunicator>::handleInput() {
  auto byte = comm_.read();
  if (!stateReader_.feed(byte)) {
    return;
  };
  auto message = stateReader_.getMessage();
  printMessage(message, debugComm_);

  // TODO: We drop non-OBD2 messages, but we should handle at least init protocol.
  if (message.isOBD2Message()) {
    OBD2Message obd2Message(message);
    auto responseWrapper = ecuResponder_.request(obd2Message);
    if (responseWrapper.hasResponse) {
      // TODO: DRY
      comm_.write(std::vector<Byte>{responseWrapper.response});
    }
  } else {
    auto responseWrapper = ecuResponder_.request(message);
    if (responseWrapper.hasResponse) {
      // TODO: DRY
      comm_.write(std::vector<Byte>{responseWrapper.response});
    }
  }
}

/* #endregion Implementation */

#endif // ECU_MOCK_HPP
