#include <Arduino.h>
#include <Communicators.hpp>
#include <DataFrame.hpp>
#include <DiagnosticCommands.hpp>
#include <RequestHandler.hpp>
#include <SerialCommunicator.hpp>

constexpr auto serialCommunicatorNumber = 2;
constexpr auto serialCommunicatorRx = GPIO_NUM_25;
constexpr auto serialCommunicatorTx = GPIO_NUM_27;

DebugSerialCommunicator debugSerialCommunicator(Serial);
SerialCommunicator serialCommunicator(sniffSerialNumber, sniffRx, sniffTx, debugSerialCommunicator);
RequestHandler requestHandler(serialCommunicator, debugSerialCommunicator);

void setup() {
  const auto debugSerialBaudRate = 115200;
  Serial.begin(debugSerialBaudRate);
  while (!Serial) {
    // Wait for serial port to connect.
  };
  debugSerialCommunicator.println("debugSerialCommunicator ready");

  requestHandlerInit(requestHandler);
  serialBT.begin("DashingBoard");
}

void loop() { requestHandler.sniff(); }
