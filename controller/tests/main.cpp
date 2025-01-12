#include <Communicators.hpp>
#include <DiagnosticCommands.hpp>
#include <EcuMock.hpp>
#include <Parser.hpp>
#include <RequestHandler.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>

constexpr double MIN_DIFFERENCE = 100. / 255;
#define EXPECT_IN_RANGE(VAL, EXPECTED)                                                             \
  EXPECT_GE((VAL), ((EXPECTED) - MIN_DIFFERENCE));                                                 \
  EXPECT_LE((VAL), ((EXPECTED) + MIN_DIFFERENCE))

// TODO: Move Simulator to a separate file.
struct Simulator {
  DataLink dataLink{};
  StdioDebugCommunicator debugCommunicator{};
  RequestHandlerDataLinkCommunicator requestHandlerCommunicator{dataLink};
  EcuMockDataLinkCommunicator ecuCommunicator{dataLink};
  EcuMock<EcuMockDataLinkCommunicator, StdioDebugCommunicator> ecuMock{ecuCommunicator,
                                                                       debugCommunicator};
  RequestHandler<RequestHandlerDataLinkCommunicator, StdioDebugCommunicator> requestHandler{
      requestHandlerCommunicator, debugCommunicator};
  Simulator() { requestHandler.loadAvailability(); }
};

TEST(DiagnosticCommandsTest, HandlesPositiveResponse) {
  Simulator simulator{};
  std::vector<Byte> commandAvailability{0x10, 0x18, 0x80, 0x03};

  EXPECT_EQ(
      simulator.requestHandler.get<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>(),
      DiagnosticCodec<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>::decode(commandAvailability));
  EXPECT_EQ(simulator.requestHandler.get<DiagnosticCommands::ENGINE_RPM>(), 800);
  EXPECT_EQ(simulator.requestHandler.get<DiagnosticCommands::VEHICLE_SPEED>(), 10);
  auto absoluteLoad = simulator.requestHandler.get<DiagnosticCommands::ABSOLUTE_LOAD>();
  EXPECT_IN_RANGE(absoluteLoad, 10.);
  EXPECT_EQ(simulator.requestHandler.get<DiagnosticCommands::FUEL_LEVEL>(), 80);
  auto engineLoad = simulator.requestHandler.get<DiagnosticCommands::ENGINE_LOAD>();
  EXPECT_IN_RANGE(engineLoad, 15.);
  auto throttlePosition = simulator.requestHandler.get<DiagnosticCommands::THROTTLE_POSITION>();
  EXPECT_IN_RANGE(throttlePosition, 10.);
  EXPECT_EQ(simulator.requestHandler.get<DiagnosticCommands::RELATIVE_THROTTLE_POSITION>(), 20);
  EXPECT_EQ(simulator.requestHandler.get<DiagnosticCommands::ENGINE_FUEL_RATE>(), 1200);
  EXPECT_EQ(simulator.requestHandler.get<DiagnosticCommands::UPTIME>(), 15);
}

TEST(DiagnosticCommandsTest, HandlesErrors) {
  Simulator simulator{};

  ASSERT_NO_THROW(simulator.requestHandler.get<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F>());
  ASSERT_NO_THROW(simulator.requestHandler.get<DiagnosticCommands::ENGINE_RPM>());
  ASSERT_NO_THROW(simulator.requestHandler.get<DiagnosticCommands::VEHICLE_SPEED>());
  ASSERT_NO_THROW(simulator.requestHandler.get<DiagnosticCommands::ABSOLUTE_LOAD>());
  ASSERT_NO_THROW(simulator.requestHandler.get<DiagnosticCommands::FUEL_LEVEL>());
  ASSERT_NO_THROW(simulator.requestHandler.get<DiagnosticCommands::ENGINE_LOAD>());
  ASSERT_NO_THROW(simulator.requestHandler.get<DiagnosticCommands::THROTTLE_POSITION>());
  ASSERT_NO_THROW(simulator.requestHandler.get<DiagnosticCommands::RELATIVE_THROTTLE_POSITION>());
  ASSERT_NO_THROW(simulator.requestHandler.get<DiagnosticCommands::ENGINE_FUEL_RATE>());
  ASSERT_NO_THROW(simulator.requestHandler.get<DiagnosticCommands::UPTIME>());
  ASSERT_NO_THROW(simulator.requestHandler.get<DiagnosticCommands::COMMAND_AVAILABILITY_20_3F>());
}

