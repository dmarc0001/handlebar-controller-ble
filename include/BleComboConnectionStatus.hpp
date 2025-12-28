#pragma once
#ifndef ESP32_BLE_CONNECTION_STATUS_H
#define ESP32_BLE_CONNECTION_STATUS_H
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include <NimBLEServer.h>
#include "NimBLECharacteristic.h"
#include <Arduino.h>
#include "appPreferences.hpp"
#include <Elog.h>
#include <memory>

class BleConnectionStatus 
{
public:
  BleConnectionStatus(void) {};
  bool connected = false;
  NimBLECharacteristic *inputKeyboard;
  NimBLECharacteristic *outputKeyboard;
  NimBLECharacteristic *inputMouse;
};

using BleConnectionStatusPtr = std::shared_ptr<BleConnectionStatus>;

#endif // CONFIG_BT_ENABLED
#endif // ESP32_BLE_CONNECTION_STATUS_H
