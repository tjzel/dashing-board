#include "SerialCommunicator.hpp"

byte SerialCommunicator::read() { 
  while(!_serial.available()) {}
  return _serial.read();
}

bool SerialCommunicator::available() {
  return _serial.available();
}

void SerialCommunicator::write(const std::vector<byte> &message) {
  _serial.write(message.data(), message.size());
}

void SerialCommunicator::write(byte byte) {
  _serial.write(byte);
}

SerialCommunicator::SerialCommunicator(Stream &serial): _serial(serial) {}

void DebugSerialCommunicator::print(byte byte) {
  _serial.print(byte, HEX);
}

void DebugSerialCommunicator::print(const std::string &str) {
  _serial.print(str.c_str());
}

void DebugSerialCommunicator::println(byte byte) {
  _serial.println(byte, HEX);
}

void DebugSerialCommunicator::println(const std::string &str) {
  _serial.println(str.c_str());
}

void DebugSerialCommunicator::println() {
  _serial.println();
}

DebugSerialCommunicator::DebugSerialCommunicator(Stream &serial): _serial(serial) {}
