#pragma once
#ifndef ESP32_BLE_COMBO
#define ESP32_BLE_COMBO
#include "sdkconfig.h"
#if defined( CONFIG_BT_ENABLED )

#include "nimconfig.h"
#if defined( CONFIG_BT_NIMBLE_ROLE_PERIPHERAL )

#include <NimBLEServer.h>
#include <NimBLECharacteristic.h>
#include <NimBLEHIDDevice.h>
#include <Print.h>
#include "BleComboConnectionStatus.hpp"
#include <memory>

#include "keycodes.hpp"

using NimBLEHIDDevicePtr = std::shared_ptr< NimBLEHIDDevice >;

//
// CLASS DEFINITION
//
class BleCombo: public NimBLEServerCallbacks
{
  public:
  using Callback = std::function< void( void ) >;

  public:
  BleCombo( std::string deviceName = "ESP32-Mouse", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100 );
  void begin( void );
  void end( void );
  void m_click( uint8_t b = MOUSE_LEFT );
  void m_move( signed char x, signed char y, signed char wheel = 0, signed char hWheel = 0 );
  void m_press( uint8_t b = MOUSE_LEFT );            // press LEFT by default
  void m_release( uint8_t b = MOUSE_LEFT );          // release LEFT by default
  bool m_isPressed( uint8_t b = MOUSE_LEFT ) const;  // check LEFT by default
  bool isConnected( void ) const;
  void setBatteryLevel( uint8_t level );
  bool isAdvertizing();
  void onConnect( Callback cb );
  void onDisconnect( Callback cb );
  void startAdvertizing();
  void k_sendReport(KeyReport *keys);
  // size_t k_press(uint8_t k);
  // size_t k_release(uint8_t k);
  // size_t write(uint8_t c);
  // size_t write(const uint8_t *buffer, size_t size);
  // void k_releaseAll(void);

  protected:
  void m_buttons( uint8_t b );
  void rawAction( uint8_t msg[], char msgSize );
  void onConnect( NimBLEServer *pServer, NimBLEConnInfo &connInfo ) override;
  void onDisconnect( NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason ) override;

  protected:
  uint8_t _buttons;
  BleConnectionStatusPtr connectionStatus;
  NimBLEHIDDevice *hid;

  uint8_t batteryLevel;
  std::string deviceManufacturer;
  std::string deviceName;

  Callback connectCallback = nullptr;     // maybe i want to callback something on connect
  Callback disconnectCallback = nullptr;  // maybe i want to callback something on disconnect
};

#endif  // CONFIG_BT_NIMBLE_ROLE_PERIPHERAL
#endif  // CONFIG_BT_ENABLED
#endif  // ESP32_BLE_COMBO
