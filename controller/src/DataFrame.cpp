#include "DataFrame.hpp"
#include <sstream>

DataFrame::DataFrame(double engineLoad, int engineRPM, int vehicleSpeed,
                     double throttlePosition, int uptime, double fuelLevel,
                     double absoluteLoad, double relativeThrottlePosition,
                     double engineFuelRate)
    : engineLoad(engineLoad), engineRPM(engineRPM), vehicleSpeed(vehicleSpeed),
      throttlePosition(throttlePosition), uptime(uptime), fuelLevel(fuelLevel),
      absoluteLoad(absoluteLoad),
      relativeThrottlePosition(relativeThrottlePosition),
      engineFuelRate(engineFuelRate) {}

DataFrame::operator std::string() const {
  std::stringstream ss;
  ss << "Data frame:" << "\n";
  isValid(engineLoad) && ss << "    Engine load: " << engineLoad << "\n";
  isValid(engineRPM) && ss << "    Engine RPM: " << engineRPM << "\n";
  isValid(vehicleSpeed) && ss << "    Vehicle speed: " << vehicleSpeed << "\n";
  isValid(throttlePosition) &&
      ss << "    Throttle position: " << throttlePosition << "\n";
  isValid(uptime) && ss << "    Uptime: " << uptime << "\n";
  isValid(fuelLevel) && ss << "    Fuel level: " << fuelLevel << "\n";
  isValid(absoluteLoad) && ss << "    Absolute load: " << absoluteLoad << "\n";
  isValid(relativeThrottlePosition) && ss << "    Relative throttle position: "
                                          << relativeThrottlePosition << "\n";
  isValid(engineFuelRate) && ss << "    Engine fuel rate: " << engineFuelRate
                                << "\n";
  return ss.str();
}
