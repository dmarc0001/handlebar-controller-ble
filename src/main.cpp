/**
 * ESP32-C3 Bluetooth HID (Tastatur & Maus)
 */
#include <Arduino.h>
#include "appPreferences.hpp"
#include "configs.hpp"
#include <BleCombo.hpp>
#include <Elog.h>

void setup()
{
  using namespace config;

  Serial.begin(115200);
  delay(3000);
  Serial.println("Start C3 HID Setup...");
  Logger.registerSerial(prefs::MYLOG, prefs::LOG_LEVEL, "main"); // We want messages with DEBUG level and lower
  Logger.debug(prefs::MYLOG, "Start C3 HID Setup...OK");
  Logger.debug(prefs::MYLOG, "START MAIN");
  //
  // init LED Object
  //
  Logger.debug(prefs::MYLOG, "init LED...");
  ConfigObj::led = std::make_shared<neopixel::OnboardLed>(prefs::LED_PIN);
  ConfigObj::led->clear();
  ConfigObj::led->setColor(ConfigObj::led->color_yellow);
  //
  // init Combo object
  //
  Logger.debug(prefs::MYLOG, "init mouse/keyboard...");
  ConfigObj::combo = std::make_shared<BleCombo>(prefs::DEVICE_NAME, prefs::DEVICE_MANUFACT, 100);
  Logger.debug(prefs::MYLOG, "init mouse/keyboard...OK");
  Logger.debug(prefs::MYLOG, "init mouse/keyboard beginn...");
  ConfigObj::combo->begin();
  Logger.debug(prefs::MYLOG, "init mouse/keyboard beginn...OK");

  pinMode(prefs::BUTTON_PIN, INPUT_PULLUP);
  Logger.debug(prefs::MYLOG, "init mouse/keyboard beginn...OK");

  Logger.debug(prefs::MYLOG, "Start C3 HID Setup...OK");
}

void loop()
{
  static uint32_t delayIfNotConnected{prefs::DELAY_IF_BT_NOT_CONNECTED_MS};
  static uint32_t nextTimeToMouseMove{ 0 };
  static uint32_t nextTimeToKeyboardEvent{ millis() + 20000 };
  static uint8_t colorcounter{0};
  static bool wasConnected{false};

  using namespace config;

  // it was not connected and its time tio check again
  if (ConfigObj::combo->isConnected())
  {
    // connected again, reset delay
    // make something other stuff later
    if (! wasConnected)
    {
      Logger.debug(prefs::MYLOG, "BT Connected!");
      wasConnected = true;
      ConfigObj::led->setColor(ConfigObj::led->color_blue);
      delayIfNotConnected = prefs::DELAY_IF_BT_NOT_CONNECTED_MS;
      nextTimeToKeyboardEvent = millis() + 20000;
    }
  }
  else
  {
    if ( wasConnected)
    {
      Logger.debug(prefs::MYLOG, "BT Disconnected!");
      wasConnected = false;
      ConfigObj::led->setColor(ConfigObj::led->color_yellow);
    }
    // still not connected, increase delay
    delayIfNotConnected += prefs::DELAY_IF_BT_NOT_CONNECTED_MS;
    if (delayIfNotConnected > prefs::MAX_DELAY_IF_BT_NOT_CONNECTED_MS)
    {
      delayIfNotConnected = prefs::MAX_DELAY_IF_BT_NOT_CONNECTED_MS;
    }
    Logger.debug(prefs::MYLOG, "wait for BT Connection...");
    delay(delayIfNotConnected);
    return;
  }

  uint32_t currentMillis = millis();
  //
  // so BT is connected, do normal work
  //

  // its time to move the mouse for test purposes?
  if( currentMillis >  nextTimeToMouseMove )
  {
    nextTimeToMouseMove = currentMillis + 2000;
    // move the mouse one pixel
    Logger.debug(prefs::MYLOG, "mouse action triggered...");
    ConfigObj::combo->move(1, 1); 
    //ConfigObj::combo->click(MOUSE_RIGHT);
    uint8_t col = colorcounter & 0x03;
    switch( col )
    {
      case 0:
        Logger.debug(prefs::MYLOG, "set LED color RED");
        ConfigObj::led->setColor(255,0,0);
        break;
      case 1:
        Logger.debug(prefs::MYLOG, "set LED color GREEN");
        ConfigObj::led->setColor(0,255,0);
        break;
      case 2:
        Logger.debug(prefs::MYLOG, "set LED color BLUE");
        ConfigObj::led->setColor(0,0,255);
        break;
      case 3:
      default:
        Logger.debug(prefs::MYLOG, "set LED off");
        ConfigObj::led->setColor(0, 0, 0);
        break;
    }
    // just to be sure we dont overflow
    ++colorcounter;

  }

  // its time to make an keyboard event?
  if( currentMillis >  nextTimeToKeyboardEvent )
  {
    nextTimeToKeyboardEvent = currentMillis + 5000;
    Logger.debug(prefs::MYLOG, "keyboard action triggered...");
    // ConfigObj::combo->write('a');
    // ConfigObj::combo->print("hello");
    // ConfigObj::combo->write(KEY_RETURN);
  }
  delay(5);
}
