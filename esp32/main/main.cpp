#include <Arduino.h>
// #include <BluetoothSerial.h>

#include <DiagnosticCommands.hpp>
#include <RequestHandler.hpp>

#include <DigitalSniffer.hpp>
#include <SerialCommunicator.hpp>

constexpr auto sniffSerialNumber = 2;

constexpr auto sniffRx = GPIO_NUM_25;
constexpr auto sniffTx = GPIO_NUM_27;
constexpr auto analogSniffer = GPIO_NUM_26;

SerialCommunicator serialCommunicator(sniffSerialNumber, sniffRx, sniffTx);
DebugSerialCommunicator debugSerialCommunicator(Serial);
DigitalSniffer digitalSniffer(serialCommunicator, debugSerialCommunicator);
// BluetoothSerial SerialBT;

// RequestHandler requestHandler(serialCommunicator, debugSerialCommunicator);
// EcuMock ecuMock{serialCommunicator, debugSerialCommunicator};

void requestHandlerInit(
    RequestHandler<SerialCommunicator, DebugSerialCommunicator> &requestHandler) {
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
  // pinMode(sniffRx, INPUT_PULLUP);
  // pinMode(sniffTx, OUTPUT);
  // Serial.begin(9600, SERIAL_8N1, sniffRx, sniffTx);

  const auto debugSerialBaudRate = 115200;
  Serial.begin(debugSerialBaudRate);
  while (!Serial) {
    // Wait for serial port to connect.
  };
  debugSerialCommunicator.println("debugSerialCommunicator ready");

  // requestHandlerInit(requestHandler);
  auto &sniffSerial = serialCommunicator.sniffInit();
  while (!sniffSerial) {
    // Wait for serial port to connect.
  }
  debugSerialCommunicator.println("Sniffing initialized");
  // SerialBT.begin("DashingBoard");
}

void loop() {
  digitalSniffer.sniff();

  // delay(250);
  // debugSerialCommunicator.println("Requesting data frame");

  // TODO: Re-initialize.

  // auto time = millis();
  // auto dataFrame = requestHandler.getDataFrame();
  // auto time2 = millis();
  // debugSerialCommunicator.println("Data frame received in " + std::to_string(time2 - time) +
  // "ms"); debugSerialCommunicator.println(std::string{dataFrame});

  // requestHandler.sniff();
  // #region Analog sniffer
  // const auto read = analogRead(analogSniffer);
  // if (read < 2000) {
  //   std::cout << read << std::endl;
  // }
  // #endregion Analog sniffer
}
