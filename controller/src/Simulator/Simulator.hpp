#include <Communicators.hpp>
#include <DataFrame.hpp>
#include <DataLink.hpp>
#include <DataSources.hpp>
#include <EcuMock.hpp>
#include <vector>

struct Simulator {
  DataLink dataLink{};
  StdioDebugCommunicator debugCommunicator{};
  RequestHandlerDataLinkCommunicator requestHandlerCommunicator{dataLink};
  EcuMockDataLinkCommunicator ecuCommunicator{dataLink};
  FakeDataProviderManager fakeDataSource{};

  // DataProvider rpmProvider_{800, 100, 1, 800, 4000};
  // DataProvider speedProvider_{10, 1, 1, 10, 140};
  // DataProvider engineLoadProvider_{15, 2, 1, 0, 100};
  // DataProvider throttlePositionProvider_{10, 5, 1, 0, 100};
  // DataProvider uptimeProvider_{15, 1, 1, 5, 60};
  // DataProvider fuelLevelProvider_{80, 2, -1, 5, 100};
  // DataProvider absoluteLoadProvider_{10, 1, 1, 0, 20};
  // DataProvider relativeThrottlePositionProvider_{20, 5, 1, 0, 90};
  // DataProvider engineFuelRateProvider_{1200, 50, 1, 100, 2500};

  // std::vector<DataFrame> createData() {
  //   std::vector<DataFrame> data;
  //   for (int i = 0; i < 1; i++) {
  //     DataFrame df;
  //     df.engineLoad = engineLoadProvider_.get();
  //     df.engineRPM = rpmProvider_.get();
  //     df.fuelLevel = fuelLevelProvider_.get();
  //     df.throttlePosition = throttlePositionProvider_.get();
  //     df.uptime = uptimeProvider_.get();
  //     df.vehicleSpeed = speedProvider_.get();
  //     df.absoluteLoad = absoluteLoadProvider_.get();
  //     df.relativeThrottlePosition = relativeThrottlePositionProvider_.get();
  //     df.engineFuelRate = engineFuelRateProvider_.get();
  //     data.push_back(df);
  //   };
  //   return data;
  // }
  // std::vector<DataFrame> data = createData();

  // TimeProvider timeProvider{};
  // RealDataProviderManager realDataSource{data, timeProvider};

  EcuMock<EcuMockDataLinkCommunicator, StdioDebugCommunicator, FakeDataProviderManager> ecuMock{
      ecuCommunicator, debugCommunicator, fakeDataSource};
  RequestHandler<RequestHandlerDataLinkCommunicator, StdioDebugCommunicator> requestHandler{
      requestHandlerCommunicator, debugCommunicator};
  Simulator() { requestHandler.loadAvailability(); }
};
