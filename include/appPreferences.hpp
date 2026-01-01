#pragma once
#include <Elog.h>

namespace prefs
{
  constexpr int MYLOG = 0;
  constexpr int TASKLOG = 1;
  constexpr int CALLOG = 2;
  constexpr char *APPNAME = ( char * ) "COMBO";                //! appname
  constexpr char *DEVICE_NAME = ( char * ) "C3-Combo Navi";    //! device name for bluetooth
  constexpr char *DEVICE_MANUFACT = ( char * ) "DIRK";         //! device name for bluetooth
  constexpr const LogLevel LOG_LEVEL = ELOG_LEVEL_DEBUG;       //! loglevel for App
  constexpr uint8_t LED_PIN = GPIO_NUM_8;                      //! pin for supermini
  constexpr uint8_t PWM_CHANNEL = 1;                           //! PWM CHANNEL
  constexpr uint8_t X_AXIS_PIN = GPIO_NUM_3;                   //! pin for x axis
  constexpr uint8_t Y_AXIS_PIN = GPIO_NUM_4;                   //! pin for y axis
  constexpr uint8_t BUTTON_PIN = GPIO_NUM_5;                   //! pin for button
  constexpr uint16_t DEFAULT_STICK_CENTER = 2280;              //! center value for joystick
  constexpr uint16_t DEFAULT_STICK_DEADZONE = 50;              //! deadzone for joystick
  constexpr uint16_t DEFAULT_STICK_MIN = 1000;                 //! min for joystick
  constexpr uint16_t DEFAULT_STICK_MAX = 3300;                 //! max for joystick
  constexpr unsigned long STICK_MEASURE_INTERVAL = 27UL;       //! interval for joystick measurement
  constexpr int MOUSE_MOVE_STEP_SHIFT = 5;                     //! shift value for mouse move step calculation
  constexpr uint32_t DELAY_IF_BT_NOT_CONNECTED_MS = 100;       //! delay if mouse not connected
  constexpr uint32_t MAX_DELAY_IF_BT_NOT_CONNECTED_MS = 1000;  //! maxdelay if mouse not connected
  constexpr uint32_t JOYSTICK_MEASURE_RTIME_DELTA_MS = 10;     //! realtime delta for joystick measurement
  // constexpr const char *MOUNTPOINT{ "/spiffs" };               //! mountpoint/makrker filesystem
  // constexpr const char *WEB_PARTITION_LABEL{ "mydata" };       //! label of the spiffs or null

}  // namespace prefs