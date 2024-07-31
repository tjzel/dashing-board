#include "DataFrame.hpp"
#include <sstream>

DataFrame::DataFrame(double engineLoad, int engineRPM, int vehicleSpeed, double throttlePosition, int uptime, double fuelLevel, double absoluteLoad, double relativeThrottlePosition, double engineFuelRate) : engineLoad(engineLoad), engineRPM(engineRPM), vehicleSpeed(vehicleSpeed), throttlePosition(throttlePosition), uptime(uptime), fuelLevel(fuelLevel), absoluteLoad(absoluteLoad), relativeThrottlePosition(relativeThrottlePosition), engineFuelRate(engineFuelRate) { }

std::string DataFrame::toString() const {
    std::stringstream ss;
    ss << "Data frame:" << "\n";
    ss << "    Engine load: " << engineLoad << "\n";
    ss << "    Engine RPM: " << engineRPM << "\n";
    ss << "    Vehicle speed: " << vehicleSpeed << "\n";
    ss << "    Throttle position: " << throttlePosition << "\n";
    ss << "    Uptime: " << uptime << "\n";
    ss << "    Fuel level: " << fuelLevel << "\n";
    ss << "    Absolute load: " << absoluteLoad << "\n";
    ss << "    Relative throttle position: " << relativeThrottlePosition << "\n";
    ss << "    Engine fuel rate: " << engineFuelRate << "\n";
    return ss.str();
}

