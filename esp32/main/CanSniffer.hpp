#ifndef CAN_SNIFFER_HPP
#define CAN_SNIFFER_HPP

#include <Arduino.h>
#include <Communicators.hpp>
#include <HardwareSerial.h>
#include <SerialCommunicator.hpp>

constexpr auto canSerialNumber = 2;
constexpr auto canRx = GPIO_NUM_16;
constexpr auto canTx = GPIO_NUM_17;
constexpr auto canBaud = 1000000;

DebugSerialCommunicator debugSerialCommunicator(Serial);
HardwareSerial CanSerial(canSerialNumber);

void setup() {
  const auto debugSerialBaudRate = 115200;
  Serial.begin(debugSerialBaudRate);
  while (!Serial) {
    // Wait for serial port to connect.
  };
  debugSerialCommunicator.println("debugSerialCommunicator ready");

  requestHandlerInit(requestHandler);
  CanSerial.begin(canBaud, SERIAL_8N1, canRx, canTx);
  while (!CanSerial) {
    // Wait for serial port to connect.
  };
  debugSerialCommunicator.println("Can serial initialized");
}

void loop() {
  if (CanSerial.available() > 0) {
    debugSerialCommunicator.print(CanSerial.read());
  };
}

#endif // CAN_SNIFFER_HPP
