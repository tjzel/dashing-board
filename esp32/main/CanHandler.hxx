#ifndef CAN_HANDLER_HPP
#define CAN_HANDLER_HPP

#include <BLEDataServer.hxx>
#include <Communicators.hxx>
#include <DataFrame.hxx>
#include <DiagnosticCommands.hxx>
#include <SerialCommunicator.hxx>

#include <cstdint>
#include <driver/twai.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <hal/twai_types.h>
#include <string>

static const char *TAG_CH = "can_handler";

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

BLEDataServer *bleServer = nullptr;

void setup() {
  ESP_LOGI(TAG_CH, "debugSerialCommunicator ready");

  bleServer = BLEDataServer::createServer();
  ESP_LOGI(TAG_CH, "BLE server ready");

  ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
  ESP_LOGI(TAG_CH, "Driver installed");
  ESP_ERROR_CHECK(twai_start());
  ESP_LOGI(TAG_CH, "Driver started");
}

static constexpr uint32_t speedID = 673;
static constexpr uint32_t rpmID = 292;

int lastSpeed = 0;
int lastRpm = 0;

void loop() {
  auto &server = *bleServer;

  twai_message_t rx_message;
  ESP_ERROR_CHECK_WITHOUT_ABORT(twai_receive(&rx_message, portMAX_DELAY));
  bool hasUpdated = false;
  if (rx_message.identifier == speedID) {
    lastSpeed = static_cast<int>((static_cast<uint32_t>(rx_message.data[1]) +
                                  (static_cast<uint32_t>(rx_message.data[0]) * 256)) /
                                 6.64);
    uint64_t now = esp_timer_get_time();
    debugSerialCommunicator.println(std::to_string(now) + " speed " + std::to_string(lastSpeed));
    hasUpdated = true;
  } else if (rx_message.identifier == rpmID) {
    lastRpm = static_cast<int>((static_cast<uint32_t>(rx_message.data[2]) +
                                (static_cast<uint32_t>(rx_message.data[1]) * 256)) /
                               4);
    uint64_t now = esp_timer_get_time();
    debugSerialCommunicator.println(std::to_string(now) + " rpm " + std::to_string(lastRpm));
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
