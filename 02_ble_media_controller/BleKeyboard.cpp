#include "BleKeyboard.h"

#if defined(CONFIG_BT_ENABLED)

#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
#include "esp_log.h"

static const char* LOG_TAG = "BleKeyboard";

#define KEYBOARD_ID 0x01
#define MEDIA_KEYS_ID 0x02

static const uint8_t _hidReportDescriptor[] = {
  0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
  0x09, 0x06,                    // USAGE (Keyboard)
  0xa1, 0x01,                    // COLLECTION (Application)
  0x85, KEYBOARD_ID,             //   REPORT_ID (1)
  0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
  0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
  0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
  0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
  0x75, 0x01,                    //   REPORT_SIZE (1)
  0x95, 0x08,                    //   REPORT_COUNT (8)
  0x81, 0x02,                    //   INPUT (Data,Var,Abs)
  0x95, 0x01,                    //   REPORT_COUNT (1)
  0x75, 0x08,                    //   REPORT_SIZE (8)
  0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
  0x95, 0x05,                    //   REPORT_COUNT (5)
  0x75, 0x01,                    //   REPORT_SIZE (1)
  0x05, 0x08,                    //   USAGE_PAGE (LEDs)
  0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
  0x29, 0x05,                    //   USAGE_MAXIMUM (Kana)
  0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
  0x95, 0x01,                    //   REPORT_COUNT (1)
  0x75, 0x03,                    //   REPORT_SIZE (3)
  0x91, 0x03,                    //   OUTPUT (Cnst,Var,Abs)
  0x95, 0x06,                    //   REPORT_COUNT (6)
  0x75, 0x08,                    //   REPORT_SIZE (8)
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
  0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
  0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
  0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
  0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
  0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
  0xc0,                          // END_COLLECTION

  // Media Keys
  0x05, 0x0c,                    // USAGE_PAGE (Consumer Devices)
  0x09, 0x01,                    // USAGE (Consumer Control)
  0xa1, 0x01,                    // COLLECTION (Application)
  0x85, MEDIA_KEYS_ID,           //   REPORT_ID (2)
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
  0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
  0x75, 0x01,                    //   REPORT_SIZE (1)
  0x95, 0x10,                    //   REPORT_COUNT (16)
  0x09, 0xb5,                    //   USAGE (Scan Next Track)
  0x09, 0xb6,                    //   USAGE (Scan Previous Track)
  0x09, 0xb7,                    //   USAGE (Stop)
  0x09, 0xcd,                    //   USAGE (Play/Pause)
  0x09, 0xe2,                    //   USAGE (Mute)
  0x09, 0xe9,                    //   USAGE (Volume Increment)
  0x09, 0xea,                    //   USAGE (Volume Decrement)
  0x09, 0x23,                    //   USAGE (AL Home)
  0x0a, 0x8a, 0x01,              //   USAGE (AL Local Machine Browser)
  0x0a, 0x92, 0x01,              //   USAGE (AL Calculator)
  0x0a, 0x2a, 0x02,              //   USAGE (AC Bookmarks)
  0x0a, 0x21, 0x02,              //   USAGE (AC Search)
  0x0a, 0x26, 0x02,              //   USAGE (AC Stop)
  0x0a, 0x24, 0x02,              //   USAGE (AC Back)
  0x0a, 0x83, 0x01,              //   USAGE (AL Consumer Control Configuration)
  0x0a, 0x8a, 0x02,              //   USAGE (AL Email Reader)
  0x81, 0x02,                    //   INPUT (Data,Var,Abs)
  0xc0                           // END_COLLECTION
};

BleKeyboard::BleKeyboard(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel) 
    : hid(0)
    , deviceName(std::string(deviceName).substr(0, 15))
    , deviceManufacturer(std::string(deviceManufacturer).substr(0,15))
    , batteryLevel(batteryLevel) {}

void BleKeyboard::begin(void)
{
  BLEDevice::init(String(deviceName.c_str()));
  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(this);

  hid = new BLEHIDDevice(pServer);
  inputKeyboard = hid->inputReport(KEYBOARD_ID);
  outputKeyboard = hid->outputReport(KEYBOARD_ID);
  inputMediaKeys = hid->inputReport(MEDIA_KEYS_ID);

  outputKeyboard->setCallbacks(this);

  hid->manufacturer()->setValue(String(deviceManufacturer.c_str()));

  hid->pnp(0x02, vid, pid, version);
  hid->hidInfo(0x00, 0x01);

  BLESecurity* pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);

  hid->reportMap((uint8_t*)_hidReportDescriptor, sizeof(_hidReportDescriptor));
  hid->startServices();

  onStarted(pServer);

  advertising = pServer->getAdvertising();
  advertising->setAppearance(HID_KEYBOARD);
  advertising->addServiceUUID(hid->hidService()->getUUID());
  advertising->setScanResponse(false);
  advertising->start();
  hid->setBatteryLevel(batteryLevel);
}

void BleKeyboard::end(void) {}

bool BleKeyboard::isConnected(void) {
  return this->connected;
}

