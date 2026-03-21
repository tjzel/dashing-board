#ifndef DATA_PROVIDER_HPP
#define DATA_PROVIDER_HPP

#include <utility>
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

  static double finalGearRatio;   // = 4.105;
  static double reverseGearRatio; // = 3.270
  static double firstGearRatio;   // = 3.454;
  static double secondGearRatio;  // = 1.904;
  static double thirdGearRatio;   // = 1.28;
  static double fourthGearRatio;  // = 0.966;
  static double fifthGearRatio;   // = 0.757;

  static double tireCircumference; // = 14 * 2.4 * 0.0254 * Math.PI;
  static double kphToRpm;          // = (1000 / 60 / tireCircumference) * 2;

  int speed_;
  int rpm_;
  int gear_ = 1;
  int step_;
  int sign_;
  int bottomThreshold_;
  int topThreshold_;
};

#endif // DATA_PROVIDER_HPP
