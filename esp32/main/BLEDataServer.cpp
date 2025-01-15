#include <BLEDataServer.hpp>
#include <BLEDescriptor.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUUID.h>
#include <BLEUtils.h>
#include <DataFrame.hpp>
#include <cstdint>
#include <sys/types.h>

const String DEVICE_NAME = "DashingBoard";

constexpr uint16_t DESCRIPTION_UUID{0x2901};

const String SERVICE_UPDATE_UUID{"58c90e1b-3337-453a-b682-463726e00ed9"};
const String UPDATE_UUID{"0fa303cb-5ec0-4672-8f60-2b7b6479f628"};

const String SERVICE_1_UUID{"8fda81a3-923d-4f40-b490-afffa3316276"};
const String ENGINE_LOAD_UUID{"085a6842-dd13-46c3-92aa-370143cf527d"};
const String ENGINE_RPM_UUID{"93dd1ddd-6f6e-4d91-8902-a7ea9af866b0"};
const String VEHICLE_SPEED_UUID{"35dc7e40-c498-48f2-bd86-3a8d8146422b"};
const String THROTTLE_POSITION_UUID{"c0d8eb64-7d61-49b0-97ca-25a9b550dd7c"};

const String SERVICE_2_UUID{"44ab3931-425b-46fe-9799-f29c69caf9a9"};
const String UPTIME_UUID{"35bd01f0-3251-4891-8e34-436ca3f589ad"};
const String FUEL_LEVEL_UUID{"906937ec-cab3-469e-864b-66c7e198b9e0"};
const String ABSOLUTE_LOAD_UUID{"5c77d475-c943-4145-afe4-b736e7304571"};
const String RELATIVE_THROTTLE_POSITION_UUID{"77dd2b02-b86b-4176-8c14-4aa05f638bcc"};

const String SERVICE_3_UUID{"3eb4dafc-586b-4ec9-aad6-70670583fcb6"};
const String ENGINE_FUEL_RATE_UUID{"451fa0ac-5e43-432a-81aa-2f1529400803"};

// 796464a0-9959-4bc3-a622-25f0dbcac388

// 58f849b7-99a8-463b-9e16-d98399166626

// 699e70c7-8677-4b7f-a9ac-6b74935b4ff8

// 579c39c5-0136-41da-9166-d6126ce14d1c

// a58b0f7e-a3cd-45ae-a2fd-bd3b124973d9

// aa8ba1fa-0877-4955-aa36-a24950a3ac61

bool deviceConnected = false;
bool oldDeviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *server) override { deviceConnected = true; };
  void onDisconnect(BLEServer *server) override { deviceConnected = false; }
};

