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

//  Low level key report: up to 6 keys and shift, ctrl etc at once
typedef struct
{
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} KeyReport;

using NimBLEHIDDevicePtr = std::shared_ptr<NimBLEHIDDevice>;

//
// CLASS DEFINITION
//
class BleCombo : protected NimBLEServerCallbacks 
{
  public:
    using Callback = std::function<void(void)>;

  public:
    BleCombo(std::string deviceName = "ESP32-Mouse", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100);
    void begin(void);
    void end(void);
    void click(uint8_t b = MOUSE_LEFT);
    void move(signed char x, signed char y, signed char wheel = 0, signed char hWheel = 0);
    void press(uint8_t b = MOUSE_LEFT);            // press LEFT by default
    void release(uint8_t b = MOUSE_LEFT);          // release LEFT by default
    bool isPressed(uint8_t b = MOUSE_LEFT) const;  // check LEFT by default
    bool isConnected(void) const;
    void setBatteryLevel(uint8_t level);
    void onConnect(Callback cb);
    void onDisconnect(Callback cb);

  protected:
    void buttons(uint8_t b);
    void rawAction(uint8_t msg[], char msgSize);
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override;
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override;

  protected:
    uint8_t               _buttons;
    NimBLEHIDDevice*      hid;
    NimBLECharacteristic* inputMouse;

    uint8_t     batteryLevel;
    std::string deviceManufacturer;
    std::string deviceName;
    bool        connected = false;

    Callback connectCallback    = nullptr;
    Callback disconnectCallback = nullptr;
};


#endif // CONFIG_BT_NIMBLE_ROLE_PERIPHERAL
#endif // CONFIG_BT_ENABLED
#endif // ESP32_BLE_COMBO
