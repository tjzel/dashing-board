#ifndef BLE_DATA_SERVER_HPP
#define BLE_DATA_SERVER_HPP

#include <DataFrame.hxx>

#include <cstdint>
#include <host/ble_hs.h>
#include <string>

class BLEDataServer {
public:
  static BLEDataServer *createServer();

  void update(const DataFrame &data);

  BLEDataServer(uint16_t connHandle, uint16_t jsonAttrHandle);

private:
  uint16_t connHandle_;
  uint16_t jsonAttrHandle_;
  bool connected_ = false;

  friend int bleGapEvent(struct ble_gap_event *event, void *arg);
};

#endif // BLE_DATA_SERVER_HPP
