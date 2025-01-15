#ifndef BLE_DATA_SERVER_HPP
#define BLE_DATA_SERVE_HPP

#include <BLEDescriptor.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUUID.h>
#include <BLEUtils.h>
#include <DataFrame.hpp>
#include <cstdint>
#include <sys/types.h>

class BLEDataServer {
public:
  static BLEDataServer *createServer();

  void update(const DataFrame &data);

  BLEDataServer(BLEServer &server, BLECharacteristic &updateCharacteristic,
                BLECharacteristic &engineLoadCharacteristic,
                BLECharacteristic &engineRPMCharacteristic,
                BLECharacteristic &vehicleSpeedCharacteristic,
                BLECharacteristic &throttlePositionCharacteristic,
                BLECharacteristic &uptimeCharacteristic, BLECharacteristic &fuelLevelCharacteristic,
                BLECharacteristic &absoluteLoadCharacteristic,
                BLECharacteristic &relativeThrottlePositionCharacteristic,
                BLECharacteristic &engineFuelRateCharacteristic);

private:
  void updateConnected();

  BLEServer &server_;

  // BLEService &serviceUpdate_;
  BLECharacteristic &updateCharacteristic_;
  // BLEDescriptor &updateDescriptor_;

  // BLEService &service1_;
  BLECharacteristic &engineLoadCharacteristic_;
  // BLEDescriptor &engineLoadDescriptor_;
  BLECharacteristic &engineRPMCharacteristic_;
  // BLEDescriptor &engineRpmDescriptor_;
  BLECharacteristic &vehicleSpeedCharacteristic_;
  // BLEDescriptor &vehicleSpeedDescriptor_;
  BLECharacteristic &throttlePositionCharacteristic_;
  // BLEDescriptor &throttlePositionDescriptor_;

  // BLEService &service2_;
  BLECharacteristic &uptimeCharacteristic_;
  // BLEDescriptor &uptimeDescriptor_;
  BLECharacteristic &fuelLevelCharacteristic_;
  // BLEDescriptor &fuelLevelDescriptor_;
  BLECharacteristic &absoluteLoadCharacteristic_;
  // BLEDescriptor &absoluteLoadDescriptor_;
  BLECharacteristic &relativeThrottlePositionCharacteristic_;
  // BLEDescriptor &relativeThrottlePositionDescriptor_;

  // BLEService &service3_;
  BLECharacteristic &engineFuelRateCharacteristic_;
  // BLEDescriptor &engineFuelRateDescriptor_;

  DataFrame data_ = {};
  uint16_t updateCount_ = 0;
};

#endif // BLE_DATA_SERVE_HPP
