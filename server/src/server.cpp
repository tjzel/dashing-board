#include <DataFrame.hpp>
#include <Simulator.hpp>
#include <httplib.h>

int main() {

  httplib::Server svr;

  Simulator simulator{};
  std::mutex simulatorMutex;

  svr.Get("/", [&](const httplib::Request & /*req*/, httplib::Response &res) {
    std::lock_guard<std::mutex> lock(simulatorMutex);
    auto dataFrame = simulator.requestHandler.getDataFrame();
    auto dataFrameJson = dataFrame.toJson();
    res.set_content(dataFrameJson, "application/json");
  });

  const auto port = 1234;
  svr.listen("localhost", port);
}
