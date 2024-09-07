#ifndef DATA_FRAME_HPP
#define DATA_FRAME_HPP
#include <string>

class DataFrame {

public:
  DataFrame(double engineLoad, int engineRPM, int vehicleSpeed,
            double throttlePosition, int uptime, double fuelLevel,
            double absoluteLoad, double relativeThrottlePosition,
            double engineFuelRate);

  explicit operator std::string() const;

private:
  template <class TNumeric> static bool isValid(TNumeric value) {
    return value >= 0;
  }

  double engineLoad;
  int engineRPM;
  int vehicleSpeed;
  double throttlePosition;
  int uptime;

  double fuelLevel;

  double absoluteLoad;
  double relativeThrottlePosition;
  double engineFuelRate;
};

#endif // DATA_FRAME_HPP