void BleKeyboard::setBatteryLevel(uint8_t level) {
  this->batteryLevel = level;
  if (this->hid != 0) {
    this->hid->setBatteryLevel(this->batteryLevel);
  }
}

void BleKeyboard::sendReport(KeyReport* keys) {
  if (this->isConnected()) {
    this->inputKeyboard->setValue((uint8_t*)keys, sizeof(KeyReport));
    this->inputKeyboard->notify();
  }
}

void BleKeyboard::sendReport(MediaKeyReport* keys) {
  if (this->isConnected()) {
    this->inputMediaKeys->setValue((uint8_t*)keys, sizeof(MediaKeyReport));
    this->inputMediaKeys->notify();
  }
}

extern const uint8_t _asciimap[128];
#define SHIFT 0x80
const uint8_t _asciimap[128] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x2a, 0x2b, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x00,
  0x2c, 0x1e|SHIFT, 0x1f|SHIFT, 0x20|SHIFT, 0x21|SHIFT, 0x22|SHIFT, 0x23|SHIFT, 0x34,
  0x26|SHIFT, 0x27|SHIFT, 0x25|SHIFT, 0x2e|SHIFT, 0x36, 0x2d, 0x37, 0x38,
  0x27, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24,
  0x25, 0x26, 0x33|SHIFT, 0x33, 0x36|SHIFT, 0x2e, 0x37|SHIFT, 0x38|SHIFT,
  0x1f|SHIFT, 0x04|SHIFT, 0x05|SHIFT, 0x06|SHIFT, 0x07|SHIFT, 0x08|SHIFT, 0x09|SHIFT, 0x0a|SHIFT,
  0x0b|SHIFT, 0x0c|SHIFT, 0x0d|SHIFT, 0x0e|SHIFT, 0x0f|SHIFT, 0x10|SHIFT, 0x11|SHIFT, 0x12|SHIFT,
  0x13|SHIFT, 0x14|SHIFT, 0x15|SHIFT, 0x16|SHIFT, 0x17|SHIFT, 0x18|SHIFT, 0x19|SHIFT, 0x1a|SHIFT,
  0x1b|SHIFT, 0x1c|SHIFT, 0x1d|SHIFT, 0x2f, 0x31, 0x30, 0x23|SHIFT, 0x2d|SHIFT,
  0x35, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
  0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12,
  0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
  0x1b, 0x1c, 0x1d, 0x2f|SHIFT, 0x31|SHIFT, 0x30|SHIFT, 0x35|SHIFT, 0x4c
};

size_t BleKeyboard::write(uint8_t c) {
  uint8_t key = 0;
  uint8_t modifiers = 0;
  if (c >= 136) {
    key = c - 136;
  } else {
    key = pgm_read_byte(_asciimap + c);
    if (!key) return 0;
    if (key & 0x80) {
      modifiers |= KEY_LEFT_SHIFT;
      key &= 0x7f;
    }
  }

  KeyReport report = {0};
  report.modifiers = modifiers;
  report.keys[0] = key;
  sendReport(&report);
  delay_ms(this->_delay_ms);

  report.modifiers = 0;
  report.keys[0] = 0;
  sendReport(&report);
  delay_ms(this->_delay_ms);

  return 1;
}

size_t BleKeyboard::write(const MediaKeyReport id) {
  uint16_t key = id[0] | (id[1] << 8);
  MediaKeyReport report = { (uint8_t)(key & 0xFF), (uint8_t)((key >> 8) & 0xFF) };
  sendReport(&report);
  delay_ms(this->_delay_ms);

  report[0] = 0;
  report[1] = 0;
  sendReport(&report);
  delay_ms(this->_delay_ms);
  return 1;
}

size_t BleKeyboard::write(const uint8_t *buffer, size_t size) {
  size_t n = 0;
  while (size--) {
    if (write(*buffer++)) n++;
    else break;
  }
  return n;
}

void BleKeyboard::release(uint8_t k) {
  KeyReport report = {0};
  sendReport(&report);
}

void BleKeyboard::release(const MediaKeyReport id) {
  MediaKeyReport report = {0, 0};
  sendReport(&report);
}

void BleKeyboard::releaseAll(void) {
  KeyReport report = {0};
  sendReport(&report);
  MediaKeyReport mediaReport = {0, 0};
  sendReport(&mediaReport);
}

void BleKeyboard::onConnect(BLEServer* pServer) {
  this->connected = true;
}

void BleKeyboard::onDisconnect(BLEServer* pServer) {
  this->connected = false;
}

void BleKeyboard::onWrite(BLECharacteristic* me) {}

void BleKeyboard::delay_ms(uint64_t ms) {
  uint64_t m = esp_timer_get_time();
  if (ms) {
    uint64_t e = (m + (ms * 1000));
    if (m > e) {
      while (esp_timer_get_time() > e) {
        vTaskDelay(1);
      }
    }
    while (esp_timer_get_time() < e) {
      vTaskDelay(1);
    }
  }
}

#endif // CONFIG_BT_ENABLED
