#include <Communicators.hpp>
#include <DiagnosticCommands.hpp>
#include <Parser.hpp>
#include <RequestHandler.hpp>
#include <Utils.hpp>
#include <iostream>
#include <string>

/* #region RequestHandlerDataLinkCommunicator */

int RequestHandlerDataLinkCommunicator::read() { return dataLink_.controllerRead(); }

bool RequestHandlerDataLinkCommunicator::available() { return dataLink_.controllerReadAvailable(); }

void RequestHandlerDataLinkCommunicator::write(Byte byte) { dataLink_.controllerWrite(byte); }

void RequestHandlerDataLinkCommunicator::write(const std::vector<Byte> &message) {
  dataLink_.controllerWrite(message);
}

void RequestHandlerDataLinkCommunicator::init() {}

void RequestHandlerDataLinkCommunicator::setOnNewData(std::function<void()> /*onNewData*/) {}

RequestHandlerDataLinkCommunicator::RequestHandlerDataLinkCommunicator(DataLink &dataLinkMock)
    : dataLink_(dataLinkMock) {}

/* #endregion RequestHandlerDataLinkCommunicator */
/* #region EcuMockDataLinkCommunicator */

int EcuMockDataLinkCommunicator::read() { return dataLink_.ecuRead(); }

bool EcuMockDataLinkCommunicator::available() { return dataLink_.ecuReadAvailable(); }

void EcuMockDataLinkCommunicator::write(Byte byte) { dataLink_.ecuWrite(byte); }

void EcuMockDataLinkCommunicator::write(const std::vector<Byte> &message) {
  dataLink_.ecuWrite(message);
}

void EcuMockDataLinkCommunicator::init() {}

void EcuMockDataLinkCommunicator::setOnNewData(std::function<void()> onNewData) {
  dataLink_.setOnNewDataForEcu(std::move(onNewData));
}

EcuMockDataLinkCommunicator::EcuMockDataLinkCommunicator(DataLink &dataLinkMock)
    : dataLink_(dataLinkMock) {}

/* #endregion EcuMockDataLinkCommunicator */

/* #region StdioDebugCommunicator */

void StdioDebugCommunicator::print(const Byte byte) {
  std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte) << std::dec;
}

void StdioDebugCommunicator::print(const std::string &str) { std::cout << str; }

void StdioDebugCommunicator::println(const Byte byte) {
  print(byte);
  println();
}

void StdioDebugCommunicator::println(const std::string &str) {
  std::cout << str;
  println();
}

void StdioDebugCommunicator::println() { std::cout << "\n"; }

/* #endregion StdioDebugCommunicator */

/**
 * OBD2 message contains at least two bytes - mode and PID.
 */
// TODO: Get it out of here.
bool isOBD2Message(Message &message) { return message.dataSize() > 1; }
