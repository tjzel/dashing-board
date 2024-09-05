#ifndef ECU_MOCK_HPP
#define ECU_MOCK_HPP

#include <DataProvider.hpp>
#include <DiagnosticCommands.hpp>
#include <EcuCore.hpp>
#include <ICommunicator.hpp>
#include <Message.hpp>
#include <Parser.hpp>
#include <cassert>

using namespace DiagnosticCommands;

enum State {
  WAITING_FOR_HEADER,
  WAITING_FOR_TARGET,
  WAITING_FOR_SOURCE,
  WAITING_FOR_DATA,
  WAITING_FOR_CHECKSUM,
};

struct InputState {
  Byte header{0x00};
  Byte dataSize{0x00};
  Byte target{0x00};
  Byte source{0x00};
  std::vector<Byte> data{};
  std::vector<Byte>::iterator dataIter{data.begin()};
  Byte checksum{0x00};
  State state{WAITING_FOR_HEADER};
};

template <ICommunicator TCommunicator> class EcuMock {
public:
  void inputArrivedHandler() { handleInput(); };

  explicit EcuMock(TCommunicator &comm) : _comm(comm) {}

private:
  void handleInput() {
    // TODO: Embed a state machine with composition instead.
    switch (_inputState.state) {
    case WAITING_FOR_HEADER:
      _inputState.header = _comm.read();
      if ((_inputState.header & REQUEST_HEADER_MODE_MASK) != REQUEST_HEADER) {
        return;
      }
      _inputState.dataSize = _inputState.header & Message::REQUEST_HEADER_SIZE_MASK;
      _inputState.data = std::vector<Byte>(_inputState.dataSize);
      _inputState.dataIter = _inputState.data.begin();
      _inputState.state = WAITING_FOR_TARGET;
      break;
    case WAITING_FOR_TARGET:
      _inputState.target = _comm.read();
      if (_inputState.target != ECU_ADDRESS) {
        _inputState.state = WAITING_FOR_HEADER;
        return;
      }
      _inputState.state = WAITING_FOR_SOURCE;
      break;
    case WAITING_FOR_SOURCE:
      _inputState.source = _comm.read(); // We don't do any checks on the source right now.
      _inputState.state = WAITING_FOR_DATA;
      break;
    case WAITING_FOR_DATA:
      *_inputState.dataIter++ = _comm.read();
      if (_inputState.dataIter == _inputState.data.end()) {
        _inputState.state = WAITING_FOR_CHECKSUM;
      };
      break;
    case WAITING_FOR_CHECKSUM:
      _inputState.checksum = _comm.read();
      Message message{_inputState.header, _inputState.target, _inputState.source, _inputState.data};
      if (_inputState.checksum != message.checksum) {
        _inputState.state = WAITING_FOR_HEADER;
        return;
      }

      // TODO: We drop non-OBD2 messages, but we should handle at least init protocol.
      if (message.isOBD2Message()) {
        OBD2Message obd2Message(message);
        auto responseWrapper = _ecuCore.request(obd2Message);
        if (responseWrapper.hasResponse) {
          // TODO: DRY
          _comm.write(responseWrapper.response);
        }
      } else {
        auto responseWrapper = _ecuCore.request(message);
        if (responseWrapper.hasResponse) {
          // TODO: DRY
          _comm.write(responseWrapper.response);
        }
      }
      _inputState.state = WAITING_FOR_HEADER;
      return;
    }
  }

  TCommunicator &_comm;
  InputState _inputState;
  EcuCore _ecuCore{};
};

#endif // ECU_MOCK_HPP
