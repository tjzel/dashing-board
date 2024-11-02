#ifndef SERIAL_COMMUNICATOR_HPP
#define SERIAL_COMMUNICATOR_HPP

#include <Arduino.h>
#include <DiagnosticCommands.hpp>
#include <HardwareSerial.h>
#include <ICommunicator.hpp>

using Byte = uint8_t;

class SerialCommunicator {
public:
  int read(const size_t timeout = 100);
  bool available();
  void write(const std::vector<Byte> &message);
  void write(Byte byte);
  void fastInit();
  void setOnNewData(std::function<void()> onNewData);

  explicit SerialCommunicator(const uint serialNumber, const gpio_num_t, const gpio_num_t tx);

private:
  static constexpr uint _baudRate = 10400;

  uint _serialNumber;
  gpio_num_t _rx;
  gpio_num_t _tx;
  HardwareSerial _serial;
};

static_assert(ICommunicator<SerialCommunicator>);

class DebugSerialCommunicator {
public:
  void print(Byte byte);
  void print(const std::string &str);
  void println(Byte byte);
  void println(const std::string &str);
  void println();
  explicit DebugSerialCommunicator(Stream &serial);

private:
  Stream &_serial;
};

static_assert(IDebugCommunicator<DebugSerialCommunicator>);

#endif // SERIAL_COMMUNICATOR_HPP
