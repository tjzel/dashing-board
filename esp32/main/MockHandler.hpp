#ifndef MOCK_HANDLER_HPP
#define MOCK_HANDLER_HPP

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Communicators.hpp>
#include <DataFrame.hpp>
#include <DataLink.hpp>
#include <DiagnosticCommands.hpp>
#include <EcuMock.hpp>
#include <HardwareSerial.h>
#include <RequestHandler.hpp>
#include <SerialCommunicator.hpp>

BluetoothSerial serialBT;

DebugSerialCommunicator debugSerialCommunicator(Serial);
DataLink dataLink;
RequestHandlerDataLinkCommunicator requestHandlerDataLinkCommunicator(dataLink);
EcuMockDataLinkCommunicator ecuMockDataLinkCommunicator(dataLink);
EcuMock ecuMock(ecuMockDataLinkCommunicator, debugSerialCommunicator);
RequestHandler requestHandler(requestHandlerDataLinkCommunicator, debugSerialCommunicator);

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void requestHandlerInit(RequestHandler<TCommunicator, TDebugCommunicator> &requestHandler) {
  requestHandler.loadAvailability();
  debugSerialCommunicator.println("Availability loaded");
  requestHandler.printAvailableCommands();
  requestHandler.printAvailableForDataFrame();
}

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

void loop() {
  auto dataFrame = requestHandler.getDataFrame();

  serialBT.println(dataFrame.toJson().c_str());
}

#endif // MOCK_HANDLER_HPP
