#ifndef SERIAL_HANDLER_BLE_HPP
#define SERIAL_HANDLER_BLE_HPP

#include <BLEDataServer.hxx>
#include <Communicators.hxx>
#include <DiagnosticCommands.hxx>
#include <RequestHandler.hxx>
#include <SerialCommunicator.hxx>

#include <driver/gpio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char *TAG_SH = "serial_handler";

static constexpr gpio_num_t kKLineRx = GPIO_NUM_25;
static constexpr gpio_num_t kKLineTx = GPIO_NUM_27;
static constexpr int kMaxInitRetries = 20;
static constexpr int kInitRetryDelayMs = 500;

DebugSerialCommunicator debugSerialCommunicator(UART_NUM_0);
SerialCommunicator serialCommunicator(UART_NUM_2, kKLineRx, kKLineTx, debugSerialCommunicator);

RequestHandler requestHandler(serialCommunicator, debugSerialCommunicator);

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void requestHandlerInit(RequestHandler<TCommunicator, TDebugCommunicator> &handler) {
  if (handler.initializeCommunication()) {
    ESP_LOGI(TAG_SH, "ECU initialized");
  } else {
    ESP_LOGW(TAG_SH, "ECU initialization failed, retrying...");
  }

  int retries = 0;
  while (!handler.isCommunicationInitialized() && retries < kMaxInitRetries) {
    ESP_LOGW(TAG_SH, "Retrying ECU initialization (%d/%d)", retries + 1, kMaxInitRetries);
    handler.initializeCommunication();
    retries++;
    vTaskDelay(pdMS_TO_TICKS(kInitRetryDelayMs));
  }

  if (!handler.isCommunicationInitialized()) {
    ESP_LOGE(TAG_SH, "ECU initialization failed after %d retries", kMaxInitRetries);
    return;
  }

  handler.loadAvailability();
  ESP_LOGI(TAG_SH, "Availability loaded");
  handler.printAvailableCommands();
  handler.printAvailableForDataFrame();
}

BLEDataServer *bleServer = nullptr;

void setup() {
  ESP_LOGI(TAG_SH, "debugSerialCommunicator ready");

  bleServer = BLEDataServer::createServer();
  ESP_LOGI(TAG_SH, "BLE server ready");

  requestHandlerInit(requestHandler);
  ESP_LOGI(TAG_SH, "Request handler ready");
}

void loop() {
  auto &server = *bleServer;
  auto dataFrame = requestHandler.getDataFrame();
  debugSerialCommunicator.println(dataFrame.toJson());
  server.update(dataFrame);
}

#endif // SERIAL_HANDLER_BLE_HPP
