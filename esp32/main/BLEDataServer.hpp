#ifndef BLE_DATA_SERVER_HPP
#define BLE_DATA_SERVE_HPP

#include <BLEDescriptor.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEService.h>
#include <BLEUUID.h>
#include <BLEUtils.h>
#include <DataFrame.hpp>
#include <cstdint>
#include <sys/types.h>

class BLEDataServer {
public:
  static BLEDataServer *createServer();

  void update(const DataFrame &data);

  BLEDataServer(BLEServer &server, BLECharacteristic &jsonCharacteristic);

private:
  void updateConnected();

  BLEServer &server_;
  BLECharacteristic &jsonCharacteristic_;
  DataFrame data_ = {};
  uint16_t updateCount_ = 0;
};

#endif // BLE_DATA_SERVE_HPP
