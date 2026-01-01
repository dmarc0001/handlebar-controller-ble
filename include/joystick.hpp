#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp32-hal-adc.h>
#include <freertos/event_groups.h>
#include <memory>
#include "appPreferences.hpp"
#include "keycodes.hpp"

using MouseMovement = uint8_t[ 5 ];
using MouseMovementCalibr = uint16_t[ 5 ];

enum MovementFields : int
{
  BUTTONS = 0,
  X_MOVE = 1,
  Y_MOVE = 2,
  WHEEL = 3,
  H_WHEEL = 4
};

typedef struct
{
  bool wasMoved;
  MouseMovement mv;
} Movement;

typedef struct
{
  bool wasMoved;
  MouseMovementCalibr mv;
} MovementCalibr;

typedef struct
{
  uint16_t min;
  uint16_t max;
  uint16_t center;
  uint16_t deadzone;
} JoystickRangeAxis;

typedef struct
{
  JoystickRangeAxis x;
  JoystickRangeAxis y;
} JoystickRange;

class BJoystick
{
  private:
  static TaskHandle_t normAdTaskHandle;     //! only one times
  static TaskHandle_t calibreTaskHandle;    //! only one times
  static TaskHandle_t calibreADTaskHandle;  //! only one times
  static SemaphoreHandle_t joySem;          //! is access to joystick values free?
  static MouseMovement mMovement;
  static MouseMovementCalibr mMovementCalibr;
  static JoystickRange range;
  static volatile bool moved;
  static volatile bool movedCalibr;

  public:
  static void begin();
  static void end();
  static bool calibreStick();
  static void setCalibre( JoystickRange & );
  static Movement getMovement();
  static MovementCalibr getMovementCalibr();

  private:
  static void normAdTask( void * );   //! the task for joystick work
  static void caliberTask( void * );  //! the task for joystick calibre
  static void adCalTask( void * );    //! the task for joystick calibre
};
