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

class BleConnectionStatus : public NimBLEServerCallbacks
{
public:
  BleConnectionStatus(void) {};
  bool connected = false;
  void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo)
  {
    Logger.debug(prefs::MYLOG, "BleConnectionStatus::onConnect...");
    this->connected = true;
  };
  void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo)
  {
    Logger.debug(prefs::MYLOG, "BleConnectionStatus::onDisConnect...");
    this->connected = false;
  };
  NimBLECharacteristic *inputKeyboard;
  NimBLECharacteristic *outputKeyboard;
  NimBLECharacteristic *inputMouse;
};

using BleConnectionStatusPtr = std::shared_ptr<BleConnectionStatus>;

#endif // CONFIG_BT_ENABLED
#endif // ESP32_BLE_CONNECTION_STATUS_H
