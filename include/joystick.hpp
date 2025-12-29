#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp32-hal-adc.h>
#include <freertos/event_groups.h>
#include <memory>
#include "appPreferences.hpp"
#include "keycodes.hpp"

using MouseMovement = uint8_t[5];

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


class BJoystick
{
  private:
  static uint16_t xCenter;
  static uint16_t yCenter;
  static TaskHandle_t taskHandle;   //! only one times
  static SemaphoreHandle_t joySem;  //! is access to joystick values free?
  static MouseMovement mMovement;
  static bool moved;

  public:
  static void begin();
  static void end();
  static Movement getMovement();

  private:
  static void mTask( void * );  //! the task for preasure
};