TEST(RequestHandlerDataLinkCommunicatorTest, MethodsTest) {
  Simulator simulator{};

  ASSERT_NO_THROW(simulator.requestHandlerCommunicator.read());
  ASSERT_NO_THROW(simulator.requestHandlerCommunicator.available());
  ASSERT_NO_THROW(simulator.requestHandlerCommunicator.init());
  ASSERT_NO_THROW(simulator.requestHandlerCommunicator.write(0x01));
  ASSERT_NO_THROW(simulator.requestHandlerCommunicator.setOnNewData([]() {}));
}

TEST(RequestHandlerTest, MethodsTest) {
  Simulator simulator{};

  ASSERT_NO_THROW(simulator.requestHandler.loadAvailability());
  ASSERT_NO_THROW(simulator.requestHandler.printAvailableCommands());
  ASSERT_NO_THROW(simulator.requestHandler.printAvailableForDataFrame());
  ASSERT_NO_THROW(simulator.requestHandler.getDataFrame());
  ASSERT_NO_THROW(simulator.requestHandler.sniff());
  ASSERT_NO_THROW(simulator.requestHandler.initializeCommunication());
  ASSERT_NO_THROW(simulator.requestHandler.request({}));
  ASSERT_NO_THROW(
      simulator.requestHandler.isCommandAvailable(DiagnosticCommands::FUEL_LEVEL::value));
  ASSERT_NO_THROW(simulator.requestHandler.isCommunicationInitialized());
}

TEST(DataLinkTest, MethodsTest) {
  Simulator simulator{};

  ASSERT_NO_THROW(simulator.dataLink.controllerRead());
  ASSERT_NO_THROW(simulator.dataLink.controllerReadAvailable());
  ASSERT_NO_THROW(simulator.dataLink.controllerWrite(0x01));
  ASSERT_NO_THROW(simulator.dataLink.controllerWrite({0x01, 0x02, 0x03}));

  ASSERT_NO_THROW(simulator.dataLink.ecuRead());
  ASSERT_NO_THROW(simulator.dataLink.ecuReadAvailable());
  ASSERT_NO_THROW(simulator.dataLink.ecuWrite(0x01));
  ASSERT_NO_THROW(simulator.dataLink.ecuWrite({0x01, 0x02, 0x03}));

  ASSERT_NO_THROW(simulator.dataLink.setOnNewDataForEcu([]() {}));
}

TEST(EcuMockDataLinkCommunicator, MethodsTest) {
  Simulator simulator{};

  ASSERT_NO_THROW(simulator.ecuCommunicator.read());
  ASSERT_NO_THROW(simulator.ecuCommunicator.available());
  ASSERT_NO_THROW(simulator.ecuCommunicator.write(0x01));
  ASSERT_NO_THROW(simulator.ecuCommunicator.write({0x01, 0x02, 0x03}));
  ASSERT_NO_THROW(simulator.ecuCommunicator.init());
  ASSERT_NO_THROW(simulator.ecuCommunicator.setOnNewData([]() {}));
}

TEST(EcuMockTest, MethodsTest) {
  Simulator simulator{};
  ASSERT_NO_THROW(simulator.ecuMock.inputArrivedHandler());
}

TEST(StdioDebugCommunicatorTest, MethodsTest) {
  Simulator simulator{};

  ASSERT_NO_THROW(simulator.debugCommunicator.print(0x01));
  ASSERT_NO_THROW(simulator.debugCommunicator.print("Hello"));
  ASSERT_NO_THROW(simulator.debugCommunicator.println(0x01));
  ASSERT_NO_THROW(simulator.debugCommunicator.println("Hello"));
  ASSERT_NO_THROW(simulator.debugCommunicator.println());
}
