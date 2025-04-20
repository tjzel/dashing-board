#ifndef CAN_HANDLER_HPP
#define CAN_HANDLER_HPP

#include <Arduino.h>
#include <BLEDataServer.hpp>
#include <BluetoothSerial.h>
#include <Communicators.hpp>
#include <DiagnosticCommands.hpp>
#include <HardwareSerial.h>
#include <RequestHandler.hpp>
#include <SerialCommunicator.hpp>
#include <cstdint>
#include <driver/twai.h>
#include <esp32-hal.h>
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
DebugSerialCommunicator debugSerialCommunicator(Serial);

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

  ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
  ESP_LOGI("dupsko", "Driver installed");
  ESP_ERROR_CHECK(twai_start());
  ESP_LOGI("dupsko", "Driver started");
}

const auto speedID = 673;
const auto rpmID = 292;

int lastSpeed = 0;
int lastRpm = 0;

void loop() {
  auto &server = *bleServer;

  twai_message_t rx_message;
  ESP_ERROR_CHECK_WITHOUT_ABORT(twai_receive(&rx_message, portMAX_DELAY));
  bool hasUpdated = false;
  if (rx_message.identifier == speedID) {
    lastSpeed = ((static_cast<uint32_t>(rx_message.data[1]) +
                  ((static_cast<uint32_t>(rx_message.data[0])) * 256))) /
                6.64;
    debugSerialCommunicator.println(std::to_string(micros()) + " speed " +
                                    std::to_string(lastSpeed));
    hasUpdated = true;
  } else if (rx_message.identifier == rpmID) {
    lastRpm = ((static_cast<uint32_t>(rx_message.data[2]) +
                ((static_cast<uint32_t>(rx_message.data[1])) * 256))) /
              4;
    debugSerialCommunicator.println(std::to_string(micros()) + " rpm " + std::to_string(lastRpm));
    hasUpdated = true;
  }

  if (hasUpdated) {
    DataFrame dataFrame{
        .engineRPM = lastRpm,
        .vehicleSpeed = lastSpeed,
    };
    server.update(dataFrame);
  }
}

#endif // CAN_HANDLER_HPP
