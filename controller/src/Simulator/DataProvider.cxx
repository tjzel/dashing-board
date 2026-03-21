#include <DataProvider.hxx>

#include <algorithm>
#include <iostream>

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

SpeedAndRPMDataProvider::SpeedAndRPMDataProvider(int initialSpeed, int step, int sign,
                                                 int bottomThreshold, int topThreshold)
    : speed_(initialSpeed), rpm_(0), step_(step), sign_(sign), bottomThreshold_(bottomThreshold),
      topThreshold_(topThreshold) {}

SpeedAndRPM SpeedAndRPMDataProvider::get() {
  const auto speed = speed_;

  // Step speed.
  speed_ += sign_ * step_;
  speed_ = std::max(speed_, 0);

  // Derive RPM from speed and current gear.
  rpm_ = static_cast<int>(speed_ * kKphToRpm * Suzuki::GEAR_RATIOS.at(gear_ - 1) *
                          Suzuki::FINAL_GEAR_RATIO);

  std::cout << "Speed: " << speed_ << " km/h, RPM: " << rpm_ << ", Gear: " << gear_ << std::endl;

  // Shift up at 6000 RPM.
  if (rpm_ >= 6000 && gear_ < 5) {
    gear_++;
    rpm_ = static_cast<int>(speed_ * kKphToRpm * Suzuki::GEAR_RATIOS.at(gear_ - 1) *
                            Suzuki::FINAL_GEAR_RATIO);
  }

  // Shift down at 2000 RPM.
  if (rpm_ <= 2000 && gear_ > 1) {
    gear_--;
    rpm_ = static_cast<int>(speed_ * kKphToRpm * Suzuki::GEAR_RATIOS.at(gear_ - 1) *
                            Suzuki::FINAL_GEAR_RATIO);
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
