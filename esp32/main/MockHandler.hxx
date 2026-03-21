#ifndef MOCK_HANDLER_HPP
#define MOCK_HANDLER_HPP

#include <Arduino.h>
#include <BLEDataServer.hxx>
#include <Communicators.hxx>
#include <DataFrame.hxx>
#include <DataLink.hxx>
#include <DataSources.hxx>
#include <DiagnosticCommands.hxx>
#include <EcuMock.hxx>
#include <HardwareSerial.h>
#include <RequestHandler.hxx>
#include <SerialCommunicator.hxx>

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
  delay(100);
}

#endif // MOCK_HANDLER_HPP
