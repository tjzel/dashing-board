#include <DataFrame.hpp>
#include <sstream>

DataFrame::operator std::string() const {
  std::stringstream stream;
  stream << "Data frame:" << "\n";
  isValid(engineLoad) && stream << "    Engine load: " << engineLoad << "\n";
  isValid(engineRPM) && stream << "    Engine RPM: " << engineRPM << "\n";
  isValid(vehicleSpeed) && stream << "    Vehicle speed: " << vehicleSpeed << "\n";
  isValid(throttlePosition) && stream << "    Throttle position: " << throttlePosition << "\n";
  isValid(uptime) && stream << "    Uptime: " << uptime << "\n";
  isValid(fuelLevel) && stream << "    Fuel level: " << fuelLevel << "\n";
  isValid(absoluteLoad) && stream << "    Absolute load: " << absoluteLoad << "\n";
  isValid(relativeThrottlePosition) && stream << "    Relative throttle position: " << relativeThrottlePosition << "\n";
  isValid(engineFuelRate) && stream << "    Engine fuel rate: " << engineFuelRate << "\n";
  return stream.str();
}
