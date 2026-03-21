#ifndef ILI9341_DISPLAY_HPP
#define ILI9341_DISPLAY_HPP

#include <array>
#include <cstdint>
#include <cstring>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Minimal ILI9341 driver using ESP-IDF SPI master driver.
// Only implements what we need: fill, rect, and text rendering.

class ILI9341Display {
public:
  static constexpr uint16_t WIDTH = 320;
  static constexpr uint16_t HEIGHT = 240;

  static constexpr uint16_t BLACK = 0x0000;
  static constexpr uint16_t RED = 0xF800;
  static constexpr uint16_t GREEN = 0x07E0;
  static constexpr uint16_t WHITE = 0xFFFF;
  static constexpr uint16_t BLUE = 0x001F;
  static constexpr uint16_t DARKGREY = 0x4208;
  static constexpr uint16_t LIGHTGREY = 0xC618;

  ILI9341Display(gpio_num_t cs, gpio_num_t dc, gpio_num_t rst, gpio_num_t mosi, gpio_num_t sclk,
                 gpio_num_t bl)
      : cs_(cs), dc_(dc), rst_(rst), mosi_(mosi), sclk_(sclk), bl_(bl) {}

  void begin() {
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << dc_) | (1ULL << rst_) | (1ULL << bl_);
    io_conf.mode = GPIO_MODE_OUTPUT;
    gpio_config(&io_conf);

    gpio_set_level(bl_, 1);

    gpio_set_level(rst_, 1);
    vTaskDelay(pdMS_TO_TICKS(kResetSetupMs));
    gpio_set_level(rst_, 0);
    vTaskDelay(pdMS_TO_TICKS(kResetPulseMs));
    gpio_set_level(rst_, 1);
    vTaskDelay(pdMS_TO_TICKS(kResetRecoveryMs));

    spi_bus_config_t buscfg = {};
    buscfg.mosi_io_num = mosi_;
    buscfg.miso_io_num = -1;
    buscfg.sclk_io_num = sclk_;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = WIDTH * HEIGHT * 2;
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg = {};
    devcfg.clock_speed_hz = kSpiClockHz;
    devcfg.mode = 0;
    devcfg.spics_io_num = cs_;
    devcfg.queue_size = kSpiQueueSize;
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi_));

    writeCommand(kCmdSwReset);
    vTaskDelay(pdMS_TO_TICKS(kResetRecoveryMs));
    writeCommand(kCmdSleepOut);
    vTaskDelay(pdMS_TO_TICKS(kSleepOutMs));

    writeCommand(kCmdPixelFormat);
    writeData(kPixelFormat16Bit);

    writeCommand(kCmdMadctl);
    writeData(kMadctlLandscape);

    writeCommand(kCmdDisplayOn);
    vTaskDelay(pdMS_TO_TICKS(kDisplayOnMs));
  }

  void fillScreen(uint16_t colour) { fillRect(0, 0, WIDTH, HEIGHT, colour); }

  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colour) {
    if (x >= WIDTH || y >= HEIGHT || w <= 0 || h <= 0) {
      return;
    }

    setAddrWindow(x, y, x + w - 1, y + h - 1);

    const auto rowPixels = static_cast<size_t>(w);
    const size_t rowBytes = rowPixels * 2;
    auto *buf = static_cast<uint16_t *>(heap_caps_malloc(rowBytes, MALLOC_CAP_DMA));
    const uint16_t swapped = __builtin_bswap16(colour);
    for (size_t i = 0; i < rowPixels; i++) {
      buf[i] = swapped; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }

    gpio_set_level(dc_, 1);
    for (int16_t row = 0; row < h; row++) {
      spi_transaction_t t = {};
      t.length = rowBytes * kBitsPerByte;
      t.tx_buffer = buf;
      spi_device_polling_transmit(spi_, &t);
    }

    heap_caps_free(buf);
  }

  void drawChar(int16_t x, int16_t y, char c, uint16_t fg, uint16_t bg, uint8_t sizeX,
                uint8_t sizeY) {
    if (c < kPrintableFirst || c > kPrintableLast) {
      c = '?';
    }
    for (uint8_t col = 0; col < kFontCharWidth; col++) {
      const uint8_t line =
          (col <
           kFontGlyphWidth) // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
              ? font5x7[c - kPrintableFirst][col]
              : 0x00;
      for (uint8_t row = 0; row < kFontHeight; row++) {
        const uint16_t colour = ((line & (1 << row)) != 0) ? fg : bg;
        if (sizeX == 1 && sizeY == 1) {
          drawPixel(static_cast<int16_t>(x + col), static_cast<int16_t>(y + row), colour);
        } else {
          fillRect(static_cast<int16_t>(x + (col * sizeX)), static_cast<int16_t>(y + (row * sizeY)),
                   sizeX, sizeY, colour);
        }
      }
    }
  }

  void drawChar(int16_t x, int16_t y, char c, uint16_t fg, uint16_t bg, uint8_t size) {
    drawChar(x, y, c, fg, bg, size, size);
  }

  void drawString(int16_t x, int16_t y, const char *str, uint16_t fg, uint16_t bg, uint8_t sizeX,
                  uint8_t sizeY) {
    while (*str != '\0') {
      drawChar(x, y, *str, fg, bg, sizeX, sizeY);
      x = static_cast<int16_t>(x + (kFontCharWidth * sizeX));
      str++; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }
  }

  void drawString(int16_t x, int16_t y, const char *str, uint16_t fg, uint16_t bg, uint8_t size) {
    drawString(x, y, str, fg, bg, size, size);
  }

  static int16_t textWidth(const char *str, uint8_t size) {
    const auto len = static_cast<int16_t>(strlen(str));
    if (len == 0) {
      return 0;
    }
    return static_cast<int16_t>((len * kFontCharWidth * size) - size);
  }

  void setMadctl(uint8_t madctl) {
    writeCommand(kCmdMadctl);
    writeData(madctl);
  }

