#include "joystick.hpp"
#include <Elog.h>

TaskHandle_t BJoystick::taskHandle{ nullptr };
SemaphoreHandle_t BJoystick::joySem{ nullptr };
MouseMovement BJoystick::mMovement{ 0 };
bool BJoystick::moved{ false };
uint16_t BJoystick::xCenter{ prefs::DEFAULT_STICK_CENTER };
uint16_t BJoystick::yCenter{ prefs::DEFAULT_STICK_CENTER };

/**
 * start the task
 */
void BJoystick::begin()
{
  Logger.debug( prefs::MYLOG, "beginn joystick..." );
  pinMode( prefs::BUTTON_PIN, INPUT_PULLUP );
  adcAttachPin( prefs::X_AXIS_PIN );
  adcAttachPin( prefs::Y_AXIS_PIN );
  analogSetAttenuation( ADC_11db );
  //
  // 0 .. 1530 x/x
  // round about 765 = center
  //
  //
  // init semaphore for access
  //
  vSemaphoreCreateBinary( BJoystick::joySem );
  // set movenents to 0
  memset( static_cast< void * >( mMovement ), 0, sizeof( MouseMovement ) );

  if ( BJoystick::taskHandle )
  {
    vTaskDelete( BJoystick::taskHandle );
    BJoystick::taskHandle = nullptr;
  }
  else
  {
    xTaskCreate( BJoystick::mTask, "j-task", configMINIMAL_STACK_SIZE * 4, nullptr, tskIDLE_PRIORITY, &BJoystick::taskHandle );
  }
}

void BJoystick::end()
{
  if ( BJoystick::taskHandle )
  {
    vTaskDelete( BJoystick::taskHandle );
    BJoystick::taskHandle = nullptr;
  }
}

Movement BJoystick::getMovement()
{
  Movement movement{ 0 };

  if ( BJoystick::moved )
  {
    if ( xSemaphoreTake( BJoystick::joySem, pdMS_TO_TICKS( 10 ) ) == pdTRUE )
    {
      // mark as moved
      movement.wasMoved = true;
      // copy data
      for ( int i = 0; i < sizeof( MouseMovement ); i++ )
      {
        movement.mv[ i ] = BJoystick::mMovement[ i ];
        if( 0 != BUTTONS )
          // aLL BUT buttons DELETE
          BJoystick::mMovement[ i ] = 0;
      }
      // in object mark as no moved
      BJoystick::moved = false;
      // delete data
      // memset( static_cast< void * >( mMovement ), 0, sizeof( MouseMovement ) );
      // buttons save...
      // BJoystick::mMovement[BUTTONS] = movement.mv[BUTTONS];
      // free semaphore
      xSemaphoreGive( BJoystick::joySem );
      // return movement
      return movement;
    }
  }
  // if not get the semaphore or was no movement
  movement.wasMoved = false;
  return movement;
}

void BJoystick::mTask( void * )
{
  static uint8_t buttonPressed{ 0 };
  uint16_t xValue = 0;
  uint16_t yValue = 0;

  //
  Logger.registerSerial( prefs::TASKLOG, prefs::LOG_LEVEL, "task" );  // We want messages with DEBUG level and lower
  Logger.debug( prefs::TASKLOG, "joystick task started..." );

  while ( true )
  {
    //
    // check values on the joystick and the button
    //

    //
    // Mouse Button Check
    //
    if ( digitalRead( prefs::BUTTON_PIN ) == LOW )
    {
      // button is pressed
      if ( buttonPressed < 2 )
      {
        ++buttonPressed;
      }
      else
      {
        if ( buttonPressed < 3 )
        {
          // Logger.debug( prefs::TASKLOG, "joystick button pressed..." );
          if ( xSemaphoreTake( BJoystick::joySem, pdMS_TO_TICKS( 10 ) ) == pdTRUE )
          {
            // set left mouse button pressed
            mMovement[ BUTTONS ] |= MOUSE_LEFT;
            BJoystick::moved = true;
            xSemaphoreGive( BJoystick::joySem );
          }
          // after this buttonPresses is set to 3 to avoid multiple press events
          ++buttonPressed;
        }
      }
    }
    else
    {
      // button is not pressed
      if ( buttonPressed > 2 )
      {
        // Logger.debug( prefs::TASKLOG, "joystick button released..." );
        buttonPressed = 0;
        if ( xSemaphoreTake( BJoystick::joySem, pdMS_TO_TICKS( 10 ) ) == pdTRUE )
        {
          // set left mouse button released
          mMovement[ BUTTONS ] &= ~MOUSE_LEFT;
          BJoystick::moved = true;
          xSemaphoreGive( BJoystick::joySem );
        }
      }
      else
        buttonPressed = 0;
    }

    //
    // joystick movement test
    //

    uint16_t xStep = 0;
    uint16_t yStep = 0;
    // x axis check
    xValue = analogRead( prefs::X_AXIS_PIN );
    if ( xValue < ( xCenter - prefs::STICK_DEADZONE ) )
    {
      // left
      xStep = 0 - static_cast< int8_t >( ( xCenter - xValue ) >> prefs::MOUSE_MOVE_STEP_SHIFT );
    }
    else if ( xValue > ( xCenter + prefs::STICK_DEADZONE ) )
    {
      // right
      xStep = static_cast< int8_t >( ( xValue - xCenter ) >> prefs::MOUSE_MOVE_STEP_SHIFT );
    }

    // y axis check
    yValue = analogRead( prefs::Y_AXIS_PIN );
    if ( yValue < ( yCenter - prefs::STICK_DEADZONE ) )
    {
      // down
      yStep = 0 - static_cast< int8_t >( ( yCenter - yValue ) >> prefs::MOUSE_MOVE_STEP_SHIFT );
    }
    else if ( yValue > ( yCenter + prefs::STICK_DEADZONE ) )
    {
      // up
      yStep = static_cast< int8_t >( ( yValue - yCenter ) >> prefs::MOUSE_MOVE_STEP_SHIFT );
    }
    //
    // is there ad step to do?
    //
    if ( ( xStep != 0 ) || ( yStep != 0 ) )
    {
      if ( xSemaphoreTake( BJoystick::joySem, pdMS_TO_TICKS( 10 ) ) == pdTRUE )
      {
        BJoystick::mMovement[ X_MOVE ] = xStep;
        BJoystick::mMovement[ Y_MOVE ] = yStep;
        BJoystick::moved = true;
        xSemaphoreGive( BJoystick::joySem );
      }
      // Logger.debug( prefs::TASKLOG, "mouse moved..." );
    }

    delay( prefs::JOYSTICK_MEASURE_RTIME_DELTA_MS );
  }
}
