#include <DataProvider.hpp>
#include <algorithm>

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
    : speed_(initialSpeed), rpm_(0), step_(step), sign_(sign), bottomThreshold_(bottomThreshold),
      topThreshold_(topThreshold) {}

SpeedAndRPM SpeedAndRPMDataProvider::get() {
  static const double gearRatios[] = {firstGearRatio, secondGearRatio, thirdGearRatio,
                                      fourthGearRatio, fifthGearRatio};

  const auto speed = speed_;

  // Step speed.
  speed_ += sign_ * step_;
  speed_ = std::max(speed_, 0);

  // Derive RPM from speed and current gear.
  rpm_ = speed_ * kphToRpm * gearRatios[gear_ - 1] * finalGearRatio;

  // Shift up at 6000 RPM.
  if (rpm_ >= 6000 && gear_ < 5) {
    gear_++;
    rpm_ = speed_ * kphToRpm * gearRatios[gear_ - 1] * finalGearRatio;
  }

  // Shift down at 2000 RPM.
  if (rpm_ <= 2000 && gear_ > 1) {
    gear_--;
    rpm_ = speed_ * kphToRpm * gearRatios[gear_ - 1] * finalGearRatio;
  }

  // Bounce at limits.
  if (speed_ <= 0) {
    speed_ = 0;
    gear_ = 1;
    sign_ = 1;
  } else if (rpm_ >= 6500 && gear_ >= 5) {
    sign_ = -1;
  }

  return {.speed = speed, .rpm = rpm_};
}

int SpeedAndRPMDataProvider::calculateRPM() { return rpm_; }
