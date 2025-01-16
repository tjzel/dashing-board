#ifndef SERIAL_HANDLER_HPP
#define SERIAL_HANDLER_HPP

#include <Arduino.h>
#include <BLEDataServer.hpp>
#include <BluetoothSerial.h>
#include <Communicators.hpp>
#include <DiagnosticCommands.hpp>
#include <HardwareSerial.h>
#include <RequestHandler.hpp>
#include <SerialCommunicator.hpp>

constexpr auto serialCommunicatorNumber = 2;

constexpr auto serialCommunicatorRx = GPIO_NUM_25;
constexpr auto serialCommunicatorTx = GPIO_NUM_27;

DebugSerialCommunicator debugSerialCommunicator(Serial);
SerialCommunicator serialCommunicator(serialCommunicatorNumber, serialCommunicatorRx,
                                      serialCommunicatorTx, debugSerialCommunicator);

RequestHandler requestHandler(serialCommunicator, debugSerialCommunicator);

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void requestHandlerInit(RequestHandler<TCommunicator, TDebugCommunicator> &requestHandler) {
  if (requestHandler.initializeCommunication()) {
    debugSerialCommunicator.println("ECU initialized");
  } else {
    debugSerialCommunicator.println("ECU initialization failed");
  }
  while (!requestHandler.isCommunicationInitialized()) {
    debugSerialCommunicator.println("Retrying ECU initialization");
    requestHandler.initializeCommunication();
  };
  // TODO: Fix this delay.
  delay(200);

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
  debugSerialCommunicator.println("debugSerialCommunicator ready");

  bleServer = BLEDataServer::createServer();
  debugSerialCommunicator.println("BLE server ready");

  requestHandlerInit(requestHandler);
  debugSerialCommunicator.println("Request handler ready");
}

void loop() {
  auto &server = *bleServer;
  auto dataFrame = requestHandler.getDataFrame();
  debugSerialCommunicator.println(dataFrame.toJson().c_str());
  server.update(dataFrame);
}

#endif // SERIAL_HANDLER_HPP
