/**
 * ESP32-C3 Bluetooth HID (Tastatur & Maus)
 */
#include <Arduino.h>
#include <Elog.h>
#include <esp_sleep.h>
#include <esp_wifi.h>
#include "main.hpp"
#include "appPreferences.hpp"
#include "BleCombo.hpp"
#include "joystick.hpp"
#include "appCurrPrefs.hpp"

bleComboSptr combo;
CurrPrefsPtr cPrefs;

void setup()
{
  Serial.begin( prefs::SERIAL_SPEED );
  // do not need
  esp_wifi_stop();
  // signal LED
  pinMode( prefs::BUTTON_PIN, INPUT_PULLUP );
  pinMode( prefs::LED_PIN, OUTPUT );
  analogWrite( prefs::LED_PIN, prefs::LED_OFF_BRIGHTNESS );
  //
  // lets blink threee times
  //
  blink_blue_led( 3 );
  // led off
  analogWrite( prefs::LED_PIN, prefs::LED_OFF_BRIGHTNESS );
#ifdef BUILD_DEBUG
  Serial.println( "Start C3 HID Setup..." );
  delay( 4000 );
#else
  delay( 10 );
#endif
  Logger.registerSerial( prefs::MYLOG, prefs::LOG_LEVEL, "main" );  // We want messages with DEBUG level and lower
  Logger.info( prefs::MYLOG, "Start C3 HID Setup" );
  Logger.debug( prefs::MYLOG, "START MAIN" );
  cPrefs = std::make_shared< CurrPrefs >();
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

  Logger.info( prefs::MYLOG, "Start C3 HID Setup...OK" );
  combo->begin();
}

void loop()
{
  static uint8_t counter{ 0 };
  static bool wasConnected{ true };
  static bool wasButtonDown{ false };
  static uint32_t timeIfButtonIsLong{ 0 };
  static uint32_t timeIfDeviceGoLightSleep{ prefs::LIGHT_SLEEP_TIMER_DELAY_MS };
  static uint32_t timeIfDeviceGoDeepSleep{ prefs::DEEP_SLEEP_TIMER_DELAY_MS };
  static esp_sleep_wakeup_cause_t cause{ ESP_SLEEP_WAKEUP_ALL };

  //
  // what's the current time
  //
  uint32_t currentMillis = millis();

  //
  // is it's time for deep sleep?
  // this means, its a long time nothing happen
  //
  if ( currentMillis > timeIfDeviceGoDeepSleep )
  {
    // time for deep sleep?
    // here is no return
    // it stsarts with setup()
    blink_blue_led( 6, 100 );
    cause = ESP_SLEEP_WAKEUP_ALL;
    make_sleep( SLEEP_DEEP );
  }

  //
  // was the last wakeup an wakeup timer event?
  //
  if ( cause == ESP_SLEEP_WAKEUP_TIMER )
  {
    //
    // nothing waked me eighter timer
    //
    // analogWrite( prefs::LED_PIN, prefs::LED_OFF_BRIGHTNESS );
    delay( 5 );
    Movement mv = BJoystick::getMovement();
    if ( mv.wasMoved )
    {
      // yeah, there was an movement
      timeIfDeviceGoLightSleep = millis() + prefs::LIGHT_SLEEP_TIMER_DELAY_MS;
      timeIfDeviceGoDeepSleep = millis() + prefs::DEEP_SLEEP_TIMER_DELAY_MS;
      cause = ESP_SLEEP_WAKEUP_ALL;
      wasConnected = false;
      // make the normal turn
    }
    else
    {
      cause = make_sleep( SLEEP_LIGHT );
      // try another turn when return from sleep
      return;
    }
  }

  //
  // is the BLE stack connected with an host?
  //
  if ( combo->isConnected() )
  {
    // while availible connection do never deep sleep
    timeIfDeviceGoDeepSleep = millis() + prefs::DEEP_SLEEP_TIMER_DELAY_MS;
    // make something other stuff later
    if ( !wasConnected )
    {
      // new connected the BLE stack, mak LED ON
      Logger.info( prefs::MYLOG, "BT Connected!" );
      wasConnected = true;
      analogWrite( prefs::LED_PIN, prefs::LED_CONNECTED_BRIGHTNESS );
    }
    //
    // test if an movement of the joystick or keys happend
    //
    Movement mv = BJoystick::getMovement();
    if ( mv.wasMoved )
    {
      // light sleep timer new set
      timeIfDeviceGoLightSleep = millis() + prefs::LIGHT_SLEEP_TIMER_DELAY_MS;
      // wakeup cause is NOT a timer
      cause = ESP_SLEEP_WAKEUP_ALL;
      //
      // here was an movement event, make sonthing
      //
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
        // Logger.debug( prefs::MYLOG, "Joystick moved!" );
      }
      // make an mouse movement
      combo->m_direct( &( mv.mv ) );
      delay( 5 );
    }
    else
    {
      //
      // there was'nt a movement event, maybe an break?
      if ( currentMillis > timeIfDeviceGoLightSleep )
      {
        //
        // make an little sleep
        //
        Logger.debug( prefs::MYLOG, "it's time to going light sleep..." );
        blink_blue_led( 2 );
        analogWrite( prefs::LED_PIN, prefs::LED_OFF_BRIGHTNESS );
        cause = make_sleep( SLEEP_LIGHT );
        // here continue after the (light) sleep
        wasConnected = false;
        // ESP_SLEEP_WAKEUP_GPIO
        // ESP_SLEEP_WAKEUP_TIMER
        // ESP_SLEEP_WAKEUP_ALL
        timeIfDeviceGoLightSleep = millis() + prefs::LIGHT_SLEEP_TIMER_DELAY_MS;
      }
    }
    delay( 5 );
  }
  else
  {
    //
    // not connected with any BLE stack
    //
    ++counter;
    if ( wasConnected )
    {
      // just switches
      // String advStr = combo->isAdvertizing() ? "true" : "false";
      Logger.info(prefs::MYLOG, "BT Disconnected!");
      // Logger.debug( prefs::MYLOG, "BT Disconnected! (Advertizing: %s)", advStr.c_str() );
      wasConnected = false;
      analogWrite( prefs::LED_PIN, prefs::LED_OFF_BRIGHTNESS );
      timeIfDeviceGoDeepSleep = millis() + prefs::DEEP_SLEEP_TIMER_DELAY_MS;
    }
    String advStr = combo->isAdvertizing() ? "true" : "false";
    // Logger.debug( prefs::MYLOG, "BT wait for connevting (Advertizing: %s)", advStr.c_str() );
    if ( !combo->isAdvertizing() )
    {
      combo->startAdvertizing();
    }
    //
    // mak an flashing with the blue LED
    //
    if ( counter == 0 )
      analogWrite( prefs::LED_PIN, prefs::LED_ATTENTION_BRIGHTNESS );
    if ( counter == 30 )
      analogWrite( prefs::LED_PIN, prefs::LED_OFF_BRIGHTNESS );
    //
    // check anmovement event
    // i'm only interested on long buton for calibre the stick
    //
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
}

