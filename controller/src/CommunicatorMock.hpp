#ifndef COMMUNICATOR_MOCK_HPP
#define COMMUNICATOR_MOCK_HPP

#include "ICommunicator.hpp"
#include <DiagnosticCommands.hpp>
#include <RequestHandler.hpp>
#include <string>

constexpr size_t BUFFER_SIZE = 128;

using ByteBuffer = std::array<uint8_t, BUFFER_SIZE>;

// TODO: Get rid of those structs, use some Views on them instead.

/**
 * K-Line message has the following structure:
 * HEADER | RECEIVER | SENDER | DATA (OBD2 MESSAGE) | CHECKSUM
 */
struct Message {
  uint8_t header;
  uint8_t dataSize;
  uint8_t receiver;
  uint8_t sender;
  std::vector<uint8_t> data;
  uint8_t checksum;
};

/**
 * OBD2 message has the following structure:
 * MODE | PID | DATA
 */
struct OBD2Message {
  uint8_t mode;
  uint8_t pid;
  uint8_t dataSize;
  std::vector<uint8_t> data;
};

bool isOBD2Message(const Message &message);

class CommunicatorMock {
public:
  uint8_t read();

  void write(const std::vector<uint8_t> &message);

  void write(uint8_t byte);

  static void print(uint8_t byte);

  static void print(const std::string &str);

  static void println(uint8_t byte);

  static void println(const std::string &str);

  static void println();

private:
  bool hasValidInputMessage();
  void handleInput();
  void sendResponse(const OBD2Message &response);

  template <DiagnosticCommands::Command TCommand> void respond() {};

  template <> void respond<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>() {
    const OBD2Message response{
        DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::prefix,
        DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::command,
        0x04,
        {0x00, 0x18, 0x00, 0x00}};
    sendResponse(response);
  }

  template <> void respond<DiagnosticCommands::ENGINE_RPM>() {
    const OBD2Message response{DiagnosticCommands::ENGINE_RPM::prefix,
                               DiagnosticCommands::ENGINE_RPM::command,
                               0x02,
                               {0x21, 0x37}};
    sendResponse(response);
  };

  template <> void respond<DiagnosticCommands::VEHICLE_SPEED>() {
    const OBD2Message response{
        DiagnosticCommands::VEHICLE_SPEED::prefix,
        DiagnosticCommands::VEHICLE_SPEED::command,
        0x01, // TODO: Not nice not to have assert for size.
        {0x69}};
    sendResponse(response);
  };

  // TODO: Not nice to use a fixed type here for `command`.
  void react(const CommandLiteral command) {
    if (command == DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::value) {
      respond<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>();
    } else if (command == DiagnosticCommands::ENGINE_RPM::value) {
      respond<DiagnosticCommands::ENGINE_RPM>();
    } else if (command == DiagnosticCommands::VEHICLE_SPEED::value) {
      respond<DiagnosticCommands::VEHICLE_SPEED>();
    } else {
      // Ignore unknown commands.
      // TODO:  Compare this behavior to real ECU.
    }
  }

  ByteBuffer _outputByteBuffer{};
  ByteBuffer::iterator _outputGuard = _outputByteBuffer.begin();
  ByteBuffer _inputByteBuffer{};
  ByteBuffer::iterator _inputBegin = _inputByteBuffer.begin();
  ByteBuffer::iterator _inputEnd = _inputByteBuffer.begin();
  Message _lastInputMessage{};
  Message _currentInputMessage{};
  bool _hasValidInputMessage = false;
};

static_assert(ICommunicator<CommunicatorMock>);
static_assert(IDebugCommunicator<CommunicatorMock>);

#endif // COMMUNICATOR_MOCK_HPP