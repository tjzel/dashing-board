#ifndef CAN_SNIFFER_HPP
#define CAN_SNIFFER_HPP

#include <Communicators.hxx>
#include <SerialCommunicator.hxx>

#include <cstdint>
#include <driver/twai.h>
#include <esp_err.h>
#include <esp_log.h>
#include <hal/twai_types.h>
#include <string>

static const char *TAG_CS = "can_sniffer";

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

DebugSerialCommunicator debugSerialCommunicator(UART_NUM_0);

void setup() {
  ESP_LOGI(TAG_CS, "debugSerialCommunicator ready");

  ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
  ESP_LOGI(TAG_CS, "Driver installed");
  ESP_ERROR_CHECK(twai_start());
  ESP_LOGI(TAG_CS, "Driver started");
}

constexpr uint32_t acceptedID = 673;

void printBinary(Byte *data, int length) {
  for (int i = 0; i < length; i++) {
    for (int j = 0; j < 8; j++) {
      debugSerialCommunicator.print(std::to_string((data[i] >> (7 - j)) & 1));
    }
  }
}

void print292(Byte *data) {
  debugSerialCommunicator.print(data[0]);
  debugSerialCommunicator.print(" ");
  debugSerialCommunicator.print(data[1]);
  debugSerialCommunicator.print(data[2]);
  debugSerialCommunicator.print(" ");
  debugSerialCommunicator.print(" ");
  debugSerialCommunicator.print(data[3]);
  debugSerialCommunicator.print(data[4]);
  debugSerialCommunicator.print(data[5]);
  debugSerialCommunicator.print(data[6]);
  debugSerialCommunicator.print(" ");
  printBinary(data + 3, 4);
  debugSerialCommunicator.println();
}

void print673(Byte *data) {
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
  twai_message_t rx_message;
  ESP_ERROR_CHECK_WITHOUT_ABORT(twai_receive(&rx_message, portMAX_DELAY));
  if (rx_message.identifier == acceptedID) {
    debugSerialCommunicator.print(std::to_string(rx_message.identifier));
    debugSerialCommunicator.print(" ");
    debugSerialCommunicator.print(rx_message.data_length_code);
    debugSerialCommunicator.print(" ");
    print673(rx_message.data);
  };
}

#endif // CAN_SNIFFER_HPP
