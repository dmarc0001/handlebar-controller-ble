#pragma once
#include <Elog.h>

namespace prefs
{
  constexpr unsigned long SERIAL_SPEED = 115200UL;
  constexpr int MYLOG = 0;
  constexpr int TASKLOG = 1;
  constexpr int CALLOG = 2;
  constexpr char *APPNAME = ( char * ) "COMBO";                   //! appname
  constexpr char *DEVICE_NAME = ( char * ) "C3-Combo Navi";       //! device name for bluetooth
  constexpr char *DEVICE_MANUFACT = ( char * ) "DIRK";            //! device name for bluetooth
  #ifdef BUILD_DEBUG
  constexpr const LogLevel LOG_LEVEL = ELOG_LEVEL_DEBUG;          //! loglevel for App
  #else
  constexpr const LogLevel LOG_LEVEL = ELOG_LEVEL_INFO;          //! loglevel for App
  #endif
  constexpr uint8_t LED_PIN = GPIO_NUM_8;                         //! pin for supermini
  constexpr uint8_t LED_CONNECTED_BRIGHTNESS = 255 - 80;          //! brightness while connect
  constexpr uint8_t LED_ATTENTION_BRIGHTNESS = 0;                 //! brightness while attention
  constexpr uint8_t LED_OFF_BRIGHTNESS = 255;                     //! brightness while attention
  constexpr uint8_t X_AXIS_PIN = GPIO_NUM_3;                      //! pin for x axis
  constexpr uint8_t Y_AXIS_PIN = GPIO_NUM_4;                      //! pin for y axis
  constexpr uint8_t BUTTON_PIN = GPIO_NUM_5;                      //! pin for button availible while deep sleep 0,1,3,4,5
  constexpr uint16_t DEFAULT_STICK_CENTER = 2280;                 //! center value for joystick
  constexpr uint16_t DEFAULT_STICK_DEADZONE = 50;                 //! deadzone for joystick
  constexpr uint16_t DEFAULT_STICK_MIN = 1000;                    //! min for joystick
  constexpr uint16_t DEFAULT_STICK_MAX = 3300;                    //! max for joystick
  constexpr unsigned long STICK_MEASURE_INTERVAL = 27UL;          //! interval for joystick measurement
  constexpr int MOUSE_MOVE_STEP_SHIFT = 5;                        //! shift value for mouse move step calculation
  constexpr uint32_t JOYSTICK_MEASURE_RTIME_DELTA_MS = 10;        //! realtime delta for joystick measurement
  constexpr uint32_t TIMER_LIGHT_SLEEP_DELAY_MS = 150;            //! self weakup timer
  constexpr uint32_t LIGHT_SLEEP_TIMER_DELAY_MS = 2 * 60 * 1000;  //! delay if sleep mode begin
  constexpr uint32_t DEEP_SLEEP_TIMER_DELAY_MS = 15 * 60 * 1000;  //! delay if sleep mode begin
}  // namespace prefs