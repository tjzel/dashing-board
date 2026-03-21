#ifndef SERIAL_HANDLER_DISPLAY_HPP
#define SERIAL_HANDLER_DISPLAY_HPP

#include <DiagnosticCommands.hxx>
#include <GearDetector.hxx>
#include <ILI9341Display.hxx>
#include <RequestHandler.hxx>
#include <SerialCommunicator.hxx>
#include <Suzuki.hxx>

#include <cstdio>
#include <driver/gpio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char *TAG_SHD = "serial_handler_display";

// ── K-line pins ──
static constexpr gpio_num_t kKLineRx = GPIO_NUM_25; // YELLOW
static constexpr gpio_num_t kKLineTx = GPIO_NUM_27; // BLUE
static constexpr int kMaxInitRetries = 20;
static constexpr int kInitRetryDelayMs = 500;

// ── ILI9341 pin assignments (ESP32 SPI2 / HSPI) ──
ILI9341Display tft(GPIO_NUM_5, GPIO_NUM_16, GPIO_NUM_4, GPIO_NUM_23, GPIO_NUM_18, GPIO_NUM_15);

// ── OBD2 data pipeline (real ECU via K-line) ──
DebugSerialCommunicator debugSerialCommunicator(UART_NUM_0);
SerialCommunicator serialCommunicator(UART_NUM_2, kKLineRx, kKLineTx, debugSerialCommunicator);
RequestHandler requestHandler(serialCommunicator, debugSerialCommunicator);

// ── Gear detection ──
GearDetector gearDetector(Suzuki::FINAL_GEAR_RATIO,
                          {Suzuki::GEAR_RATIOS.begin(), Suzuki::GEAR_RATIOS.end()},
                          Suzuki::TIRE_CIRCUMFERENCE);

int previousSpeed = -1;
int previousRpm = -1;
int previousGear = -1;

// ── BOOT button for orientation toggle ──
static constexpr gpio_num_t BUTTON_PIN = GPIO_NUM_0;
//   MY MX MV ML BGR MH 0 0
static constexpr uint8_t orientations[] = {
    0b00101000, // landscape normal
    0b01101000, // landscape HUD (mirrored + upside down)
};
static constexpr size_t NUM_ORIENTATIONS = sizeof(orientations) / sizeof(orientations[0]);
size_t currentOrientation = 1;
bool lastButtonState = true;

// ── Display layout constants ──

// Speed: fixed-width 3-digit, stretched vertically.
static constexpr uint8_t SPEED_SIZE_X = 12;
static constexpr uint8_t SPEED_SIZE_Y = 15;
static constexpr int16_t SPEED_Y = 35;

// RPM bar.
static constexpr int16_t RPM_BAR_X = 10;
static constexpr int16_t RPM_BAR_Y = 200;
static constexpr int16_t RPM_BAR_W = 300;
static constexpr int16_t RPM_BAR_H = 30;
static constexpr int RPM_MAX = 7000;

static constexpr uint8_t RPM_TEXT_SIZE = 2;
static constexpr int16_t RPM_TEXT_Y = RPM_BAR_Y - 7 * RPM_TEXT_SIZE - 4;

static constexpr uint16_t RPM_COLOR_LOW = 0x9CF3;  // soft grey-white
static constexpr uint16_t RPM_COLOR_MID = 0x3ECA;  // muted teal-green
static constexpr uint16_t RPM_COLOR_HIGH = 0x3B5F; // subdued blue

// Gear indicator.
static constexpr int16_t GEAR_X = 10;
static constexpr int16_t GEAR_Y = 10;

// ── Drawing helpers ──

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
  tft.drawString(x, RPM_TEXT_Y, buf, ILI9341Display::DARKGREY, ILI9341Display::BLACK,
                 RPM_TEXT_SIZE);
}

