#ifndef COMMUNICATOR_MOCK_HPP
#define COMMUNICATOR_MOCK_HPP

#include "Parser.hpp"
#include <DiagnosticCommands.hpp>
#include <ICommunicator.hpp>
#include <Message.hpp>
#include <RequestHandler.hpp>
#include <span>
#include <string>

using namespace DiagnosticCommands;

constexpr size_t BUFFER_SIZE = 128;
using ByteBuffer = std::array<byte, BUFFER_SIZE>;

class DataProvider {
public:
  int get();

  DataProvider(int initialValue, int step, int sign, int bottomThreshold,
               int topThreshold);

private:
  int _value;
  int _step;
  int _sign;
  int _bottomThreshold;
  int _topThreshold;
};

class CommunicatorMock {
public:
  byte read();

  void write(const std::vector<byte> &message);

  void write(byte byte);

  static void print(byte byte);

  static void print(const std::string &str);

  static void println(byte byte);

  static void println(const std::string &str);

  static void println();

private:
  bool hasValidInputMessage();
  void handleInput();
  void sendResponse(const byte mode, const byte pid,
                    const std::span<const byte> obd2Payload);
  void send(const Message &message);

  template <Command TCommand> void respond() {};

  template <> void respond<COMMAND_AVAILABILITY_00_1F>() {
    const byte mode = COMMAND_AVAILABILITY_00_1F::mode;
    const byte pid = COMMAND_AVAILABILITY_00_1F::pid;
    const std::array<byte,
                     COMMAND_AVAILABILITY_00_1F::ParsingFormula::byteCount>
        data{0x00, 0x18, 0x00, 0x00};

    sendResponse(mode, pid, data);
  }

  template <> void respond<ENGINE_RPM>() {
    const byte mode = ENGINE_RPM::mode;
    const byte pid = ENGINE_RPM::pid;
    const auto data = Parser<ENGINE_RPM>::reverseParse(_rpmProvider.get());

    sendResponse(mode, pid, data);
  };

  template <> void respond<VEHICLE_SPEED>() {
    const byte mode = VEHICLE_SPEED::mode;
    const byte pid = VEHICLE_SPEED::pid;
    const auto data = Parser<VEHICLE_SPEED>::reverseParse(_speedProvider.get());

    sendResponse(mode, pid, data);
  };

  void handleDiagnosticRequest(const CommandLiteral pid);

  static constexpr byte ECU_ADDRESS = 0x33;
  static constexpr byte REQUEST_HEADER = 0xC0;
  static constexpr byte RESPONSE_HEADER = 0x80;
  static constexpr byte REQUEST_HEADER_MODE_MASK = 0xf0;
  static constexpr byte OBD2_HEADER_SIZE = 0x02;

  ByteBuffer _inputBuffer{};
  ByteBuffer::iterator _inputEnd = _inputBuffer.begin();
  ByteBuffer _outputBuffer{};
  ByteBuffer::iterator _outputBegin = _outputBuffer.begin();
  ByteBuffer::iterator _outputEnd = _outputBuffer.begin();
  Message _inputMessage{0x00, 0x00, 0x00, {}};
  Message _outputMessage{0x00, 0x00, 0x00, {}};
  bool _hasValidInputMessage = false;
  DataProvider _speedProvider{10, 1, 1, 10, 100};
  DataProvider _rpmProvider{800, 100, 1, 800, 4000};
};

static_assert(ICommunicator<CommunicatorMock>);
static_assert(IDebugCommunicator<CommunicatorMock>);

#endif // COMMUNICATOR_MOCK_HPP
