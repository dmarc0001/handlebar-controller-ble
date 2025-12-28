#pragma once
#ifndef ESP32_BLE_COMBO
#define ESP32_BLE_COMBO
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include "nimconfig.h"
#if defined(CONFIG_BT_NIMBLE_ROLE_PERIPHERAL)

#include <NimBLEServer.h>
#include <NimBLECharacteristic.h>
#include <NimBLEHIDDevice.h>
#include <Print.h>
#include "BleComboConnectionStatus.hpp"
#include <memory>

#include "keycodes.hpp"

using NimBLEHIDDevicePtr = std::shared_ptr<NimBLEHIDDevice>;

//
// CLASS DEFINITION
//
class BleCombo : public Print, NimBLEServerCallbacks, NimBLECharacteristicCallbacks
{
public:
  using Callback = std::function<void(void)>;

public:
  BleCombo(std::string deviceName = "ESP32-Combo", std::string deviceManufacturer = "nowhere", uint8_t batteryLevel = 99);

protected:
  // NimBLEServerCallbacks
  void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) override;
  void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) override;
  // NimBLECharacteristicCallbacks
  virtual void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override;

public:
  // mouse methods
  void mouse_click(uint8_t b = MOUSE_LEFT);
  void mouse_move(signed char x, signed char y, signed char wheel = 0, signed char hWheel = 0);
  void mouse_press(uint8_t b = MOUSE_LEFT);           // press LEFT by default
  void mouse_release(uint8_t b = MOUSE_LEFT);         // release LEFT by default
  bool mouse_isPressed(uint8_t b = MOUSE_LEFT) const; // check LEFT by default

  // mouse and keyboard common methods
  void begin(void);
  void end(void);
  bool isConnected(void) const;
  void onConnect(Callback cb);
  void onDisconnect(Callback cb);
  void setBatteryLevel(uint8_t level);

  // keyboard methods
  void keyboard_sendReport(KeyReport *keys);
  size_t keyboard_press(uint8_t k);
  size_t keyboard_release(uint8_t k);
  size_t write(uint8_t c);
  size_t write(const uint8_t *buffer, size_t size);
  void keyboard_releaseAll(void);

protected:
  // mouse methods
  void mouse_buttons(uint8_t b);
  void mouse_rawAction(uint8_t msg[], char msgSize);
  uint8_t _mouse_buttons; // mouse buttons state

  // keyboard members
  KeyReport _keyReport;

  // mouse and keyboard common members
  uint8_t batteryLevel;
  std::string deviceManufacturer;
  std::string deviceName;
  NimBLEHIDDevicePtr hid;
  BleConnectionStatusPtr connectionStatus;
  Callback connectCallback = nullptr;
  Callback disconnectCallback = nullptr;
};

#endif // CONFIG_BT_NIMBLE_ROLE_PERIPHERAL
#endif // CONFIG_BT_ENABLED
#endif // ESP32_BLE_COMBO
