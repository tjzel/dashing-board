#ifndef DIGITAL_SNIFFER_HPP
#define DIGITAL_SNIFFER_HPP

#include <Communicators.hxx>
#include <SerialCommunicator.hxx>

#include <esp_log.h>

static const char *TAG_DS = "digital_sniffer";

class DigitalSniffer {
public:
  void sniff() {
    const auto byte = comm_.read();
    if (byte > -1) {
      debugComm_.print(byte);
    }
  }

  DigitalSniffer(SerialCommunicator &comm, DebugSerialCommunicator &debugComm)
      : comm_(comm), debugComm_(debugComm) {}

private:
  SerialCommunicator &comm_;
  DebugSerialCommunicator &debugComm_;
};

static constexpr gpio_num_t kSnifferRx = GPIO_NUM_25;
static constexpr gpio_num_t kSnifferTx = GPIO_NUM_26;

DebugSerialCommunicator debugSerialCommunicator(UART_NUM_0);
SerialCommunicator serialCommunicator(UART_NUM_2, kSnifferRx, kSnifferTx, debugSerialCommunicator);
DigitalSniffer digitalSniffer(serialCommunicator, debugSerialCommunicator);

void setup() { ESP_LOGI(TAG_DS, "debugSerialCommunicator ready"); }

void loop() { digitalSniffer.sniff(); }

#endif // DIGITAL_SNIFFER_HPP
