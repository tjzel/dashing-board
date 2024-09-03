#include <Arduino.h>
#include <DiagnosticCommands.hpp>
#include <SerialCommunicator.hpp>
#include <RequestHandler.hpp>
#include <iostream>

HardwareSerial sniffSerial(2);

constexpr auto sniffRx = RX2; // 16
constexpr auto sniffTx = TX2; // 17
constexpr auto analogSniffer = 26;

SerialCommunicator serialCommunicator(Serial2);
DebugSerialCommunicator debugSerialCommunicator(Serial);
RequestHandler requestHandler(serialCommunicator, debugSerialCommunicator);

void setup() {
  pinMode(sniffRx, INPUT_PULLUP);
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("outSerial ready");

  sniffSerial.begin(10400, SERIAL_8N1, sniffRx, sniffTx);
  while (!sniffSerial)
    ;
  Serial.println("sniffSerial ready");
}

void loop() {
  Serial.println("Sniffing");
  requestHandler.sniff();
}
