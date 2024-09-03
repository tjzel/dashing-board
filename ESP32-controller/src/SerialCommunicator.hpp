#ifndef SERIAL_COMMUNICATOR_HPP
#define SERIAL_COMMUNICATOR_HPP

#include <Arduino.h>
#include <DiagnosticCommands.hpp>
#include <ICommunicator.hpp>
#include <string>

class SerialCommunicator {
public:
  byte read();
  bool available();
  void write(const std::vector<byte> &message);
  void write(byte byte);
  SerialCommunicator(Stream &serial);

private:

  Stream &_serial;
};

static_assert(ICommunicator<SerialCommunicator>);

class DebugSerialCommunicator {
public:
  void print(byte byte);
  void print(const std::string &str);
  void println(byte byte);
  void println(const std::string &str);
  void println();
  DebugSerialCommunicator(Stream &serial);

private:
  Stream &_serial;
};

static_assert(IDebugCommunicator<DebugSerialCommunicator>);

#endif // SERIAL_COMMUNICATOR_HPP

