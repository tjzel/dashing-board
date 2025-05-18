#ifndef COMPARATOR_HPP
#define COMPARATOR_HPP

#include "esp_intr_alloc.h"
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
#include <esp_err.h>
#include <hal/twai_types.h>

static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
static const twai_general_config_t g_config = {.controller_id = 0,
                                               .mode = TWAI_MODE_LISTEN_ONLY,
                                               .tx_io = GPIO_NUM_21,
                                               .rx_io = GPIO_NUM_22,
                                               .clkout_io = TWAI_IO_UNUSED,
                                               .bus_off_io = TWAI_IO_UNUSED,
                                               .tx_queue_len = 0,
                                               .rx_queue_len = 5,
                                               .alerts_enabled = TWAI_ALERT_NONE,
                                               .clkout_divider = 0,
                                               .intr_flags = ESP_INTR_FLAG_INTRDISABLED};

DebugSerialCommunicator debugSerialCommunicator(Serial);

constexpr uint32_t acceptedID673 = 673;
constexpr uint32_t acceptedID784 = 784;

void log673() {
  twai_message_t rx_message;
  ESP_ERROR_CHECK_WITHOUT_ABORT(twai_receive(&rx_message, portMAX_DELAY));
  while (rx_message.identifier != acceptedID673) {
    ESP_ERROR_CHECK_WITHOUT_ABORT(twai_receive(&rx_message, portMAX_DELAY));
  }
  debugSerialCommunicator.print(
      std::to_string(((static_cast<uint32_t>(rx_message.data[1]) +
                       ((static_cast<uint32_t>(rx_message.data[0])) * 256)))));
  debugSerialCommunicator.print(" ");
  debugSerialCommunicator.print(
      std::to_string(((static_cast<uint32_t>(rx_message.data[3]) +
                       ((static_cast<uint32_t>(rx_message.data[2])) * 256)))));
  debugSerialCommunicator.print(" ");
  debugSerialCommunicator.print(
      std::to_string(((static_cast<uint32_t>(rx_message.data[5]) +
                       ((static_cast<uint32_t>(rx_message.data[4])) * 256)))));
  debugSerialCommunicator.print(" ");
  debugSerialCommunicator.print(
      std::to_string(((static_cast<uint32_t>(rx_message.data[7]) +
                       ((static_cast<uint32_t>(rx_message.data[6])) * 256)))));
}

void log784() {
  twai_message_t rx_message;
  ESP_ERROR_CHECK_WITHOUT_ABORT(twai_receive(&rx_message, portMAX_DELAY));
  while (rx_message.identifier != acceptedID784) {
    ESP_ERROR_CHECK_WITHOUT_ABORT(twai_receive(&rx_message, portMAX_DELAY));
  }
  debugSerialCommunicator.print(
      std::to_string(((static_cast<uint32_t>(rx_message.data[2]) +
                       ((static_cast<uint32_t>(rx_message.data[1])) * 256)))));
}

constexpr auto serialCommunicatorNumber = 2;

constexpr auto serialCommunicatorRx = GPIO_NUM_25;
constexpr auto serialCommunicatorTx = GPIO_NUM_27;
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

  // ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
  // ESP_LOGI("dupsko", "Driver installed");
  // ESP_ERROR_CHECK(twai_start());
  // ESP_LOGI("dupsko", "Driver started");
}

void setup() {

  const auto debugSerialBaudRate = 115200;
  Serial.begin(debugSerialBaudRate);
  while (!Serial) {
    // Wait for serial port to connect.
  };
  debugSerialCommunicator.println("debugSerialCommunicator ready");

  ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
  ESP_LOGI("dupsko", "Driver installed");
  ESP_ERROR_CHECK(twai_start());
  ESP_LOGI("dupsko", "Driver started");

  requestHandlerInit(requestHandler);
  debugSerialCommunicator.println("Request handler ready");
}

void loop() {
  const auto speed = requestHandler.get<DiagnosticCommands::VEHICLE_SPEED>();
  debugSerialCommunicator.print(std::to_string(speed));
  debugSerialCommunicator.print(" ");
  log673();
  debugSerialCommunicator.print(" ");
  log784();
  debugSerialCommunicator.println();
}

#endif // COMPARATOR_HPP
