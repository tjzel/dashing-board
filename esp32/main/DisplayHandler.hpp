#ifndef DISPLAY_HANDLER_HPP
#define DISPLAY_HANDLER_HPP

#include <Communicators.hpp>
#include <DataFrame.hpp>
#include <DataLink.hpp>
#include <DataSources.hpp>
#include <DiagnosticCommands.hpp>
#include <EcuMock.hpp>
#include <ILI9341Display.hpp>
#include <RequestHandler.hpp>
#include <SerialCommunicator.hpp>

#include <cstdio>
#include <esp_log.h>

static const char *TAG = "display";

// ── ILI9341 pin assignments (ESP32 SPI2 / HSPI) ──
//   CS  = GPIO 5
//   DC  = GPIO 16
//   RST = GPIO 4
//   SCK = GPIO 18, MOSI = GPIO 23, BL = GPIO 15
ILI9341Display tft(GPIO_NUM_5, GPIO_NUM_16, GPIO_NUM_4, GPIO_NUM_23, GPIO_NUM_18, GPIO_NUM_15);

// ── Data pipeline (same as MockHandler) ──
DebugSerialCommunicator debugSerialCommunicator(Serial);
DataLink dataLink;
RequestHandlerDataLinkCommunicator requestHandlerDataLinkCommunicator(dataLink);
EcuMockDataLinkCommunicator ecuMockDataLinkCommunicator(dataLink);
FakeDataProviderManager fakeDataProviderManager;
EcuMock ecuMock(ecuMockDataLinkCommunicator, debugSerialCommunicator, fakeDataProviderManager);
RequestHandler requestHandler(requestHandlerDataLinkCommunicator, debugSerialCommunicator);

int previousSpeed = -1;
char previousBuf[8] = {};
int16_t previousX = 0;

// BOOT button on GPIO 0 (active low, has internal pull-up).
static constexpr gpio_num_t BUTTON_PIN = GPIO_NUM_0;
//   MY MX MV ML BGR MH 0 0
static constexpr uint8_t orientations[] = {
    0b00101000, // landscape normal
    0b01101000, // landscape HUD (mirrored + upside down)
};
static constexpr size_t NUM_ORIENTATIONS = sizeof(orientations) / sizeof(orientations[0]);
size_t currentOrientation = 0;
bool lastButtonState = true; // pulled high when not pressed

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void requestHandlerInit(RequestHandler<TCommunicator, TDebugCommunicator> &handler) {
  handler.loadAvailability();
  debugSerialCommunicator.println("Availability loaded");
  handler.printAvailableCommands();
  handler.printAvailableForDataFrame();
}

void drawSpeedLabel() {
  const char *label = "km/h";
  int16_t w = tft.textWidth(label, 3);
  tft.drawString((ILI9341Display::WIDTH - w) / 2, 195, label, ILI9341Display::LIGHTGREY,
                 ILI9341Display::BLACK, 3);
}

void drawSpeed(int speed) {
  // If the digit count changed, clear the old region first to handle width
  // differences (e.g. "99" -> "100"). Otherwise drawString paints both fg and
  // bg pixels in one pass — no flicker.
  if (previousBuf[0] != '\0' && strlen(previousBuf) != 0) {
    int16_t oldW = tft.textWidth(previousBuf, 10);
    if (oldW > tft.textWidth("000", 10)) // safety cap
      oldW = tft.textWidth("000", 10);
    int16_t newBufW;
    {
      char tmp[8];
      snprintf(tmp, sizeof(tmp), "%d", speed);
      newBufW = tft.textWidth(tmp, 10);
    }
    if (oldW != newBufW) {
      tft.fillRect(previousX, 60, oldW, 70, ILI9341Display::BLACK);
    }
  }

  char buf[8];
  snprintf(buf, sizeof(buf), "%d", speed);
  int16_t w = tft.textWidth(buf, 10);
  int16_t x = (ILI9341Display::WIDTH - w) / 2;
  tft.drawString(x, 60, buf, ILI9341Display::RED, ILI9341Display::BLACK, 10);

  memcpy(previousBuf, buf, sizeof(previousBuf));
  previousX = x;
}

void setup() {
  const auto debugSerialBaudRate = 115200;
  Serial.begin(debugSerialBaudRate);
  while (!Serial) {
  };
  debugSerialCommunicator.println();
  debugSerialCommunicator.println("debugSerialCommunicator ready");

  // BOOT button as input with pull-up.
  gpio_config_t btn_conf = {};
  btn_conf.pin_bit_mask = (1ULL << BUTTON_PIN);
  btn_conf.mode = GPIO_MODE_INPUT;
  btn_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  gpio_config(&btn_conf);

  tft.begin();
  tft.fillScreen(ILI9341Display::BLACK);
  ESP_LOGI(TAG, "ILI9341 display initialized");

  drawSpeedLabel();

  requestHandlerInit(requestHandler);
  ESP_LOGI(TAG, "Request Handler initialized");
}

void loop() {
  // Check BOOT button for orientation toggle (active low, debounced by press edge).
  bool buttonState = gpio_get_level(BUTTON_PIN);
  if (!buttonState && lastButtonState) {
    currentOrientation = (currentOrientation + 1) % NUM_ORIENTATIONS;
    tft.setMadctl(orientations[currentOrientation]);
    tft.fillScreen(ILI9341Display::BLACK);
    drawSpeedLabel();
    previousBuf[0] = '\0';
    previousSpeed = -1;
    vTaskDelay(pdMS_TO_TICKS(200)); // simple debounce
  }
  lastButtonState = buttonState;

  auto dataFrame = requestHandler.getDataFrame();
  int speed = dataFrame.vehicleSpeed;

  if (speed != previousSpeed) {
    drawSpeed(speed);
    previousSpeed = speed;
  }

  // vTaskDelay(pdMS_TO_TICKS(200));
}

#endif // DISPLAY_HANDLER_HPP
