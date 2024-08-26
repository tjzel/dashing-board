#include <Arduino.h>
// #include <RequestHandler.hpp>
#include <Requests.hpp>
#include <iostream>

HardwareSerial sniffSerial(1);

constexpr auto sniffRx = 19;
constexpr auto sniffTx = TX2;
constexpr auto analogSniffer = 26;

// RequestHandler requestHandler(Serial, sniffSerial);

size_t timeee;

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
  timeee = millis();
}

void loop() {
  // #region Requesting
  // requestHandler.request(Requests::ENGINE_RPM::value);

  // #endregion

  // #region Sniffing
  // Serial.println(analogRead(analogSniffer) / 4095.0 * 3.3);
  Serial.println("Sniffing");

  // if (requestHandler.sniff()) {
  //   const auto elapsed = millis() - timeee;
  //   timeee = millis();
  //   Serial.print("Time elapsed: ");
  //   Serial.println(elapsed);
  // } else {
  //   Serial.println("Sniffing failed");
  // }
  // #endregion
}
