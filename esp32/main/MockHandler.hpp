#ifndef MOCK_HANDLER_HPP
#define MOCK_HANDLER_HPP

#include <Arduino.h>
#include <BLEDataServer.hpp>
#include <Communicators.hpp>
#include <DataFrame.hpp>
#include <DataLink.hpp>
#include <DataSources.hpp>
#include <DiagnosticCommands.hpp>
#include <EcuMock.hpp>
#include <HardwareSerial.h>
#include <RequestHandler.hpp>
#include <SerialCommunicator.hpp>

// BluetoothSerial serialBT;

DebugSerialCommunicator debugSerialCommunicator(Serial);
DataLink dataLink;
RequestHandlerDataLinkCommunicator requestHandlerDataLinkCommunicator(dataLink);
EcuMockDataLinkCommunicator ecuMockDataLinkCommunicator(dataLink);
FakeDataProviderManager fakeDataProviderManager;
EcuMock ecuMock(ecuMockDataLinkCommunicator, debugSerialCommunicator, fakeDataProviderManager);
RequestHandler requestHandler(requestHandlerDataLinkCommunicator, debugSerialCommunicator);

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void requestHandlerInit(RequestHandler<TCommunicator, TDebugCommunicator> &requestHandler) {
  requestHandler.loadAvailability();
  debugSerialCommunicator.println("Availability loaded");
  requestHandler.printAvailableCommands();
  requestHandler.printAvailableForDataFrame();
}

BLEDataServer *bleServer = nullptr;

void setup() {
  const auto debugSerialBaudRate = 115200;
  Serial.begin(debugSerialBaudRate);
  while (!Serial) {
    // Wait for serial port to connect.
  };
  debugSerialCommunicator.println();
  debugSerialCommunicator.println("debugSerialCommunicator ready");

  bleServer = BLEDataServer::createServer();
  debugSerialCommunicator.println("BLE server created");

  requestHandlerInit(requestHandler);
  debugSerialCommunicator.println("Request Handler initialized");
}

void loop() {
  auto &server = *bleServer;

  auto dataFrame = requestHandler.getDataFrame();
  server.update(dataFrame);
}

#endif // MOCK_HANDLER_HPP
