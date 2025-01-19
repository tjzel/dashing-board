#include <Communicators.hpp>
#include <DataFrame.hpp>
#include <DataLink.hpp>
#include <Simulator.hpp>
#include <httplib.h>
#include <sstream>

std::string DataFrameToJson(const DataFrame &dataFrame) {
  std::stringstream stream;
  stream << "{";
  stream << "\"engineLoad\":" << dataFrame.engineLoad << ",";
  stream << "\"engineRPM\":" << dataFrame.engineRPM << ",";
  stream << "\"vehicleSpeed\":" << dataFrame.vehicleSpeed << ",";
  stream << "\"throttlePosition\":" << dataFrame.throttlePosition << ",";
  stream << "\"uptime\":" << dataFrame.uptime << ",";
  stream << "\"fuelLevel\":" << dataFrame.fuelLevel << ",";
  stream << "\"absoluteLoad\":" << dataFrame.absoluteLoad << ",";
  stream << "\"relativeThrottlePosition\":" << dataFrame.relativeThrottlePosition << ",";
  stream << "\"engineFuelRate\":" << dataFrame.engineFuelRate;
  stream << "}";
  return stream.str();
}

int main() {

  httplib::Server svr;

  Simulator simulator{};

  svr.Get("/", [&](const httplib::Request & /*req*/, httplib::Response &res) {
    auto dataFrame = simulator.requestHandler.getDataFrame();
    auto dataFrameJson = DataFrameToJson(dataFrame);
    res.set_content(dataFrameJson, "application/json");
  });

  svr.listen("localhost", 1234);
}
