#ifndef CAN_SNIFFER_HPP
#define CAN_SNIFFER_HPP

#include <Arduino.h>
#include <Communicators.hpp>
#include <HardwareSerial.h>
#include <SerialCommunicator.hpp>
#include <cstdint>
#include <driver/twai.h>
#include <esp_err.h>
#include <hal/twai_types.h>

static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
static const twai_general_config_t g_config = {
    .controller_id = 0,
    .mode = TWAI_MODE_LISTEN_ONLY,
    .tx_io = GPIO_NUM_21,
    .rx_io = GPIO_NUM_22,
    .clkout_io = TWAI_IO_UNUSED,
    .bus_off_io = TWAI_IO_UNUSED,
    .tx_queue_len = 0,
    .rx_queue_len = 5,
    .alerts_enabled = TWAI_ALERT_NONE,
    .clkout_divider = 0,
};

// constexpr auto canSerialNumber = 2;
// constexpr auto canRx = GPIO_NUM_16;
// constexpr auto canTx = GPIO_NUM_17;
// constexpr auto canBaud = 1000000;

DebugSerialCommunicator debugSerialCommunicator(Serial);
// HardwareSerial CanSerial(canSerialNumber);

void setup() {
  const auto debugSerialBaudRate = 115200;
  Serial.begin(debugSerialBaudRate);
  while (!Serial) {
    // Wait for serial port to connect.
  };
  debugSerialCommunicator.println("debugSerialCommunicator ready");

  // requestHandlerInit(requestHandler);
  // CanSerial.begin(canBaud, SERIAL_8N1, canRx, canTx);
  // while (!CanSerial) {
  //   // Wait for serial port to connect.
  // };
  // debugSerialCommunicator.println("Can serial initialized");
  ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
  ESP_LOGI("dupsko", "Driver installed");
  ESP_ERROR_CHECK(twai_start());
  ESP_LOGI("dupsko", "Driver started");
}

// constexpr uint32_t acceptedID = 288; // nie prędkość
// constexpr uint32_t acceptedID = 290; // gaz
// constexpr uint32_t acceptedID = 292; // obroty, temperatura
// constexpr uint32_t acceptedID = 304; // jakieś obroty, zużycie
// constexpr uint32_t acceptedID = 792; // gaz, hamulec dyskretnie
constexpr uint32_t acceptedID = 673; // gaz, hamulec dyskretnie

void printBinary(Byte *data, int length) {
  for (int i = 0; i < length; i++) {
    for (int j = 0; j < 8; j++) {
      debugSerialCommunicator.print(std::to_string((data[i] >> (7 - j)) & 1));
    }
  }
}
void print292(Byte *data) {
  int length = 7;
  debugSerialCommunicator.print(data[0]);
  debugSerialCommunicator.print(" ");
  debugSerialCommunicator.print(data[1]);
  debugSerialCommunicator.print(data[2]);
  debugSerialCommunicator.print(" ");
  // debugSerialCommunicator.print(std::to_string(
  // ((sta`tic_cast<uint32_t>(data[2]) + ((static_cast<uint32_t>(data[1])) * 256)) / 4)));
  debugSerialCommunicator.print(" ");
  debugSerialCommunicator.print(data[3]);
  debugSerialCommunicator.print(data[4]);
  debugSerialCommunicator.print(data[5]);
  debugSerialCommunicator.print(data[6]);
  debugSerialCommunicator.print(" ");
  printBinary(data + 3, length - 3);
  debugSerialCommunicator.println();
}

void print673(Byte *data) {
  int length = 7;
  debugSerialCommunicator.print(data[0]);
  debugSerialCommunicator.print(data[1]);
  debugSerialCommunicator.print(" ");
  debugSerialCommunicator.print(std::to_string(
      ((static_cast<uint32_t>(data[1]) + ((static_cast<uint32_t>(data[0])) * 256)))));
  debugSerialCommunicator.println();
}

void log673(Byte *data) {
  debugSerialCommunicator.print(std::to_string(
      ((static_cast<uint32_t>(data[1]) + ((static_cast<uint32_t>(data[0])) * 256)))));
  debugSerialCommunicator.print(" ");
  debugSerialCommunicator.print(std::to_string(
      ((static_cast<uint32_t>(data[2]) + ((static_cast<uint32_t>(data[3])) * 256)))));
  debugSerialCommunicator.print(" ");
  debugSerialCommunicator.print(std::to_string(
      ((static_cast<uint32_t>(data[4]) + ((static_cast<uint32_t>(data[5])) * 256)))));
  debugSerialCommunicator.print(" ");
  debugSerialCommunicator.print(std::to_string(
      ((static_cast<uint32_t>(data[6]) + ((static_cast<uint32_t>(data[7])) * 256)))));
}

void loop() {
  // if (CanSerial.available() > 0) {
  //   debugSerialCommunicator.print(CanSerial.read());
  // };
  twai_message_t rx_message;
  ESP_ERROR_CHECK_WITHOUT_ABORT(twai_receive(&rx_message, portMAX_DELAY));
  if (rx_message.identifier == acceptedID) {
    debugSerialCommunicator.print(std::to_string(rx_message.identifier));
    debugSerialCommunicator.print(" ");
    debugSerialCommunicator.print(rx_message.data_length_code);
    debugSerialCommunicator.print(" ");
    // for (int i = 0; i < rx_message.data_length_code; i++) {
    //   debugSerialCommunicator.print(rx_message.data[i]);
    // }
    // debugSerialCommunicator.println();
    // print292(rx_message.data);
    print673(rx_message.data);
  };
}

#endif // CAN_SNIFFER_HPP
