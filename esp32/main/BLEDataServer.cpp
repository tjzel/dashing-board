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

const String SERVICE_UUID{"58c90e1b-3337-453a-b682-463726e00ed9"};
const String JSON_CHARACTERISTIC{"796464a0-9959-4bc3-a622-25f0dbcac388"};

// TODO: Move those somewhere else.
bool deviceConnected = false;
bool oldDeviceConnected = false;

// TODO: Refactor this.
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *server) override {
    deviceConnected = true;
    const auto id = server->getConnId();
    server->updatePeerMTU(id, 512);
    // std::cout << "Connected" << "\n";
  };
  void onDisconnect(BLEServer *server) override {
    deviceConnected = false;
    // std::cout << "Disconnected" << "\n";
  };
};

BLEDataServer *BLEDataServer::createServer() {
  BLEDevice::setMTU(512);
  BLEDevice::init(DEVICE_NAME);

  auto *server = BLEDevice::createServer();
  server->setCallbacks(new MyServerCallbacks());

  auto *service = server->createService(SERVICE_UUID);

  auto *jsonCharacteristic =
      service->createCharacteristic(JSON_CHARACTERISTIC, BLECharacteristic::PROPERTY_NOTIFY);
  auto *jsonDescriptor = new BLEDescriptor{BLEUUID(DESCRIPTION_UUID)};
  jsonDescriptor->setValue("json");
  jsonCharacteristic->setValue("{}");
  jsonCharacteristic->addDescriptor(jsonDescriptor);

  service->start();
  server->getAdvertising()->start();

  return new BLEDataServer{*server, *jsonCharacteristic};
}

void BLEDataServer::update(const DataFrame &data) {
  data_ = data;

  if (deviceConnected) {
    updateConnected();
    // TODO: Handle stack congestion here
  }

  if (!deviceConnected && oldDeviceConnected) {
    server_.getAdvertising()->start(); // restart advertising
    // TODO: Fix delay
    delay(500);
    // wait for disconnection
  }

  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}

BLEDataServer::BLEDataServer(BLEServer &server, BLECharacteristic &jsonCharacteristic)
    : server_(server), jsonCharacteristic_(jsonCharacteristic) {}

void BLEDataServer::updateConnected() {
  jsonCharacteristic_.setValue(data_.toJson().c_str());
  jsonCharacteristic_.notify();
}
