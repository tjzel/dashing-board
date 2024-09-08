#ifndef COMMUNICATOR_MOCK_HPP
#define COMMUNICATOR_MOCK_HPP

#include <DiagnosticCommands.hpp>
#include <EcuMock.hpp>
#include <ICommunicator.hpp>
#include <Message.hpp>
#include <RequestHandler.hpp>
#include <span>
#include <string>

using namespace DiagnosticCommands;

constexpr size_t BUFFER_SIZE = 128;
using ByteBuffer = std::array<Byte, BUFFER_SIZE>;

class CommunicatorBufferMock;

class EcuCommunicatorProxyMock {
public:
  Byte read();
  bool available();

  void write(Byte byte);
  void write(const std::vector<Byte> &message);

  void fastInit();

  EcuCommunicatorProxyMock(CommunicatorBufferMock &communicatorBufferMock);

private:
  CommunicatorBufferMock &_communicatorBufferMock;
};

class DebugCommunicatorMock {
public:
  void print(Byte byte);
  void print(const std::string &str);
  void println(Byte byte);
  void println(const std::string &str);
  void println();
};

class CommunicatorBufferMock {
public:
  Byte controllerRead();
  bool controllerReadAvailable();
  void controllerWrite(Byte byte);
  void controllerWrite(const std::vector<Byte> &message);

  Byte ecuRead();
  bool ecuReadAvailable();
  void ecuWrite(Byte byte);
  void ecuWrite(const std::vector<Byte> &message);

  CommunicatorBufferMock();

private:
  ByteBuffer _controllerBuffer{};
  ByteBuffer::iterator _controllerBegin = _controllerBuffer.begin();
  ByteBuffer::iterator _controllerEnd = _controllerBuffer.begin();
  std::span<Byte> _controllerBufferView{_controllerBegin, _controllerEnd};
  void _onControllerBufferChanged();
  void _onControllerBufferReadWhenEmpty();

  ByteBuffer _ecuBuffer{};
  ByteBuffer::iterator _ecuBegin = _ecuBuffer.begin();
  ByteBuffer::iterator _ecuEnd = _ecuBuffer.begin();
  std::span<Byte> _ecuBufferView{_ecuBegin, _ecuEnd};
  void _onEcuBufferChanged();
  void _onEcuBufferWrite();

  EcuCommunicatorProxyMock _ecuCommunicatorProxyMock;
  EcuMock<EcuCommunicatorProxyMock, DebugCommunicatorMock> _ecuMock;
};

class ControllerCommunicatorProxyMock {
public:
  Byte read();
  bool available();

  void write(Byte byte);
  void write(const std::vector<Byte> &message);

  void fastInit();

  ControllerCommunicatorProxyMock();

private:
  CommunicatorBufferMock _communicatorBufferMock;
};

static_assert(ICommunicator<ControllerCommunicatorProxyMock>);
static_assert(ICommunicator<EcuCommunicatorProxyMock>);
static_assert(IDebugCommunicator<DebugCommunicatorMock>);

#endif // COMMUNICATOR_MOCK_HPP
