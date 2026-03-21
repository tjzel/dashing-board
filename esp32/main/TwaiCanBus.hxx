#ifndef TWAI_CAN_BUS_HPP
#define TWAI_CAN_BUS_HPP

#include <ICanBus.hxx>

#include <driver/gpio.h>
#include <driver/twai.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <hal/twai_types.h>

class TwaiCanBus {
public:
  TwaiCanBus(gpio_num_t tx, gpio_num_t rx, twai_mode_t mode = TWAI_MODE_NORMAL)
      : tx_(tx), rx_(rx), mode_(mode) {}

  void init() {
    const twai_filter_config_t f = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    const twai_timing_config_t t = TWAI_TIMING_CONFIG_500KBITS();
    const twai_general_config_t g = {
        .controller_id = 0,
        .mode = mode_,
        .tx_io = tx_,
        .rx_io = rx_,
        .clkout_io = TWAI_IO_UNUSED,
        .bus_off_io = TWAI_IO_UNUSED,
        .tx_queue_len = 5,
        .rx_queue_len = 10,
        .alerts_enabled = TWAI_ALERT_NONE,
        .clkout_divider = 0,
        .intr_flags = 0,
    };
    ESP_ERROR_CHECK(twai_driver_install(&g, &t, &f));
    ESP_ERROR_CHECK(twai_start());
  }

  static bool send(const CanFrame &frame) {
    twai_message_t msg = {};
    msg.identifier = frame.id;
    msg.data_length_code = frame.dlc;
    for (uint8_t i = 0; i < frame.dlc && i < CAN_FRAME_MAX_BYTES; i++) {
      msg.data[i] = frame.data.at(i);
    }
    return twai_transmit(&msg, pdMS_TO_TICKS(100)) == ESP_OK;
  }

  static CanFrame receive(uint32_t timeoutMs) {
    twai_message_t msg = {};
    if (twai_receive(&msg, pdMS_TO_TICKS(timeoutMs)) != ESP_OK) {
      return {};
    }
    CanFrame result;
    result.id = msg.identifier;
    result.dlc = msg.data_length_code;
    for (uint8_t i = 0; i < msg.data_length_code && i < CAN_FRAME_MAX_BYTES; i++) {
      result.data.at(i) = msg.data[i];
    }
    return result;
  }

  static CanFrame receiveFrom(uint32_t identifier, uint32_t timeoutMs) {
    // Drain frames until we find one matching the requested ID or timeout.
    // In practice OBD2 responses arrive within a few frames.
    CanFrame frame;
    static constexpr int MAX_DRAIN = 32;
    for (int i = 0; i < MAX_DRAIN; i++) {
      frame = receive(timeoutMs);
      if (frame.id == identifier) {
        return frame;
      }
      if (frame.id == 0 && frame.dlc == 0) {
        break; // timeout
      }
    }
    return {};
  }

private:
  gpio_num_t tx_;
  gpio_num_t rx_;
  twai_mode_t mode_;
};

static_assert(ICanBus<TwaiCanBus>);

#endif // TWAI_CAN_BUS_HPP
