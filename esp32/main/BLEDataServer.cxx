#include <BLEDataServer.hxx>

#include <cstring>
#include <esp_log.h>
#include <esp_nimble_hci.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <host/ble_hs.h>
#include <host/ble_uuid.h>
#include <host/util/util.h>
#include <nimble/nimble_port.h>
#include <nimble/nimble_port_freertos.h>
#include <services/gap/ble_svc_gap.h>
#include <services/gatt/ble_svc_gatt.h>

static const char *TAG_BLE = "ble_server";

static const char *DEVICE_NAME = "DashingBoard";

// Service UUID: 58c90e1b-3337-453a-b682-463726e00ed9
static const ble_uuid128_t SERVICE_UUID = BLE_UUID128_INIT(
    0xd9, 0x0e, 0xe0, 0x26, 0x37, 0x46, 0x82, 0xb6, 0x3a, 0x45, 0x37, 0x33, 0x1b, 0x0e, 0xc9, 0x58);

// JSON Characteristic UUID: 796464a0-9959-4bc3-a622-25f0dbcac388
static const ble_uuid128_t JSON_CHAR_UUID = BLE_UUID128_INIT(
    0x88, 0xc3, 0xca, 0xdb, 0xf0, 0x25, 0x22, 0xa6, 0xc3, 0x4b, 0x59, 0x99, 0xa0, 0x64, 0x64, 0x79);

static uint16_t gJsonAttrHandle = 0;
static BLEDataServer *gServer = nullptr;

static int jsonCharAccess(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg) {
  if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
    const char *val = "{}";
    os_mbuf_append(ctxt->om, val, strlen(val));
  }
  return 0;
}

// clang-format off
static const struct ble_gatt_chr_def gattChars[] = {
    {
        .uuid = &JSON_CHAR_UUID.u,
        .access_cb = jsonCharAccess,
        .arg = nullptr,
        .descriptors = nullptr,
        .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
        .min_key_size = 0,
        .val_handle = &gJsonAttrHandle,
    },
    {0}, // sentinel
};

static const struct ble_gatt_svc_def gattSvcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &SERVICE_UUID.u,
        .includes = nullptr,
        .characteristics = gattChars,
    },
    {0}, // sentinel
};
// clang-format on

int bleGapEvent(struct ble_gap_event *event, void *arg) {
  auto *server = static_cast<BLEDataServer *>(arg);
  switch (event->type) {
  case BLE_GAP_EVENT_CONNECT:
    if (event->connect.status == 0) {
      server->connHandle_ = event->connect.conn_handle;
      server->connected_ = true;
      // Request higher MTU for JSON payloads.
      ble_att_set_preferred_mtu(512);
      ble_gattc_exchange_mtu(event->connect.conn_handle, nullptr, nullptr);
      ESP_LOGI(TAG_BLE, "Client connected");
    }
    break;
  case BLE_GAP_EVENT_DISCONNECT:
    server->connected_ = false;
    ESP_LOGI(TAG_BLE, "Client disconnected, restarting advertising");
    // Restart advertising.
    BLEDataServer::createServer();
    break;
  case BLE_GAP_EVENT_MTU:
    ESP_LOGI(TAG_BLE, "MTU updated to %d", event->mtu.value);
    break;
  default:
    break;
  }
  return 0;
}

static void startAdvertising() {
  struct ble_gap_adv_params adv_params = {};
  adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
  adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

  struct ble_hs_adv_fields fields = {};
  fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
  fields.tx_pwr_lvl_is_present = 1;
  fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
  fields.name = reinterpret_cast<const uint8_t *>(DEVICE_NAME);
  fields.name_len = strlen(DEVICE_NAME);
  fields.name_is_complete = 1;

  ble_gap_adv_set_fields(&fields);
  ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, nullptr, BLE_HS_FOREVER, &adv_params, bleGapEvent,
                    gServer);
}

static void bleHostTask(void *param) {
  nimble_port_run();
  nimble_port_freertos_deinit();
}

static void onSync() {
  ble_hs_id_infer_auto(0, nullptr);
  startAdvertising();
}

BLEDataServer *BLEDataServer::createServer() {
  if (gServer != nullptr) {
    // Already initialized, just restart advertising.
    startAdvertising();
    return gServer;
  }

  nimble_port_init();

  ble_svc_gap_init();
  ble_svc_gatt_init();
  ble_gatts_count_cfg(gattSvcs);
  ble_gatts_add_svcs(gattSvcs);

  ble_svc_gap_device_name_set(DEVICE_NAME);
  ble_hs_cfg.sync_cb = onSync;

  gServer = new BLEDataServer(0, gJsonAttrHandle);
  nimble_port_freertos_init(bleHostTask);

  return gServer;
}

void BLEDataServer::update(const DataFrame &data) {
  if (!connected_) {
    return;
  }

  std::string json = data.toJson();
  struct os_mbuf *om = ble_hs_mbuf_from_flat(json.data(), json.size());
  ble_gatts_notify_custom(connHandle_, jsonAttrHandle_, om);
}

BLEDataServer::BLEDataServer(uint16_t connHandle, uint16_t jsonAttrHandle)
    : connHandle_(connHandle), jsonAttrHandle_(jsonAttrHandle) {}