BLEDataServer *BLEDataServer::createServer() {
  BLEDevice::init(DEVICE_NAME);
  auto *server = BLEDevice::createServer();
  server->setCallbacks(new MyServerCallbacks());

  auto *serviceUpdate = server->createService(SERVICE_UPDATE_UUID);

  auto *updateCharacteristic =
      serviceUpdate->createCharacteristic(UPDATE_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  auto *updateDescriptor = new BLEDescriptor{BLEUUID((DESCRIPTION_UUID))};
  updateDescriptor->setValue("update");
  updateCharacteristic->setValue("0");
  updateCharacteristic->addDescriptor(updateDescriptor);

  auto *service1 = server->createService(SERVICE_1_UUID);

  auto *engineLoadCharacteristic =
      service1->createCharacteristic(ENGINE_LOAD_UUID, BLECharacteristic::PROPERTY_READ);
  auto *engineLoadDescriptor = new BLEDescriptor{BLEUUID(DESCRIPTION_UUID)};
  engineLoadDescriptor->setValue("engineLoad");
  engineLoadCharacteristic->setValue("21.37");
  engineLoadCharacteristic->addDescriptor(engineLoadDescriptor);

  auto *engineRpmCharacteristic =
      service1->createCharacteristic(ENGINE_RPM_UUID, BLECharacteristic::PROPERTY_READ);
  auto *engineRpmDescriptor = new BLEDescriptor{BLEUUID(DESCRIPTION_UUID)};
  engineRpmDescriptor->setValue("engineRPM");
  engineRpmCharacteristic->setValue("2137");
  engineRpmCharacteristic->addDescriptor(engineRpmDescriptor);

  auto *vehicleSpeedCharacteristic =
      service1->createCharacteristic(VEHICLE_SPEED_UUID, BLECharacteristic::PROPERTY_READ);
  auto *vehicleSpeedDescriptor = new BLEDescriptor{BLEUUID(DESCRIPTION_UUID)};
  vehicleSpeedDescriptor->setValue("vehicleSpeed");
  vehicleSpeedCharacteristic->setValue("21");
  vehicleSpeedCharacteristic->addDescriptor(vehicleSpeedDescriptor);

  auto *throttlePositionCharacteristic =
      service1->createCharacteristic(THROTTLE_POSITION_UUID, BLECharacteristic::PROPERTY_READ);
  auto *throttlePositionDescriptor = new BLEDescriptor{BLEUUID(DESCRIPTION_UUID)};
  throttlePositionDescriptor->setValue("throttlePosition");
  throttlePositionCharacteristic->setValue("37");
  throttlePositionCharacteristic->addDescriptor(throttlePositionDescriptor);

  auto *service2 = server->createService(SERVICE_2_UUID);

  auto *uptimeCharacteristic =
      service2->createCharacteristic(UPTIME_UUID, BLECharacteristic::PROPERTY_READ);
  auto *uptimeDescriptor = new BLEDescriptor{BLEUUID(DESCRIPTION_UUID)};
  uptimeDescriptor->setValue("uptime");
  uptimeCharacteristic->setValue("2137");
  uptimeCharacteristic->addDescriptor(uptimeDescriptor);

  auto *fuelLevelCharacteristic =
      service2->createCharacteristic(FUEL_LEVEL_UUID, BLECharacteristic::PROPERTY_READ);
  auto *fuelLevelDescriptor = new BLEDescriptor{BLEUUID(DESCRIPTION_UUID)};
  fuelLevelDescriptor->setValue("fuelLevel");
  fuelLevelCharacteristic->setValue("21.37");
  fuelLevelCharacteristic->addDescriptor(fuelLevelDescriptor);

  auto *absoluteLoadCharacteristic =
      service2->createCharacteristic(ABSOLUTE_LOAD_UUID, BLECharacteristic::PROPERTY_READ);
  auto *absoluteLoadDescriptor = new BLEDescriptor{BLEUUID(DESCRIPTION_UUID)};
  absoluteLoadDescriptor->setValue("absoluteLoad");
  absoluteLoadCharacteristic->setValue("2137");
  absoluteLoadCharacteristic->addDescriptor(absoluteLoadDescriptor);

  auto *relativeThrottlePositionCharacteristic = service2->createCharacteristic(
      RELATIVE_THROTTLE_POSITION_UUID, BLECharacteristic::PROPERTY_READ);
  auto *relativeThrottlePositionDescriptor = new BLEDescriptor{BLEUUID(DESCRIPTION_UUID)};
  relativeThrottlePositionDescriptor->setValue("relativeThrottlePosition");
  relativeThrottlePositionCharacteristic->setValue("37");
  relativeThrottlePositionCharacteristic->addDescriptor(relativeThrottlePositionDescriptor);

  auto *service3 = server->createService(SERVICE_3_UUID);

  auto *engineFuelRateCharacteristic =
      service3->createCharacteristic(ENGINE_FUEL_RATE_UUID, BLECharacteristic::PROPERTY_READ);
  auto *engineFuelRateDescriptor = new BLEDescriptor{BLEUUID(DESCRIPTION_UUID)};
  engineFuelRateDescriptor->setValue("engineFuelRate");
  engineFuelRateCharacteristic->setValue("2137");
  engineFuelRateCharacteristic->addDescriptor(engineFuelRateDescriptor);

  serviceUpdate->start();
  service1->start();
  service2->start();
  service3->start();

  server->getAdvertising()->start();

  return new BLEDataServer{*server,
                           *updateCharacteristic,
                           *engineLoadCharacteristic,
                           *engineRpmCharacteristic,
                           *vehicleSpeedCharacteristic,
                           *throttlePositionCharacteristic,
                           *uptimeCharacteristic,
                           *fuelLevelCharacteristic,
                           *absoluteLoadCharacteristic,
                           *relativeThrottlePositionCharacteristic,
                           *engineFuelRateCharacteristic};
}

void BLEDataServer::update(const DataFrame &data) {
  data_ = data;

  if (deviceConnected) {
    updateConnected();
    // add delay to prevent stack congestion
  }

  if (!deviceConnected && oldDeviceConnected) {
    server_.getAdvertising()->start(); // restart advertising
    // delay(500);
    // wait for disconnection
  }

  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}

BLEDataServer::BLEDataServer(BLEServer &server, BLECharacteristic &updateCharacteristic,
                             BLECharacteristic &engineLoadCharacteristic,
                             BLECharacteristic &engineRPMCharacteristic,
                             BLECharacteristic &vehicleSpeedCharacteristic,
                             BLECharacteristic &throttlePositionCharacteristic,
                             BLECharacteristic &uptimeCharacteristic,
                             BLECharacteristic &fuelLevelCharacteristic,
                             BLECharacteristic &absoluteLoadCharacteristic,
                             BLECharacteristic &relativeThrottlePositionCharacteristic,
                             BLECharacteristic &engineFuelRateCharacteristic)
    : server_(server), updateCharacteristic_(updateCharacteristic),
      engineLoadCharacteristic_(engineLoadCharacteristic),
      engineRPMCharacteristic_(engineRPMCharacteristic),
      vehicleSpeedCharacteristic_(vehicleSpeedCharacteristic),
      throttlePositionCharacteristic_(throttlePositionCharacteristic),
      uptimeCharacteristic_(uptimeCharacteristic),
      fuelLevelCharacteristic_(fuelLevelCharacteristic),
      absoluteLoadCharacteristic_(absoluteLoadCharacteristic),
      relativeThrottlePositionCharacteristic_(relativeThrottlePositionCharacteristic),
      engineFuelRateCharacteristic_(engineFuelRateCharacteristic) {}

void BLEDataServer::updateConnected() {

  engineLoadCharacteristic_.setValue(std::to_string(data_.engineLoad).c_str());
  engineRPMCharacteristic_.setValue(std::to_string(data_.engineRPM).c_str());
  vehicleSpeedCharacteristic_.setValue(std::to_string(data_.vehicleSpeed).c_str());
  throttlePositionCharacteristic_.setValue(std::to_string(data_.throttlePosition).c_str());
  uptimeCharacteristic_.setValue(std::to_string(data_.uptime).c_str());
  fuelLevelCharacteristic_.setValue(std::to_string(data_.fuelLevel).c_str());
  absoluteLoadCharacteristic_.setValue(std::to_string(data_.absoluteLoad).c_str());
  relativeThrottlePositionCharacteristic_.setValue(
      std::to_string(data_.relativeThrottlePosition).c_str());
  engineFuelRateCharacteristic_.setValue(std::to_string(data_.engineFuelRate).c_str());

  updateCharacteristic_.setValue(std::to_string(updateCount_++).c_str());
  updateCharacteristic_.notify();
}