//
// let the blue LED flashing
//
void blink_blue_led( uint8_t _times, uint32_t _interval )
{
  for ( uint32_t i = 0; i < _times; i++ )
  {
    analogWrite( prefs::LED_PIN, prefs::LED_ATTENTION_BRIGHTNESS );
    delay( _interval );
    analogWrite( prefs::LED_PIN, prefs::LED_OFF_BRIGHTNESS );
    delay( _interval );
  }
}

//
// let the controller a litte bit sleep
//
esp_sleep_wakeup_cause_t make_sleep( esp_sleep_type _type )
{
  //
  // light sleep or deep sleep?
  //
  esp_err_t is_ok;

  switch ( _type )
  {
    case SLEEP_LIGHT:
      /* code */
      Logger.info( prefs::MYLOG, "Controller into LIGHT SLEEP, Wakeup with mouse button!" );
      Logger.debug( prefs::MYLOG, "Controller wakes without restart." );
      is_ok = esp_deep_sleep_enable_gpio_wakeup( BIT( prefs::BUTTON_PIN ), ESP_GPIO_WAKEUP_GPIO_LOW );
      esp_sleep_enable_timer_wakeup( prefs::TIMER_LIGHT_SLEEP_DELAY_MS * 1000 );
      delay( 20 );
      esp_light_sleep_start();
      // return cause of wakeup
      return esp_sleep_get_wakeup_cause();
      break;

    case SLEEP_DEEP:
      //
      // disable (for secure) timer wakeup
      //
      esp_sleep_disable_wakeup_source( ESP_SLEEP_WAKEUP_TIMER );
      //
      // enable GPIO for wakeup controller
      //
      is_ok = esp_deep_sleep_enable_gpio_wakeup( BIT( prefs::BUTTON_PIN ), ESP_GPIO_WAKEUP_GPIO_LOW );
      if ( is_ok == ESP_OK )
      {
        Logger.info( prefs::MYLOG, "Controller into DEEP SLEEP, Wakeup with mouse button!" );
        Logger.debug( prefs::MYLOG, "Controller wakes with restart." );

        delay( 20 );
        // this function never returns, it starts with setup()
        esp_deep_sleep_start();
      }
      break;
  }
  return ESP_SLEEP_WAKEUP_ALL;
}
