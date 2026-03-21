#include <SerialCommunicator.hxx>

#include <array>

static constexpr size_t kUartRxBufSize = 256;
static constexpr size_t kUartTxBufSize = 0;
static constexpr int kFastInitPulseMs = 25;

// ── SerialCommunicator ──

SerialCommunicator::SerialCommunicator(uart_port_t port, gpio_num_t rx, gpio_num_t tx,
                                       DebugSerialCommunicator &debugComm)
    : port_(port), rx_(rx), tx_(tx), debugComm_(debugComm) {}

void SerialCommunicator::uartInstall() {
  if (installed_) {
    uartUninstall();
  }
  const uart_config_t cfg = {
      .baud_rate = static_cast<int>(kBaudRate),
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .rx_flow_ctrl_thresh = 0,
      .source_clk = UART_SCLK_DEFAULT,
      .flags = {},
  };
  ESP_ERROR_CHECK(uart_param_config(port_, &cfg));
  ESP_ERROR_CHECK(uart_set_pin(port_, tx_, rx_, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
  ESP_ERROR_CHECK(uart_driver_install(port_, kUartRxBufSize, kUartTxBufSize, 0, nullptr, 0));
  installed_ = true;
}

void SerialCommunicator::uartUninstall() {
  if (installed_) {
    uart_driver_delete(port_);
    installed_ = false;
  }
}

int SerialCommunicator::read(const size_t timeout) {
  uint8_t byte = 0;
  const int n = uart_read_bytes(port_, &byte, 1, pdMS_TO_TICKS(timeout));
  if (n <= 0) {
    return -1;
  }
  return byte;
}

bool SerialCommunicator::available() {
  size_t len = 0;
  uart_get_buffered_data_len(port_, &len);
  return len > 0;
}

void SerialCommunicator::write(const std::vector<Byte> &message) {
  const uint32_t now = millisNow();
  if (now - lastRequestMs_ < kRequestCooldownMs) {
    vTaskDelay(pdMS_TO_TICKS(kRequestCooldownMs - (now - lastRequestMs_)));
  }
  lastRequestMs_ = millisNow();

  uart_write_bytes(port_, message.data(), message.size());
  // Drain K-line echo (one echo byte per byte sent).
  for (size_t i = 0; i < message.size(); i++) {
    read();
  }
}

void SerialCommunicator::write(Byte byte) {
  uart_write_bytes(port_, &byte, 1);
  // Drain echo.
  read();
}

void SerialCommunicator::init() {
  uartUninstall();

  // K-line fast init: pull TX low then high for 25ms each.
  gpio_config_t tx_conf = {};
  tx_conf.pin_bit_mask = 1ULL << tx_;
  tx_conf.mode = GPIO_MODE_OUTPUT;
  gpio_config(&tx_conf);

  gpio_config_t rx_conf = {};
  rx_conf.pin_bit_mask = 1ULL << rx_;
  rx_conf.mode = GPIO_MODE_INPUT;
  rx_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  gpio_config(&rx_conf);

  gpio_set_level(tx_, 0);
  vTaskDelay(pdMS_TO_TICKS(kFastInitPulseMs));
  gpio_set_level(tx_, 1);
  vTaskDelay(pdMS_TO_TICKS(kFastInitPulseMs));

  uartInstall();
}

void SerialCommunicator::setOnNewData(const std::function<void()> & /*onNewData*/) {}

// ── DebugSerialCommunicator ──

static constexpr int kDebugBaudRate = 115200;

DebugSerialCommunicator::DebugSerialCommunicator(uart_port_t port) : port_(port) {
  const uart_config_t cfg = {
      .baud_rate = kDebugBaudRate,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .rx_flow_ctrl_thresh = 0,
      .source_clk = UART_SCLK_DEFAULT,
      .flags = {},
  };
  uart_param_config(port_, &cfg);
  uart_set_pin(port_, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
               UART_PIN_NO_CHANGE);
  // UART0 is typically already installed by ESP-IDF for console output.
  // Only install if not already present.
  if (!uart_is_driver_installed(port_)) {
    uart_driver_install(port_, kUartRxBufSize, kUartTxBufSize, 0, nullptr, 0);
  }
}

void DebugSerialCommunicator::print(Byte byte) {
  std::array<char, 3> buf{};
  snprintf(buf.data(), buf.size(), "%02X", byte);
  uart_write_bytes(port_, buf.data(), 2);
}

void DebugSerialCommunicator::print(const std::string &str) {
  uart_write_bytes(port_, str.data(), str.size());
}

void DebugSerialCommunicator::println(Byte byte) {
  print(byte);
  println();
}

void DebugSerialCommunicator::println(const std::string &str) {
  uart_write_bytes(port_, str.data(), str.size());
  println();
}

void DebugSerialCommunicator::println() {
  uart_write_bytes(port_, "\r\n", 2);
}
