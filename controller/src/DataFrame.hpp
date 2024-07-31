#ifndef DATAFRAME_HPP
#define DATAFRAME_HPP
#include<string>

class DataFrame {

public:
    DataFrame(
        double engineLoad,
        int engineRPM,
        int vehicleSpeed,
        double throttlePosition,
        int uptime,
        double fuelLevel,
        double absoluteLoad,
        double relativeThrottlePosition,
        double engineFuelRate);

    [[nodiscard]] std::string toString() const;

private:
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

#endif //DATAFRAME_HPP
