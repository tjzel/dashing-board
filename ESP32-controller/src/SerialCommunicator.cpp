#include <SerialCommunicator.hpp>
#include <iostream>

int SerialCommunicator::read(const size_t timeout) {
  const auto start = millis();
  while (!_serial.available() && millis() - start < timeout) {
    ;
  }
  return _serial.read();
}

bool SerialCommunicator::available() { return _serial.available(); }

void SerialCommunicator::write(const std::vector<Byte> &message) {
  for (auto byte : message) {
    _serial.write(byte);
  };
  for (auto byte : message) {
    read();
  };
}

void SerialCommunicator::write(Byte byte) {
  _serial.write(byte);
  // We need to dump the echo.
  read();
}

void SerialCommunicator::fastInit() {
  _serial.end();
  // gpio_reset_pin(_rx);
  // gpio_reset_pin(_tx);
  pinMode(_rx, INPUT_PULLUP);
  pinMode(_tx, OUTPUT);
  digitalWrite(_tx, LOW);
  delay(25);
  digitalWrite(_tx, HIGH);
  delay(25);
  // gpio_reset_pin(_tx);
  // pinMode(_rx, INPUT_PULLUP);
  _serial.begin(_baudRate, SERIAL_8N1, _rx, _tx);
};

SerialCommunicator::SerialCommunicator(const uint serialNumber, const gpio_num_t rx, const gpio_num_t tx)
    : _serialNumber(serialNumber), _rx(rx), _tx(tx), _serial(HardwareSerial(serialNumber)) {}

void DebugSerialCommunicator::print(Byte byte) { _serial.print(byte, HEX); }

void DebugSerialCommunicator::print(const std::string &str) { _serial.print(str.c_str()); }

void DebugSerialCommunicator::println(Byte byte) { _serial.println(byte, HEX); }

void DebugSerialCommunicator::println(const std::string &str) { _serial.println(str.c_str()); }

void DebugSerialCommunicator::println() { _serial.println(); }

DebugSerialCommunicator::DebugSerialCommunicator(Stream &serial) : _serial(serial) {}
