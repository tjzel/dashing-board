#include <Communicators.hxx>
#include <DataFrame.hxx>
#include <DiagnosticCommands.hxx>
#include <RequestHandler.hxx>
#include <SerialCommunicator.hxx>

#include <esp_log.h>

static const char *TAG_SN = "sniffer";

static constexpr gpio_num_t kSnifferRx = GPIO_NUM_25;
static constexpr gpio_num_t kSnifferTx = GPIO_NUM_27;

DebugSerialCommunicator debugSerialCommunicator(UART_NUM_0);
SerialCommunicator serialCommunicator(UART_NUM_2, kSnifferRx, kSnifferTx, debugSerialCommunicator);
RequestHandler requestHandler(serialCommunicator, debugSerialCommunicator);

void setup() { ESP_LOGI(TAG_SN, "debugSerialCommunicator ready"); }

void loop() { requestHandler.sniff(); }
