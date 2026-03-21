#ifndef DATA_PROVIDER_HPP
#define DATA_PROVIDER_HPP

#include <Suzuki.hxx>

class DataProvider {
public:
  int get();
  DataProvider(int initialValue, int step, int sign, int bottomThreshold, int topThreshold);

private:
  int value_;
  int step_;
  int sign_;
  int bottomThreshold_;
  int topThreshold_;
};

struct SpeedAndRPM {
  int speed;
  int rpm;
};

class SpeedAndRPMDataProvider {
public:
  SpeedAndRPM get();
  SpeedAndRPMDataProvider(int initialSpeed, int step, int sign, int bottomThreshold,
                          int topThreshold);

private:
  int calculateRPM();

  static constexpr double kKphToRpm = 1000.0 / 60.0 / Suzuki::TIRE_CIRCUMFERENCE;

  int speed_;
  int rpm_;
  int gear_ = 1;
  int step_;
  int sign_;
  int bottomThreshold_;
  int topThreshold_;
};

#endif // DATA_PROVIDER_HPP