private:
  static constexpr int kResetSetupMs = 5;
  static constexpr int kResetPulseMs = 20;
  static constexpr int kResetRecoveryMs = 150;
  static constexpr int kSleepOutMs = 500;
  static constexpr int kDisplayOnMs = 100;
  static constexpr int kSpiClockHz = 26'000'000;
  static constexpr int kSpiQueueSize = 7;
  static constexpr int kBitsPerByte = 8;

  static constexpr uint8_t kCmdSwReset = 0x01;
  static constexpr uint8_t kCmdSleepOut = 0x11;
  static constexpr uint8_t kCmdDisplayOn = 0x29;
  static constexpr uint8_t kCmdMadctl = 0x36;
  static constexpr uint8_t kCmdPixelFormat = 0x3A;
  static constexpr uint8_t kCmdColumnAddrSet = 0x2A;
  static constexpr uint8_t kCmdPageAddrSet = 0x2B;
  static constexpr uint8_t kCmdMemWrite = 0x2C;
  static constexpr uint8_t kPixelFormat16Bit = 0x55;
  static constexpr uint8_t kMadctlLandscape = 0b01101000;

  static constexpr uint8_t kFontGlyphWidth = 5;
  static constexpr uint8_t kFontCharWidth = 6;
  static constexpr uint8_t kFontHeight = 7;
  static constexpr char kPrintableFirst = 32;
  static constexpr char kPrintableLast = 126;

  gpio_num_t cs_, dc_, rst_, mosi_, sclk_, bl_;
  spi_device_handle_t spi_{};

  void writeCommand(uint8_t cmd) {
    gpio_set_level(dc_, 0);
    spi_transaction_t t = {};
    t.length = kBitsPerByte;
    t.tx_buffer = &cmd;
    spi_device_polling_transmit(spi_, &t);
  }

  void writeData(uint8_t data) {
    gpio_set_level(dc_, 1);
    spi_transaction_t t = {};
    t.length = kBitsPerByte;
    t.tx_buffer = &data;
    spi_device_polling_transmit(spi_, &t);
  }

  void writeData16(uint16_t val) {
    uint16_t swapped = __builtin_bswap16(val);
    gpio_set_level(dc_, 1);
    spi_transaction_t t = {};
    t.length = kBitsPerByte * 2;
    t.tx_buffer = &swapped;
    spi_device_polling_transmit(spi_, &t);
  }

  void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(kCmdColumnAddrSet);
    writeData16(x0);
    writeData16(x1);
    writeCommand(kCmdPageAddrSet);
    writeData16(y0);
    writeData16(y1);
    writeCommand(kCmdMemWrite);
  }

  void drawPixel(int16_t x, int16_t y, uint16_t colour) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
      return;
    }
    setAddrWindow(x, y, x, y);
    writeData16(colour);
  }

  // clang-format off
  static constexpr std::array<std::array<uint8_t, kFontGlyphWidth>, 95> font5x7 = {{
    {0x00,0x00,0x00,0x00,0x00}, // ' '
    {0x00,0x00,0x5F,0x00,0x00}, // '!'
    {0x00,0x07,0x00,0x07,0x00}, // '"'
    {0x14,0x7F,0x14,0x7F,0x14}, // '#'
    {0x24,0x2A,0x7F,0x2A,0x12}, // '$'
    {0x23,0x13,0x08,0x64,0x62}, // '%'
    {0x36,0x49,0x55,0x22,0x50}, // '&'
    {0x00,0x05,0x03,0x00,0x00}, // '''
    {0x00,0x1C,0x22,0x41,0x00}, // '('
    {0x00,0x41,0x22,0x1C,0x00}, // ')'
    {0x14,0x08,0x3E,0x08,0x14}, // '*'
    {0x08,0x08,0x3E,0x08,0x08}, // '+'
    {0x00,0x50,0x30,0x00,0x00}, // ','
    {0x08,0x08,0x08,0x08,0x08}, // '-'
    {0x00,0x60,0x60,0x00,0x00}, // '.'
    {0x20,0x10,0x08,0x04,0x02}, // '/'
    {0x3E,0x51,0x49,0x45,0x3E}, // '0'
    {0x00,0x42,0x7F,0x40,0x00}, // '1'
    {0x42,0x61,0x51,0x49,0x46}, // '2'
    {0x21,0x41,0x45,0x4B,0x31}, // '3'
    {0x18,0x14,0x12,0x7F,0x10}, // '4'
    {0x27,0x45,0x45,0x45,0x39}, // '5'
    {0x3C,0x4A,0x49,0x49,0x30}, // '6'
    {0x01,0x71,0x09,0x05,0x03}, // '7'
    {0x36,0x49,0x49,0x49,0x36}, // '8'
    {0x06,0x49,0x49,0x29,0x1E}, // '9'
    {0x00,0x36,0x36,0x00,0x00}, // ':'
    {0x00,0x56,0x36,0x00,0x00}, // ';'
    {0x08,0x14,0x22,0x41,0x00}, // '<'
    {0x14,0x14,0x14,0x14,0x14}, // '='
    {0x00,0x41,0x22,0x14,0x08}, // '>'
    {0x02,0x01,0x51,0x09,0x06}, // '?'
    {0x32,0x49,0x79,0x41,0x3E}, // '@'
    {0x7E,0x11,0x11,0x11,0x7E}, // 'A'
    {0x7F,0x49,0x49,0x49,0x36}, // 'B'
    {0x3E,0x41,0x41,0x41,0x22}, // 'C'
    {0x7F,0x41,0x41,0x22,0x1C}, // 'D'
    {0x7F,0x49,0x49,0x49,0x41}, // 'E'
    {0x7F,0x09,0x09,0x09,0x01}, // 'F'
    {0x3E,0x41,0x49,0x49,0x7A}, // 'G'
    {0x7F,0x08,0x08,0x08,0x7F}, // 'H'
    {0x00,0x41,0x7F,0x41,0x00}, // 'I'
    {0x20,0x40,0x41,0x3F,0x01}, // 'J'
    {0x7F,0x08,0x14,0x22,0x41}, // 'K'
    {0x7F,0x40,0x40,0x40,0x40}, // 'L'
    {0x7F,0x02,0x0C,0x02,0x7F}, // 'M'
    {0x7F,0x04,0x08,0x10,0x7F}, // 'N'
    {0x3E,0x41,0x41,0x41,0x3E}, // 'O'
    {0x7F,0x09,0x09,0x09,0x06}, // 'P'
    {0x3E,0x41,0x51,0x21,0x5E}, // 'Q'
    {0x7F,0x09,0x19,0x29,0x46}, // 'R'
    {0x46,0x49,0x49,0x49,0x31}, // 'S'
    {0x01,0x01,0x7F,0x01,0x01}, // 'T'
    {0x3F,0x40,0x40,0x40,0x3F}, // 'U'
    {0x1F,0x20,0x40,0x20,0x1F}, // 'V'
    {0x3F,0x40,0x38,0x40,0x3F}, // 'W'
    {0x63,0x14,0x08,0x14,0x63}, // 'X'
    {0x07,0x08,0x70,0x08,0x07}, // 'Y'
    {0x61,0x51,0x49,0x45,0x43}, // 'Z'
    {0x00,0x7F,0x41,0x41,0x00}, // '['
    {0x02,0x04,0x08,0x10,0x20}, // '\'
    {0x00,0x41,0x41,0x7F,0x00}, // ']'
    {0x04,0x02,0x01,0x02,0x04}, // '^'
    {0x40,0x40,0x40,0x40,0x40}, // '_'
    {0x00,0x01,0x02,0x04,0x00}, // '`'
    {0x20,0x54,0x54,0x54,0x78}, // 'a'
    {0x7F,0x48,0x44,0x44,0x38}, // 'b'
    {0x38,0x44,0x44,0x44,0x20}, // 'c'
    {0x38,0x44,0x44,0x48,0x7F}, // 'd'
    {0x38,0x54,0x54,0x54,0x18}, // 'e'
    {0x08,0x7E,0x09,0x01,0x02}, // 'f'
    {0x0C,0x52,0x52,0x52,0x3E}, // 'g'
    {0x7F,0x08,0x04,0x04,0x78}, // 'h'
    {0x00,0x44,0x7D,0x40,0x00}, // 'i'
    {0x20,0x40,0x44,0x3D,0x00}, // 'j'
    {0x7F,0x10,0x28,0x44,0x00}, // 'k'
    {0x00,0x41,0x7F,0x40,0x00}, // 'l'
    {0x7C,0x04,0x18,0x04,0x78}, // 'm'
    {0x7C,0x08,0x04,0x04,0x78}, // 'n'
    {0x38,0x44,0x44,0x44,0x38}, // 'o'
    {0x7C,0x14,0x14,0x14,0x08}, // 'p'
    {0x08,0x14,0x14,0x18,0x7C}, // 'q'
    {0x7C,0x08,0x04,0x04,0x08}, // 'r'
    {0x48,0x54,0x54,0x54,0x20}, // 's'
    {0x04,0x3F,0x44,0x40,0x20}, // 't'
    {0x3C,0x40,0x40,0x20,0x7C}, // 'u'
    {0x1C,0x20,0x40,0x20,0x1C}, // 'v'
    {0x3C,0x40,0x30,0x40,0x3C}, // 'w'
    {0x44,0x28,0x10,0x28,0x44}, // 'x'
    {0x0C,0x50,0x50,0x50,0x3C}, // 'y'
    {0x44,0x64,0x54,0x4C,0x44}, // 'z'
    {0x00,0x08,0x36,0x41,0x00}, // '{'
    {0x00,0x00,0x7F,0x00,0x00}, // '|'
    {0x00,0x41,0x36,0x08,0x00}, // '}'
    {0x10,0x08,0x08,0x10,0x08}, // '~'
  }};
  // clang-format on
};

#endif // ILI9341_DISPLAY_HPP
