#ifndef DISPLAY_HANDLER_HPP
#define DISPLAY_HANDLER_HPP

#include <Communicators.hxx>
#include <DataFrame.hxx>
#include <DataLink.hxx>
#include <DataSources.hxx>
#include <DiagnosticCommands.hxx>
#include <EcuMock.hxx>
#include <ILI9341Display.hxx>
#include <RequestHandler.hxx>
#include <SerialCommunicator.hxx>

#include <GearDetector.hxx>

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
DebugSerialCommunicator debugSerialCommunicator(UART_NUM_0);
DataLink dataLink;
RequestHandlerDataLinkCommunicator requestHandlerDataLinkCommunicator(dataLink);
EcuMockDataLinkCommunicator ecuMockDataLinkCommunicator(dataLink);
FakeDataProviderManager fakeDataProviderManager;
EcuMock ecuMock(ecuMockDataLinkCommunicator, debugSerialCommunicator, fakeDataProviderManager);
RequestHandler requestHandler(requestHandlerDataLinkCommunicator, debugSerialCommunicator);

int previousSpeed = -1;
int previousRpm = -1;
int previousGear = -1;

// Gear ratios from DataProvider.hxx simulator.
// tireCircumference = 14 * 2.4 * 0.0254 * π, radius = circumference / (2π)
static constexpr double TIRE_RADIUS = 14 * 2.4 * 0.0254 / 2.0;
GearDetector gearDetector(4.105, {3.454, 1.904, 1.28, 0.966, 0.757}, TIRE_RADIUS);

// BOOT button on GPIO 0 (active low, has internal pull-up).
static constexpr gpio_num_t BUTTON_PIN = GPIO_NUM_0;
//   MY MX MV ML BGR MH 0 0
static constexpr uint8_t orientations[] = {
    0b00101000, // landscape normal
    0b01101000, // landscape HUD (mirrored + upside down)
};
static constexpr size_t NUM_ORIENTATIONS = sizeof(orientations) / sizeof(orientations[0]);
size_t currentOrientation = 1;
bool lastButtonState = true; // pulled high when not pressed

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void requestHandlerInit(RequestHandler<TCommunicator, TDebugCommunicator> &handler) {
  handler.loadAvailability();
  debugSerialCommunicator.println("Availability loaded");
  handler.printAvailableCommands();
  handler.printAvailableForDataFrame();
}

// Fixed-width 3-digit speed display — stretched vertically.
static constexpr uint8_t SPEED_SIZE_X = 12;
static constexpr uint8_t SPEED_SIZE_Y = 15;
static constexpr int16_t SPEED_Y = 35;

// RPM bar layout.
static constexpr int16_t RPM_BAR_X = 10;
static constexpr int16_t RPM_BAR_Y = 200;
static constexpr int16_t RPM_BAR_W = 300;
static constexpr int16_t RPM_BAR_H = 30;
static constexpr int RPM_MAX = 7000;

static constexpr uint8_t RPM_TEXT_SIZE = 2;
static constexpr int16_t RPM_TEXT_Y = RPM_BAR_Y - 7 * RPM_TEXT_SIZE - 4;

// Muted bar colors — less harsh than pure RGB.
static constexpr uint16_t RPM_COLOR_LOW = 0x9CF3;  // soft grey-white
static constexpr uint16_t RPM_COLOR_MID = 0x3ECA;  // muted teal-green
static constexpr uint16_t RPM_COLOR_HIGH = 0x3B5F; // subdued blue

uint16_t rpmBarColor(int rpm) {
  if (rpm > 3000)
    return RPM_COLOR_HIGH;
  if (rpm > 2000)
    return RPM_COLOR_MID;
  return RPM_COLOR_LOW;
}

void drawRpmText(int rpm) {
  char buf[8];
  snprintf(buf, sizeof(buf), "%5d", rpm);
  int16_t w = tft.textWidth(buf, RPM_TEXT_SIZE);
  int16_t x = RPM_BAR_X + RPM_BAR_W - w;
  tft.drawString(x, RPM_TEXT_Y, buf, ILI9341Display::DARKGREY, ILI9341Display::BLACK, RPM_TEXT_SIZE);
}

void drawRpmBar(int rpm) {
  int16_t fillW = (int32_t)rpm * RPM_BAR_W / RPM_MAX;
  if (fillW > RPM_BAR_W)
    fillW = RPM_BAR_W;
  if (fillW > 0)
    tft.fillRect(RPM_BAR_X, RPM_BAR_Y, fillW, RPM_BAR_H, rpmBarColor(rpm));
  int16_t emptyW = RPM_BAR_W - fillW;
  if (emptyW > 0)
    tft.fillRect(RPM_BAR_X + fillW, RPM_BAR_Y, emptyW, RPM_BAR_H, ILI9341Display::LIGHTGREY);
}

// Gear indicator — top-left corner, small.
static constexpr uint8_t GEAR_SIZE = SPEED_SIZE_X;
static constexpr int16_t GEAR_X = 10;
static constexpr int16_t GEAR_Y = 10;

void drawGear(int gear) {
  char buf[4];
  if (gear == 0)
    snprintf(buf, sizeof(buf), "N");
  else
    snprintf(buf, sizeof(buf), "%d", gear);
  tft.drawString(GEAR_X, GEAR_Y, buf, ILI9341Display::RED, ILI9341Display::BLACK, SPEED_SIZE_X, SPEED_SIZE_Y);
}

void drawSpeed(int speed) {
  // Right-justify within 3 chars so digit positions never shift.
  char buf[8];
  snprintf(buf, sizeof(buf), "%3d", speed);

  int16_t w = tft.textWidth(buf, SPEED_SIZE_X);
  // Offset right so speed doesn't overlap the gear indicator.
  int16_t speedAreaLeft = GEAR_X + (6 * SPEED_SIZE_X) + 10;
  int16_t x = speedAreaLeft + (ILI9341Display::WIDTH - speedAreaLeft - w) / 2;
  tft.drawString(x, SPEED_Y, buf, ILI9341Display::RED, ILI9341Display::BLACK, SPEED_SIZE_X, SPEED_SIZE_Y);
}

void setup() {
  ESP_LOGI(TAG, "debugSerialCommunicator ready");

  // BOOT button as input with pull-up.
  gpio_config_t btn_conf = {};
  btn_conf.pin_bit_mask = (1ULL << BUTTON_PIN);
  btn_conf.mode = GPIO_MODE_INPUT;
  btn_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  gpio_config(&btn_conf);

  tft.begin();
  tft.fillScreen(ILI9341Display::BLACK);
  ESP_LOGI(TAG, "ILI9341 display initialized");

  drawRpmBar(0);

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
    drawRpmBar(0);
    previousSpeed = -1;
    previousRpm = -1;
    previousGear = -1;
    vTaskDelay(pdMS_TO_TICKS(200)); // simple debounce
  }
  lastButtonState = buttonState;

  auto dataFrame = requestHandler.getDataFrame();
  int speed = dataFrame.vehicleSpeed;
  int rpm = dataFrame.engineRPM;
  int gear = gearDetector.detect(rpm, speed);

  if (speed != previousSpeed) {
    drawSpeed(speed);
    previousSpeed = speed;
  }

  if (rpm != previousRpm) {
    drawRpmText(rpm);
    drawRpmBar(rpm);
    previousRpm = rpm;
  }

  if (gear != previousGear) {
    drawGear(gear);
    previousGear = gear;
  }

  // vTaskDelay(pdMS_TO_TICKS(200));
}

#endif // DISPLAY_HANDLER_HPP
