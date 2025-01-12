#ifndef DIGITAL_SNIFFER_HPP
#define DIGITAL_SNIFFER_HPP

#include <Arduino.h>
#include <Communicators.hpp>
#include <HardwareSerial.h>
#include <SerialCommunicator.hpp>

class DigitalSniffer {
public:
  void sniff() {
    const auto byte = comm_.read();
    if (byte > -1) {
      debugComm_.print(byte);
      // if (std::isalpha(static_cast<unsigned char>(byte))) {
      //   debugComm_.print(std::string{' ', static_cast<char>(byte)});
      // }
      // debugComm_.println();
    } else {
      // debugComm_.println("No data");
    }
  }

  DigitalSniffer(SerialCommunicator &comm, DebugSerialCommunicator &debugComm)
      : comm_(comm), debugComm_(debugComm) {}

private:
  SerialCommunicator &comm_;
  DebugSerialCommunicator &debugComm_;
};

constexpr auto serialCommunicatorNumber = 2;
constexpr auto serialCommunicatorRx = GPIO_NUM_25;
constexpr auto serialCommunicatorTx = GPIO_NUM_26;

DebugSerialCommunicator debugSerialCommunicator(Serial);
SerialCommunicator serialCommunicator(serialCommunicatorNumber, serialCommunicatorRx,
                                      serialCommunicatorTx, debugSerialCommunicator);
DigitalSniffer digitalSniffer(serialCommunicator, debugSerialCommunicator);

void setup() {
  const auto debugSerialBaudRate = 115200;
  Serial.begin(debugSerialBaudRate);
  while (!Serial) {
    // Wait for serial port to connect.
  };
  debugSerialCommunicator.println("debugSerialCommunicator ready");
}

void loop() { digitalSniffer.sniff(); }

#endif // DIGITAL_SNIFFER_HPP
