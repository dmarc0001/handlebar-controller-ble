/**
 * ESP32-C3 Bluetooth HID (Tastatur & Maus)
 */
#include <Arduino.h>
#include <Elog.h>
#include "appPreferences.hpp"
#include "BleCombo.hpp"
#include "joystick.hpp"

bleComboSptr combo;

void setup()
{
  Serial.begin( 115200 );
  delay( 3000 );
  Serial.println( "Start C3 HID Setup..." );
  Logger.registerSerial( prefs::MYLOG, prefs::LOG_LEVEL, "main" );  // We want messages with DEBUG level and lower
  Logger.debug( prefs::MYLOG, "Start C3 HID Setup...OK" );
  Logger.debug( prefs::MYLOG, "START MAIN" );
  pinMode( prefs::BUTTON_PIN, INPUT_PULLUP );
  pinMode( prefs::LED_PIN, OUTPUT );
  analogWrite( prefs::LED_PIN, 255 );
  //
  // init joystick object
  //
  BJoystick::begin();
  //
  // init Combo object
  //
  Logger.debug( prefs::MYLOG, "init mouse/keyboard..." );
  combo = std::make_shared< BleCombo >( prefs::DEVICE_NAME, prefs::DEVICE_MANUFACT, 90 );
  Logger.debug( prefs::MYLOG, "init mouse/keyboard...OK" );
  Logger.debug( prefs::MYLOG, "init mouse/keyboard beginn..." );
  combo->begin();
  Logger.debug( prefs::MYLOG, "init mouse/keyboard beginn...OK" );

  Logger.debug( prefs::MYLOG, "init mouse/keyboard beginn...OK" );

  Logger.debug( prefs::MYLOG, "Start C3 HID Setup...OK" );
}

void loop()
{
  static uint32_t delayIfNotConnected{ prefs::DELAY_IF_BT_NOT_CONNECTED_MS };
  static uint32_t nextTimeToMouseMove{ 0 };
  static uint32_t nextTimeToKeyboardEvent{ millis() + 20000 };
  static uint8_t colorcounter{ 0 };
  static bool wasConnected{ false };

  // it was not connected and its time tio check again
  if ( combo->isConnected() )
  {
    // connected again, reset delay
    // make something other stuff later
    if ( !wasConnected )
    {
      Logger.debug( prefs::MYLOG, "BT Connected!" );
      wasConnected = true;
      // analogWrite(prefs::LED_PIN, 255);
      delayIfNotConnected = prefs::DELAY_IF_BT_NOT_CONNECTED_MS;
      nextTimeToKeyboardEvent = millis() + 20000;
    }
    Movement mv = BJoystick::getMovement();
    if ( mv.wasMoved )
    {
      if ( mv.mv[ BUTTONS ] )
        Logger.debug( prefs::MYLOG, "Joystick moved! (+Button)" );
      else
        Logger.debug( prefs::MYLOG, "Joystick moved!" );
      combo->m_direct( &( mv.mv ) );
      delay( 5 );
    }
  }
  else
  {
    if ( wasConnected )
    {
      String advStr = combo->isAdvertizing() ? "true" : "false";
      Logger.debug( prefs::MYLOG, "BT Disconnected! (Advertizing: %s)", advStr.c_str() );
      wasConnected = false;
      // analogWrite( prefs::LED_PIN, 255 );
    }
    // still not connected, increase delay
    delayIfNotConnected += prefs::DELAY_IF_BT_NOT_CONNECTED_MS;
    if ( delayIfNotConnected > prefs::MAX_DELAY_IF_BT_NOT_CONNECTED_MS )
    {
      delayIfNotConnected = prefs::MAX_DELAY_IF_BT_NOT_CONNECTED_MS;
    }
    String advStr = combo->isAdvertizing() ? "true" : "false";
    Logger.debug( prefs::MYLOG, "BT wait for connevting (Advertizing: %s)", advStr.c_str() );
    if ( !combo->isAdvertizing() )
    {
      combo->startAdvertizing();
    }
    delay( delayIfNotConnected );
    return;
  }

  uint32_t currentMillis = millis();
  delay( 5 );
}
