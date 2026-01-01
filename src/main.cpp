/**
 * ESP32-C3 Bluetooth HID (Tastatur & Maus)
 */
#include <Arduino.h>
#include <Elog.h>
#include "main.hpp"
#include "appPreferences.hpp"
#include "BleCombo.hpp"
#include "joystick.hpp"
#include "appCurrPrefs.hpp"

bleComboSptr combo;
CurrPrefsPtr cPrefs;

void setup()
{
  Serial.begin( 115200 );
  delay( 3000 );
  Serial.println( "Start C3 HID Setup..." );
  Logger.registerSerial( prefs::MYLOG, prefs::LOG_LEVEL, "main" );  // We want messages with DEBUG level and lower
  Logger.debug( prefs::MYLOG, "Start C3 HID Setup...OK" );
  Logger.debug( prefs::MYLOG, "START MAIN" );
  cPrefs = std::make_shared< CurrPrefs >();
  pinMode( prefs::BUTTON_PIN, INPUT_PULLUP );
  pinMode( prefs::LED_PIN, OUTPUT );
  analogWrite( prefs::LED_PIN, 255 );
  //
  // init joystick object
  //
  BJoystick::begin();
  Logger.debug( prefs::MYLOG, "load joystick calibration..." );
  JoystickRange range = cPrefs->getJoystickRange();
  BJoystick::setCalibre( range );
  delay( 10 );
  //
  // init Combo object
  //
  Logger.debug( prefs::MYLOG, "init mouse/keyboard..." );
  combo = std::make_shared< BleCombo >( prefs::DEVICE_NAME, prefs::DEVICE_MANUFACT, 90 );
  Logger.debug( prefs::MYLOG, "init mouse/keyboard...OK" );
  Logger.debug( prefs::MYLOG, "init mouse/keyboard beginn..." );
  Logger.debug( prefs::MYLOG, "init mouse/keyboard beginn...OK" );

  Logger.debug( prefs::MYLOG, "init mouse/keyboard beginn...OK" );

  Logger.debug( prefs::MYLOG, "Start C3 HID Setup...OK" );
  combo->begin();
}

void loop()
{
  static uint32_t delayIfNotConnected{ prefs::DELAY_IF_BT_NOT_CONNECTED_MS };
  static uint32_t nextTimeToMouseMove{ 0 };
  static uint32_t nextTimeToKeyboardEvent{ millis() + 20000 };
  static uint8_t counter{ 0 };
  static bool wasConnected{ true };
  static bool wasButtonDown{ false };
  static uint32_t timeIfButtonIsLong{ 0 };

  // it was not connected and its time tio check again
  if ( combo->isConnected() )
  {
    // connected again, reset delay
    // make something other stuff later
    if ( !wasConnected )
    {
      Logger.debug( prefs::MYLOG, "BT Connected!" );
      wasConnected = true;
      analogWrite(prefs::LED_PIN, 255 - 80);
      delayIfNotConnected = prefs::DELAY_IF_BT_NOT_CONNECTED_MS;
      nextTimeToKeyboardEvent = millis() + 20000;
    }
    Movement mv = BJoystick::getMovement();
    if ( mv.wasMoved )
    {
      if ( mv.mv[ BUTTONS ] )
      {
        if ( mv.mv[ BUTTONS ] & MOUSE_LEFT )
        {
          if ( !wasButtonDown )
          {
            wasButtonDown = true;
            // button just down
            Logger.debug( prefs::MYLOG, "Button Down..." );
            timeIfButtonIsLong = millis() + 3000;
          }
        }
        Logger.debug( prefs::MYLOG, "Joystick moved! (+Button)" );
      }
      else
      {
        if ( wasButtonDown )
        {
          wasButtonDown = false;
          Logger.debug( prefs::MYLOG, "Button Up..." );
          // button was pressed, just released
          if ( millis() > timeIfButtonIsLong )
          {
            Logger.debug( prefs::MYLOG, "Button long down..." );
            BJoystick::calibreStick();
            Logger.debug( prefs::MYLOG, "Button long down...OK" );
          }
        }
        Logger.debug( prefs::MYLOG, "Joystick moved!" );
      }
      combo->m_direct( &( mv.mv ) );
      delay( 5 );
    }
    delay( 5 );
  }
  else
  {
    // 
    // not connected
    //
    ++counter;
    if ( wasConnected )
    {
      // just switches
      String advStr = combo->isAdvertizing() ? "true" : "false";
      Logger.debug( prefs::MYLOG, "BT Disconnected! (Advertizing: %s)", advStr.c_str() );
      wasConnected = false;
      analogWrite( prefs::LED_PIN, 255);
    }
    // still not connected, increase delay
    delayIfNotConnected += prefs::DELAY_IF_BT_NOT_CONNECTED_MS;
    if ( delayIfNotConnected > prefs::MAX_DELAY_IF_BT_NOT_CONNECTED_MS )
    {
      delayIfNotConnected = prefs::MAX_DELAY_IF_BT_NOT_CONNECTED_MS;
    }
    String advStr = combo->isAdvertizing() ? "true" : "false";
    // Logger.debug( prefs::MYLOG, "BT wait for connevting (Advertizing: %s)", advStr.c_str() );
    if ( !combo->isAdvertizing() )
    {
      combo->startAdvertizing();
    }
    if( counter == 0 )
      analogWrite( prefs::LED_PIN, 0 );
    if ( counter == 30 )
      analogWrite( prefs::LED_PIN, 255 );
    // uint8_t aa = counter >> 3;
    // if( aa & 0x3  == 0 )
    // {
    //   analogWrite( prefs::LED_PIN, 0 );
    //   delay(80);
    // }
    // else if (aa & 0x03 == 3 )
    //   analogWrite( prefs::LED_PIN, 255 );
    Movement mv = BJoystick::getMovement();
    if ( mv.wasMoved )
    {
      if ( mv.mv[ BUTTONS ] )
      {
        if ( mv.mv[ BUTTONS ] & MOUSE_LEFT )
        {
          if ( !wasButtonDown )
          {
            wasButtonDown = true;
            // button just down
            Logger.debug( prefs::MYLOG, "Button Down..." );
            timeIfButtonIsLong = millis() + 3000;
          }
        }
        // Logger.debug( prefs::MYLOG, "Joystick moved! (+Button)" );
      }
      else
      {
        if ( wasButtonDown )
        {
          wasButtonDown = false;
          Logger.debug( prefs::MYLOG, "Button Up..." );
          // button was pressed, just released
          if ( millis() > timeIfButtonIsLong )
          {
            Logger.debug( prefs::MYLOG, "Button long down..." );
            BJoystick::calibreStick();
            Logger.debug( prefs::MYLOG, "Button long down...OK" );
          }
        }
      }
    }
    delay( 5 );
  }

  uint32_t currentMillis = millis();
}
