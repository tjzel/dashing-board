#ifndef COMPARATOR_HPP
#define COMPARATOR_HPP

#include <Communicators.hxx>
#include <DiagnosticCommands.hxx>
#include <RequestHandler.hxx>
#include <SerialCommunicator.hxx>

#include <cstdint>
#include <driver/twai.h>
#include <esp_intr_alloc.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <hal/twai_types.h>
#include <string>

static const char *TAG_CMP = "comparator";

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

DebugSerialCommunicator debugSerialCommunicator(UART_NUM_0);

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

static constexpr gpio_num_t kKLineRx = GPIO_NUM_25;
static constexpr gpio_num_t kKLineTx = GPIO_NUM_27;

SerialCommunicator serialCommunicator(UART_NUM_2, kKLineRx, kKLineTx, debugSerialCommunicator);
RequestHandler requestHandler(serialCommunicator, debugSerialCommunicator);

static constexpr int kMaxInitRetries = 20;
static constexpr int kInitRetryDelayMs = 500;

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void requestHandlerInit(RequestHandler<TCommunicator, TDebugCommunicator> &handler) {
  if (handler.initializeCommunication()) {
    ESP_LOGI(TAG_CMP, "ECU initialized");
  } else {
    ESP_LOGW(TAG_CMP, "ECU initialization failed, retrying...");
  }

  int retries = 0;
  while (!handler.isCommunicationInitialized() && retries < kMaxInitRetries) {
    ESP_LOGW(TAG_CMP, "Retrying ECU initialization (%d/%d)", retries + 1, kMaxInitRetries);
    handler.initializeCommunication();
    retries++;
    vTaskDelay(pdMS_TO_TICKS(kInitRetryDelayMs));
  }

  if (!handler.isCommunicationInitialized()) {
    ESP_LOGE(TAG_CMP, "ECU initialization failed after %d retries", kMaxInitRetries);
    return;
  }

  handler.loadAvailability();
  ESP_LOGI(TAG_CMP, "Availability loaded");
  handler.printAvailableCommands();
  handler.printAvailableForDataFrame();
}

void setup() {
  ESP_LOGI(TAG_CMP, "debugSerialCommunicator ready");

  ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
  ESP_LOGI(TAG_CMP, "Driver installed");
  ESP_ERROR_CHECK(twai_start());
  ESP_LOGI(TAG_CMP, "Driver started");

  requestHandlerInit(requestHandler);
  ESP_LOGI(TAG_CMP, "Request handler ready");
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
