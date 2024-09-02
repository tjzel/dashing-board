#ifndef ECU_MOCK_HPP
#define ECU_MOCK_HPP

#include <DataProvider.hpp>
#include <DiagnosticCommands.hpp>
#include <ICommunicator.hpp>
#include <Message.hpp>
#include <Parser.hpp>
#include <cassert>
#include <coroutine>

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

template <ICommunicator TCommunicator>
class EcuMock {
public:
  void inputArrivedHandler() {
    while (_comm.available()) {
      handleInput();
    }
  };

  EcuMock(TCommunicator &comm) : _comm(comm) {}

private:
  void handleInput() {
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
        handleDiagnosticRequest(obd2Message);
      }
      _inputState.state = WAITING_FOR_HEADER;
      return;
    }
  }

  void handleDiagnosticRequest(OBD2Message &message) {
    // TODO: Reject malformed messages, i.e. with data longer than defined in the standard.
    const CommandLiteral command = message.command();
    if (command == COMMAND_AVAILABILITY_00_1F::value) {
      respondTo<COMMAND_AVAILABILITY_00_1F>(message);
    } else if (command == ENGINE_RPM::value) {
      respondTo<ENGINE_RPM>(message);
    } else if (command == VEHICLE_SPEED::value) {
      respondTo<VEHICLE_SPEED>(message);
    } else {
      // Ignore unknown pids.
      // TODO: Compare this behavior to real ECU.
    }
  };

  template <DiagnosticCommands::Command TCommand>
  void respondTo(OBD2Message &) {
    throw std::runtime_error("Not implemented");
  }

  /* #region respondTo specializations */
  template <>
  void respondTo<COMMAND_AVAILABILITY_00_1F>(OBD2Message &message) {
    const Byte target = message.message.source;
    const Byte mode = COMMAND_AVAILABILITY_00_1F::mode;
    const Byte pid = COMMAND_AVAILABILITY_00_1F::pid;
    assert(pid == COMMAND_AVAILABILITY_00_1F::pid);
    // 0x18 - only RPM and SPEED available.
    // TODO: Use reverseParse from a map.
    const std::vector<Byte> data{0x00, 0x18, 0x00, 0x00};
    assert(data.size() == COMMAND_AVAILABILITY_00_1F::ParsingFormula::byteCount);

    sendResponse(target, mode, pid, data);
  }
  template <>
  void respondTo<ENGINE_RPM>(OBD2Message &message) {
    const Byte target = message.message.source;
    const Byte mode = ENGINE_RPM::mode;
    const Byte pid = ENGINE_RPM::pid;
    assert(pid == ENGINE_RPM::pid);
    const auto data = Parser<ENGINE_RPM>::reverseParse(_rpmProvider.get());
    assert(data.size() == ENGINE_RPM::ParsingFormula::byteCount);

    sendResponse(target, mode, pid, data);
  }
  template <>
  void respondTo<VEHICLE_SPEED>(OBD2Message &message) {
    const Byte target = message.message.source;
    const Byte mode = VEHICLE_SPEED::mode;
    const Byte pid = VEHICLE_SPEED::pid;
    assert(pid == VEHICLE_SPEED::pid);
    const auto data = Parser<VEHICLE_SPEED>::reverseParse(_speedProvider.get());
    assert(data.size() == VEHICLE_SPEED::ParsingFormula::byteCount);

    sendResponse(target, mode, pid, data);
  }
  /* #endregion respondTo specializations */

  void sendResponse(Byte target, Byte mode, Byte pid, const std::vector<Byte> &obd2Data) {
    const Byte size = obd2Data.size() + OBD2_MIN_HEADER_SIZE;
    const Byte format = RESPONSE_HEADER | size;

    std::vector<Byte> data{mode, pid};
    data.insert(data.end(), obd2Data.begin(), obd2Data.end());
    assert(data.size() == size);

    const Message response{format, target, ECU_ADDRESS, data};
    _comm.write(response);
  };

  TCommunicator &_comm;
  DataProvider _rpmProvider{800, 100, 1, 800, 4000};
  DataProvider _speedProvider{10, 1, 1, 10, 100};
  InputState _inputState;
};

#endif // ECU_MOCK_HPP
