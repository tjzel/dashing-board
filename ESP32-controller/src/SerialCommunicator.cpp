#include "SerialCommunicator.hpp"
#include <iostream>

byte SerialCommunicator::read(const size_t timeout) {
  const auto start = micros();
  while (!available()) {
    if (micros() - start > timeout) {
      // TODO: Log the timeout.
      // std::cout << "Timeout" << std::endl;
      return 0x00;
    }
  }
  // std::cout << "Time spent waiting: " << (micros() - start) << "ms\n";
  _waitTime += micros() - start;
  return _serial.read();
}

bool SerialCommunicator::available() { return _serial.available(); }

void SerialCommunicator::write(const std::vector<byte> &message) {
  // std::cout << "Sending:\n    ";
  // for (auto byte : message) {
  //   std::cout << std::hex << static_cast<int>(byte) << " ";
  // }
  // std::cout << std::endl;
  // std::cout << "\n    SEND OK" << std::endl;
  auto writeStart = micros();
  _serial.write(message.data(), message.size());
  auto writeTime = micros() - writeStart;
  // We need to dump the echo
  auto dumpStart = micros();
  for (auto byte : message) {
    read();
  }
  auto dumpTime = micros() - dumpStart;
  _operationCount++;
  _writeTime += writeTime;
  _dumpTime += dumpTime;
  if (_operationCount == 100) {
    auto elapsed = micros() - _time;
    std::cout << "Time elapsed: " << elapsed / 1000000.0 << "s\n";
    std::cout << "Operations per second: " << 100.0 / (elapsed / 1000000.0) << "\n";
    std::cout << "Wait time: " << _waitTime / 1000000.0 << "s\n";
    std::cout << "Write time: " << _writeTime / 1000000.0 << "s\n";
    std::cout << "Dump time: " << _dumpTime / 1000000.0 << "s\n";
    std::cout << std::endl;
    _time = micros();
    _waitTime = 0;
    _dumpTime = 0;
    _writeTime = 0;
    _operationCount = 0;
  }
  // auto now = micros();
  // // std::cout << "Time elapsed: " << (now - _time) << "ms\n";
  // _time = now;
}

void SerialCommunicator::write(byte byte) { _serial.write(byte); }

SerialCommunicator::SerialCommunicator(Stream &serial) : _serial(serial) {}

void DebugSerialCommunicator::print(byte byte) { _serial.print(byte, HEX); }

void DebugSerialCommunicator::print(const std::string &str) { _serial.print(str.c_str()); }

void DebugSerialCommunicator::println(byte byte) { _serial.println(byte, HEX); }

void DebugSerialCommunicator::println(const std::string &str) { _serial.println(str.c_str()); }

void DebugSerialCommunicator::println() { _serial.println(); }

DebugSerialCommunicator::DebugSerialCommunicator(Stream &serial) : _serial(serial) {}
