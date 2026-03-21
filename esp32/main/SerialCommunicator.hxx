#ifndef SERIAL_COMMUNICATOR_HPP
#define SERIAL_COMMUNICATOR_HPP

#include <DiagnosticCommands.hxx>
#include <ICommunicator.hxx>

#include <cstdint>
#include <cstdio>
#include <driver/gpio.h>
#include <driver/uart.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <functional>
#include <string>
#include <vector>

using Byte = uint8_t;

class DebugSerialCommunicator {
public:
  void print(Byte byte);
  void print(const std::string &str);
  void println(Byte byte);
  void println(const std::string &str);
  void println();
  explicit DebugSerialCommunicator(uart_port_t port);

private:
  uart_port_t port_;
};

static_assert(IDebugCommunicator<DebugSerialCommunicator>);

class SerialCommunicator {
public:
  static constexpr size_t kDefaultReadTimeoutMs = 120;
  int read(size_t timeout = kDefaultReadTimeoutMs);
  bool available();
  void write(const std::vector<Byte> &message);
  void write(Byte byte);
  void init();
  void setOnNewData(const std::function<void()> &onNewData);

  explicit SerialCommunicator(uart_port_t port, gpio_num_t rx, gpio_num_t tx,
                              DebugSerialCommunicator &debugComm);

private:
  static constexpr uint32_t kBaudRate = 10400;
  static constexpr uint32_t kRequestCooldownMs = 110;

  uart_port_t port_;
  gpio_num_t rx_;
  gpio_num_t tx_;
  bool installed_ = false;
  uint32_t lastRequestMs_ = 0;

  DebugSerialCommunicator &debugComm_;

  static constexpr int64_t kUsPerMs = 1000;
  static uint32_t millisNow() {
    return static_cast<uint32_t>(esp_timer_get_time() / kUsPerMs);
  }

  void uartInstall();
  void uartUninstall();
};

static_assert(ICommunicator<SerialCommunicator>);

#endif // SERIAL_COMMUNICATOR_HPP
