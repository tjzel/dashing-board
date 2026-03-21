#ifndef MOCK_HANDLER_HPP
#define MOCK_HANDLER_HPP

#include <BLEDataServer.hxx>
#include <Communicators.hxx>
#include <DataFrame.hxx>
#include <DataLink.hxx>
#include <DataSources.hxx>
#include <DiagnosticCommands.hxx>
#include <EcuMock.hxx>
#include <RequestHandler.hxx>
#include <SerialCommunicator.hxx>

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char *TAG_MH = "mock_handler";

DebugSerialCommunicator debugSerialCommunicator(UART_NUM_0);
DataLink dataLink;
RequestHandlerDataLinkCommunicator requestHandlerDataLinkCommunicator(dataLink);
EcuMockDataLinkCommunicator ecuMockDataLinkCommunicator(dataLink);
FakeDataProviderManager fakeDataProviderManager;
EcuMock ecuMock(ecuMockDataLinkCommunicator, debugSerialCommunicator, fakeDataProviderManager);
RequestHandler requestHandler(requestHandlerDataLinkCommunicator, debugSerialCommunicator);

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void requestHandlerInit(RequestHandler<TCommunicator, TDebugCommunicator> &requestHandler) {
  requestHandler.loadAvailability();
  ESP_LOGI(TAG_MH, "Availability loaded");
  requestHandler.printAvailableCommands();
  requestHandler.printAvailableForDataFrame();
}

BLEDataServer *bleServer = nullptr;

void setup() {
  ESP_LOGI(TAG_MH, "debugSerialCommunicator ready");

  bleServer = BLEDataServer::createServer();
  ESP_LOGI(TAG_MH, "BLE server created");

  requestHandlerInit(requestHandler);
  ESP_LOGI(TAG_MH, "Request Handler initialized");
}

void loop() {
  auto &server = *bleServer;
  auto dataFrame = requestHandler.getDataFrame();
  server.update(dataFrame);
  vTaskDelay(pdMS_TO_TICKS(100));
}

#endif // MOCK_HANDLER_HPP
