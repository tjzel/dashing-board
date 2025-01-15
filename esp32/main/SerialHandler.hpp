#ifndef SERIAL_HANDLER_HPP
#define SERIAL_HANDLER_HPP

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Communicators.hpp>
#include <DiagnosticCommands.hpp>
#include <DigitalSniffer.hpp>
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
BluetoothSerial serialBT;

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
  delay(100);

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

#endif // SERIAL_HANDLER_HPP
