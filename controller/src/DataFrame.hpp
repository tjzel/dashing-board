#ifndef DATAFRAME_HPP
#define DATAFRAME_HPP
#include <string>

// TODO: Make it generic as well.
class DataFrame {

public:
  DataFrame(double engineLoad, int engineRPM, int vehicleSpeed,
            double throttlePosition, int uptime, double fuelLevel,
            double absoluteLoad, double relativeThrottlePosition,
            double engineFuelRate);

  [[nodiscard]] std::string toString() const;

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

#endif // DATAFRAME_HPP
