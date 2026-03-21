#include <DataProvider.hpp>

int DataProvider::get() {
  const auto value = value_;
  if (value_ <= bottomThreshold_) {
    sign_ = 1;
  } else if (value_ >= topThreshold_) {
    sign_ = -1;
  }

  value_ += sign_ * step_;
  return value;
}

DataProvider::DataProvider(int initialValue, int step, int sign, int bottomThreshold,
                           int topThreshold)
    : value_(initialValue), step_(step), sign_(sign), bottomThreshold_(bottomThreshold),
      topThreshold_(topThreshold) {}

double SpeedAndRPMDataProvider::finalGearRatio = 4.105;
double SpeedAndRPMDataProvider::firstGearRatio = 3.454;
double SpeedAndRPMDataProvider::secondGearRatio = 1.904;
double SpeedAndRPMDataProvider::thirdGearRatio = 1.28;
double SpeedAndRPMDataProvider::fourthGearRatio = 0.966;
double SpeedAndRPMDataProvider::fifthGearRatio = 0.757;
double SpeedAndRPMDataProvider::tireCircumference = 14 * 2.4 * 0.0254 * 3.14;
double SpeedAndRPMDataProvider::kphToRpm =
    (1000 / 60 / SpeedAndRPMDataProvider::tireCircumference) * 2;

SpeedAndRPMDataProvider::SpeedAndRPMDataProvider(int initialSpeed, int step, int sign,
                                                 int bottomThreshold, int topThreshold)
    : speed_(initialSpeed), rpm_(calculateRPM()), step_(step), sign_(sign),
      bottomThreshold_(bottomThreshold), topThreshold_(topThreshold) {}

SpeedAndRPM SpeedAndRPMDataProvider::get() {
  const auto speed = speed_;
  const auto rpm = calculateRPM();
  if (speed_ <= bottomThreshold_) {
    sign_ = 1;
  } else if (speed_ >= topThreshold_) {
    sign_ = -1;
  }

  speed_ += sign_ * step_;
  return {.speed = speed, .rpm = rpm};
}

int SpeedAndRPMDataProvider::calculateRPM() {
  double gearRatio = 0;
  switch (gear_) {
  case 1:
    gearRatio = firstGearRatio;
    break;
  case 2:
    gearRatio = secondGearRatio;
    break;
  case 3:
    gearRatio = thirdGearRatio;
    break;
  case 4:
    gearRatio = fourthGearRatio;
    break;
  case 5:
    gearRatio = fifthGearRatio;
    break;
  }
  gearRatio *= finalGearRatio;
  return speed_ * kphToRpm * gearRatio;
}
