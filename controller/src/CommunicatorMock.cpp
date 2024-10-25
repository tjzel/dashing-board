#include <CommunicatorMock.hpp>
#include <DiagnosticCommands.hpp>
#include <Parser.hpp>
#include <RequestHandler.hpp>
#include <Utils.hpp>
#include <iostream>
#include <string>

/* #region BufferMock */

Byte CommunicatorBufferMock::controllerRead() {
  if (_controllerBufferView.empty()) {
    _onControllerBufferReadWhenEmpty();
  }
  const auto byte = _controllerBufferView.front();
  _controllerBegin++;
  _onControllerBufferChanged();
  return byte;
}

bool CommunicatorBufferMock::controllerReadAvailable() { return !_controllerBufferView.empty(); }

void CommunicatorBufferMock::controllerWrite(Byte byte) {
  *_ecuEnd++ = byte;
  _onEcuBufferChanged();
  _onEcuBufferWrite();
}

void CommunicatorBufferMock::controllerWrite(const std::vector<Byte> &message) {
  auto *it = _ecuEnd;
  for (auto byte : message) {
    *it++ = byte;
  }
  _ecuEnd = it;
  _onEcuBufferChanged();
  _onEcuBufferWrite();
}

Byte CommunicatorBufferMock::ecuRead() {
  if (_ecuBufferView.empty()) {
    throw std::runtime_error("ECU should never attempt to read from empty buffer.");
  }
  const auto byte = _ecuBufferView.front();
  _ecuBegin++;
  _onEcuBufferChanged();
  return byte;
}

bool CommunicatorBufferMock::ecuReadAvailable() { return !_ecuBufferView.empty(); }

void CommunicatorBufferMock::ecuWrite(Byte byte) {
  *_controllerEnd++ = byte;
  _onControllerBufferChanged();
}

void CommunicatorBufferMock::ecuWrite(const std::vector<Byte> &message) {
  auto *it = _controllerEnd;
  for (auto byte : message) {
    *it++ = byte;
  }
  _controllerEnd = it;
  _onControllerBufferChanged();
}

CommunicatorBufferMock::CommunicatorBufferMock()
    : _ecuCommunicatorProxyMock(*this), _ecuMock(_ecuCommunicatorProxyMock, _debugCommunicatorMock) {}

void CommunicatorBufferMock::_onControllerBufferChanged() {
  if (_controllerBegin == _controllerEnd) {
    _controllerBegin = _controllerBuffer.begin();
    _controllerEnd = _controllerBegin;
  }
  _controllerBufferView = std::span<Byte>(_controllerBegin, _controllerEnd);
}

void CommunicatorBufferMock::_onControllerBufferReadWhenEmpty() {
  // Simulating that another controller request was sent.
  const Message message{
      REQUEST_HEADER | OBD2_MIN_HEADER_SIZE, ECU_ADDRESS, CONTROLLER_ADDRESS, {ENGINE_RPM::mode, ENGINE_RPM::pid}};
  controllerWrite(std::vector<Byte>{message});
}

void CommunicatorBufferMock::_onEcuBufferChanged() {
  if (_ecuBegin == _ecuEnd) {
    _ecuBegin = _ecuBuffer.begin();
    _ecuEnd = _ecuBegin;
  }
  _ecuBufferView = std::span<Byte>(_ecuBegin, _ecuEnd);
}

void CommunicatorBufferMock::_onEcuBufferWrite() { _ecuMock.inputArrivedHandler(); }

/* #endregion BufferMock */

/* #region Proxies */

int ControllerCommunicatorProxyMock::read() { return _communicatorBufferMock.controllerRead(); }

bool ControllerCommunicatorProxyMock::available() { return _communicatorBufferMock.controllerReadAvailable(); }

void ControllerCommunicatorProxyMock::write(Byte byte) { _communicatorBufferMock.controllerWrite(byte); }

void ControllerCommunicatorProxyMock::write(const std::vector<Byte> &message) {
  _communicatorBufferMock.controllerWrite(message);
}

ControllerCommunicatorProxyMock::ControllerCommunicatorProxyMock() : _communicatorBufferMock() {}

int EcuCommunicatorProxyMock::read() { return _communicatorBufferMock.ecuRead(); }

bool EcuCommunicatorProxyMock::available() { return _communicatorBufferMock.ecuReadAvailable(); }

void EcuCommunicatorProxyMock::write(Byte byte) { _communicatorBufferMock.ecuWrite(byte); }

void EcuCommunicatorProxyMock::write(const std::vector<Byte> &message) { _communicatorBufferMock.ecuWrite(message); }

EcuCommunicatorProxyMock::EcuCommunicatorProxyMock(CommunicatorBufferMock &communicatorBufferMock)
    : _communicatorBufferMock(communicatorBufferMock) {}

/* #endregion Proxies */

/* #region DebugCommunicatorMock */

void DebugCommunicatorMock::print(const Byte byte) {
  std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
}

void DebugCommunicatorMock::print(const std::string &str) { std::cout << str; }

void DebugCommunicatorMock::println(const Byte byte) {
  std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
  println();
}

void DebugCommunicatorMock::println(const std::string &str) {
  std::cout << str;
  println();
}

void DebugCommunicatorMock::println() { std::cout << "\n"; }

/* #endregion DebugCommunicatorMock */

/**
 * OBD2 message contains at least two bytes - mode and PID.
 */
bool isOBD2Message(Message &message) { return message.dataSize() > 1; }