void drawRpmBar(int rpm) {
  int16_t fillW = static_cast<int32_t>(rpm) * RPM_BAR_W / RPM_MAX;
  if (fillW > RPM_BAR_W)
    fillW = RPM_BAR_W;
  if (fillW > 0)
    tft.fillRect(RPM_BAR_X, RPM_BAR_Y, fillW, RPM_BAR_H, rpmBarColor(rpm));
  int16_t emptyW = RPM_BAR_W - fillW;
  if (emptyW > 0)
    tft.fillRect(RPM_BAR_X + fillW, RPM_BAR_Y, emptyW, RPM_BAR_H, ILI9341Display::LIGHTGREY);
}

void drawGear(int gear) {
  char buf[4];
  if (gear == 0)
    snprintf(buf, sizeof(buf), "N");
  else
    snprintf(buf, sizeof(buf), "%d", gear);
  tft.drawString(GEAR_X, GEAR_Y, buf, ILI9341Display::RED, ILI9341Display::BLACK, SPEED_SIZE_X,
                 SPEED_SIZE_Y);
}

void drawSpeed(int speed) {
  char buf[8];
  snprintf(buf, sizeof(buf), "%3d", speed);
  int16_t w = tft.textWidth(buf, SPEED_SIZE_X);
  int16_t speedAreaLeft = GEAR_X + (6 * SPEED_SIZE_X) + 10;
  int16_t x = speedAreaLeft + (ILI9341Display::WIDTH - speedAreaLeft - w) / 2;
  tft.drawString(x, SPEED_Y, buf, ILI9341Display::RED, ILI9341Display::BLACK, SPEED_SIZE_X,
                 SPEED_SIZE_Y);
}

// ── ECU init with retries ──

template <ICommunicator TCommunicator, IDebugCommunicator TDebugCommunicator>
void requestHandlerInit(RequestHandler<TCommunicator, TDebugCommunicator> &handler) {
  if (handler.initializeCommunication()) {
    ESP_LOGI(TAG_SHD, "ECU initialized");
  } else {
    ESP_LOGW(TAG_SHD, "ECU initialization failed, retrying...");
  }

  int retries = 0;
  while (!handler.isCommunicationInitialized() && retries < kMaxInitRetries) {
    ESP_LOGW(TAG_SHD, "Retrying ECU initialization (%d/%d)", retries + 1, kMaxInitRetries);
    handler.initializeCommunication();
    retries++;
    vTaskDelay(pdMS_TO_TICKS(kInitRetryDelayMs));
  }

  if (!handler.isCommunicationInitialized()) {
    ESP_LOGE(TAG_SHD, "ECU initialization failed after %d retries", kMaxInitRetries);
    return;
  }

  handler.loadAvailability();
  ESP_LOGI(TAG_SHD, "Availability loaded");
  handler.printAvailableCommands();
  handler.printAvailableForDataFrame();
}

// ── Entry points ──

void setup() {
  // BOOT button as input with pull-up.
  gpio_config_t btn_conf = {};
  btn_conf.pin_bit_mask = (1ULL << BUTTON_PIN);
  btn_conf.mode = GPIO_MODE_INPUT;
  btn_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  gpio_config(&btn_conf);

  tft.begin();
  tft.fillScreen(ILI9341Display::BLACK);
  ESP_LOGI(TAG_SHD, "ILI9341 display initialized");

  drawRpmBar(0);

  requestHandlerInit(requestHandler);
  ESP_LOGI(TAG_SHD, "Request handler ready");
}

void loop() {
  // Orientation toggle on BOOT button press.
  bool buttonState = gpio_get_level(BUTTON_PIN);
  if (!buttonState && lastButtonState) {
    currentOrientation = (currentOrientation + 1) % NUM_ORIENTATIONS;
    tft.setMadctl(orientations[currentOrientation]);
    tft.fillScreen(ILI9341Display::BLACK);
    drawRpmBar(0);
    previousSpeed = -1;
    previousRpm = -1;
    previousGear = -1;
    vTaskDelay(pdMS_TO_TICKS(200));
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
}

#endif // SERIAL_HANDLER_DISPLAY_HPP
