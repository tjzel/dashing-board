#ifndef ECU_MOCK_HPP
#define ECU_MOCK_HPP

#include <DataProvider.hpp>
#include <DataSources.hpp>
#include <DiagnosticCommands.hpp>
#include <EcuResponder.hpp>
#include <ICommunicator.hpp>
#include <Message.hpp>
#include <Parser.hpp>
#include <StateReader.hpp>
#include <Utils.hpp>
#include <cassert>

/* #region Declaration */
template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator,
          IDataProviderManager TDataProviderManager>
class EcuMock {
public:
  void inputArrivedHandler();

  explicit EcuMock(TCommunicator &comm, TDebugCommunicator &debugComm,
                   TDataProviderManager &dataSource);

private:
  void handleInput();

  TCommunicator &comm_;
  TDebugCommunicator &debugComm_;
  StateReader stateReader_{
      {.isHeaderValid = [](int byte) { return (byte & ADDRESSING_MASK) == FUNCTIONAL_ADDRESSING; },
       .isTargetValid = [](int byte) { return byte == ECU_ADDRESS; },
       .isSourceValid = [](int) { return true; },
       .getTimestamp = []() { return 0; }}};
  EcuResponder<TDataProviderManager> ecuResponder_;
};
/* #endregion Declaration */
/* #region Implementation */

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator,
          IDataProviderManager TDataProviderManager>
void EcuMock<TCommunicator, TDebugCommunicator, TDataProviderManager>::inputArrivedHandler() {
  while (comm_.available()) {
    handleInput();
  }
};

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator,
          IDataProviderManager TDataProviderManager>
EcuMock<TCommunicator, TDebugCommunicator, TDataProviderManager>::EcuMock(
    TCommunicator &comm, TDebugCommunicator &debugComm, TDataProviderManager &dataSource)
    : comm_(comm), debugComm_(debugComm), ecuResponder_({dataSource}) {
  comm_.setOnNewData([&]() { inputArrivedHandler(); });
}

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator,
          IDataProviderManager TDataProviderManager>
void EcuMock<TCommunicator, TDebugCommunicator, TDataProviderManager>::handleInput() {
  auto byte = comm_.read();
  if (byte == -1 || !stateReader_.feed(byte)) {
    return;
  };
  auto message = stateReader_.getMessage();
  // printMessage(message, debugComm_);

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
