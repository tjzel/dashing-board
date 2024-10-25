#include <Arduino.h>
#include <DiagnosticCommands.hpp>
#include <EcuMock.hpp>
#include <RequestHandler.hpp>
#include <SerialCommunicator.hpp>

constexpr auto sniffSerialNumber = 1;

constexpr auto sniffRx = GPIO_NUM_25;
constexpr auto sniffTx = GPIO_NUM_27;
constexpr auto analogSniffer = GPIO_NUM_26;

SerialCommunicator serialCommunicator(sniffSerialNumber, sniffRx, sniffTx);
DebugSerialCommunicator debugSerialCommunicator(Serial);

RequestHandler requestHandler(serialCommunicator, debugSerialCommunicator);
// EcuMock ecuMock{serialCommunicator, debugSerialCommunicator};

void setup() {
  pinMode(sniffRx, INPUT_PULLUP);
  Serial.begin(9600);
  while (!Serial)
    ;
  debugSerialCommunicator.println("debugSerialCommunicator ready");

  // if (requestHandler.initializeCommunication()) {
  //   debugSerialCommunicator.println("ECU initialized");
  // } else {
  //   debugSerialCommunicator.println("ECU initialization failed");
  // }
  while (!requestHandler.isCommunicationInitialized()) {
    requestHandler.initializeCommunication();
  };

  delay(100);

  requestHandler.loadAvailability();
  debugSerialCommunicator.println("Availability loaded");
  requestHandler.printAvailableCommands();
  requestHandler.printAvailableForDataFrame();
}

void loop() {

  // TODO: Re-initialize.

  // auto time = millis();
  // auto dataFrame = requestHandler.getDataFrame();
  // auto time2 = millis();
  // debugSerialCommunicator.println("Data frame received in " + std::to_string(time2 - time) + " ms");
  // debugSerialCommunicator.println(std::string{dataFrame});

  // requestHandler.sniff();
  // #region Analog sniffer
  // const auto read = analogRead(analogSniffer);
  // if (read < 2000) {
  //   std::cout << read << std::endl;
  // }
  // #endregion Analog sniffer
}
