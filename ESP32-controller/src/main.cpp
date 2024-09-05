#include <Arduino.h>
#include <DiagnosticCommands.hpp>
#include <EcuMock.hpp>
#include <RequestHandler.hpp>
#include <SerialCommunicator.hpp>

HardwareSerial sniffSerial(1);

// constexpr auto sniffRx = RX2; // 16
constexpr auto sniffRx = 25;
// constexpr auto sniffTx = TX2; // 17
constexpr auto sniffTx = 27;
constexpr auto analogSniffer = 26;

SerialCommunicator serialCommunicator(sniffSerial);
DebugSerialCommunicator debugSerialCommunicator(Serial);
// RequestHandler requestHandler(serialCommunicator, debugSerialCommunicator);
EcuMock ecuMock{serialCommunicator};

void setup() {
  pinMode(sniffRx, INPUT_PULLUP);
  Serial.begin(9600);
  while (!Serial)
    ;
  // Serial.println("outSerial ready");
  delay(100);
  std::cout << "outSerial ready" << std::endl;

  sniffSerial.begin(10400, SERIAL_8N1, sniffRx, sniffTx);
  while (!sniffSerial)
    ;
  delay(100);
  // Serial.println("sniffSerial ready");
  std::cout << "sniffSerial ready" << std::endl;
}

void loop() {
  // Serial.println("Sniffing");
  // serialCommunicator.write({0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f});
  // sniffSerial.write(0x01);
  // delay(100);
  // std::cout << "Sniffing" << std::endl;
  // requestHandler.sniff();
  ecuMock.inputArrivedHandler();

  // #region Analog sniffer
  // const auto read = analogRead(analogSniffer);
  // if (read < 2000) {
  //   std::cout << read << std::endl;
  // }
  // #endregion Analog sniffer
}
