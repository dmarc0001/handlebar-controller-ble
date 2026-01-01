#include "joystick.hpp"
#include <Elog.h>
//
// instnce members
//
TaskHandle_t BJoystick::normAdTaskHandle{ nullptr };
TaskHandle_t BJoystick::calibreTaskHandle{ nullptr };
TaskHandle_t BJoystick::calibreADTaskHandle{ nullptr };
SemaphoreHandle_t BJoystick::joySem{ nullptr };
MouseMovement BJoystick::mMovement{ 0 };
MouseMovementCalibr BJoystick::mMovementCalibr{ 0 };
JoystickRange BJoystick::range{ 0 };
bool volatile BJoystick::moved{ false };
bool volatile BJoystick::movedCalibr{ false };

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
  memset( static_cast< void * >( mMovementCalibr ), 0, sizeof( MouseMovementCalibr ) );
  //
  // defaults set
  //
  BJoystick::range.x.min = prefs::DEFAULT_STICK_MIN;
  BJoystick::range.x.max = prefs::DEFAULT_STICK_MAX;
  BJoystick::range.x.center = prefs::DEFAULT_STICK_CENTER;
  BJoystick::range.x.deadzone = prefs::DEFAULT_STICK_DEADZONE;
  BJoystick::range.y.min = prefs::DEFAULT_STICK_MIN;
  BJoystick::range.y.max = prefs::DEFAULT_STICK_MAX;
  BJoystick::range.y.center = prefs::DEFAULT_STICK_CENTER;
  BJoystick::range.y.deadzone = prefs::DEFAULT_STICK_DEADZONE;
  //
  if ( BJoystick::normAdTaskHandle )
  {
    vTaskDelete( BJoystick::normAdTaskHandle );
    BJoystick::normAdTaskHandle = nullptr;
  }
  xTaskCreate( BJoystick::normAdTask, "norm-task", configMINIMAL_STACK_SIZE * 4, nullptr, tskIDLE_PRIORITY,
               &BJoystick::normAdTaskHandle );
}

/**
 * end the object tasks
 */
void BJoystick::end()
{
  if ( BJoystick::normAdTaskHandle )
  {
    vTaskDelete( BJoystick::normAdTaskHandle );
    BJoystick::normAdTaskHandle = nullptr;
  }
  if ( BJoystick::calibreTaskHandle )
  {
    vTaskDelete( BJoystick::calibreTaskHandle );
    BJoystick::calibreTaskHandle = nullptr;
  }
  if ( BJoystick::calibreADTaskHandle )
  {
    vTaskDelete( BJoystick::calibreADTaskHandle );
    BJoystick::calibreADTaskHandle = nullptr;
  }
}

/**
 * set the valubre values
 */
void BJoystick::setCalibre( JoystickRange &_jr )
{
  BJoystick::range.x.min = _jr.x.min;
  BJoystick::range.x.max = _jr.x.max;
  BJoystick::range.x.center = _jr.x.center;
  BJoystick::range.x.deadzone = _jr.x.deadzone;
  BJoystick::range.y.min = _jr.y.min;
  BJoystick::range.y.max = _jr.y.max;
  BJoystick::range.y.center = _jr.y.center;
  BJoystick::range.y.deadzone = _jr.y.deadzone;
}

/**
 * calibre the stick, start and end tasks for it, return to caller
 */
bool BJoystick::calibreStick()
{
  bool retVal{ false };

  Logger.info( prefs::MYLOG, "beginn joystick calibration..." );
  Logger.debug( prefs::MYLOG, "suspend joystick worker task..." );
  if ( BJoystick::normAdTaskHandle )
  {
    vTaskSuspend( BJoystick::normAdTaskHandle );
  }
  delay( 100 );
  xSemaphoreGive( BJoystick::joySem );
  // no movements in direction host
  BJoystick::moved = false;
  BJoystick::movedCalibr = false;
  if ( BJoystick::calibreTaskHandle )
  {
    vTaskDelete( BJoystick::calibreTaskHandle );
    BJoystick::calibreTaskHandle = nullptr;
    delay( 20 );
  }
  Logger.debug( prefs::MYLOG, "start joystick caliber task  (calibre-task)..." );
  xTaskCreate( BJoystick::caliberTask, "calibre-task", configMINIMAL_STACK_SIZE * 6, nullptr, tskIDLE_PRIORITY,
               &BJoystick::calibreTaskHandle );
  delay( 2 );
  return retVal;
}

/**
 * return the movement for the mouse, if there was an movement at all
 */
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
        if ( 0 != BUTTONS )
          // aLL BUT buttons DELETE
          BJoystick::mMovement[ i ] = 0;
      }
      // in object mark as no moved
      BJoystick::moved = false;
      xSemaphoreGive( BJoystick::joySem );
      // return movement
      return movement;
    }
  }
  // if not get the semaphore or was no movement
  movement.wasMoved = false;
  return movement;
}

/**
 * get an movement for the calibration routine, if thte was an value at all
 */
MovementCalibr BJoystick::getMovementCalibr()
{
  MovementCalibr movement{ 0 };

  if ( BJoystick::movedCalibr )
  {
    if ( xSemaphoreTake( BJoystick::joySem, pdMS_TO_TICKS( 10 ) ) == pdTRUE )
    {
      // mark as moved
      movement.wasMoved = true;
      // copy data
      for ( int i = 0; i < sizeof( MouseMovement ); i++ )
      {
        movement.mv[ i ] = BJoystick::mMovementCalibr[ i ];
        if ( 0 != BUTTONS )
          // aLL BUT buttons DELETE
          BJoystick::mMovementCalibr[ i ] = 0;
      }
      // in object mark as no moved
      BJoystick::movedCalibr = false;
      // free semaphore
      xSemaphoreGive( BJoystick::joySem );
      // return movement
      return movement;
    }
    Logger.warning( prefs::MYLOG, "BJoystick::getMovementCalibr can't get semaphore!" );
  }
  // if not get the semaphore or was no movement
  movement.wasMoved = false;
  return movement;
}

/**
 * the "normal" task who read the joystick values and buttons
 */
void BJoystick::normAdTask( void * )
{
  static uint8_t buttonPressed{ 0 };
  uint16_t xValue = 0;
  uint16_t yValue = 0;

  //
  Logger.registerSerial( prefs::TASKLOG, prefs::LOG_LEVEL, "tsk" );  // We want messages with DEBUG level and lower
  Logger.debug( prefs::TASKLOG, "joystick normal worker task started..." );

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
          Logger.debug( prefs::TASKLOG, "joystick button pressed..." );
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
        Logger.debug( prefs::TASKLOG, "joystick button released..." );
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
    xValue = analogRead( prefs::X_AXIS_PIN );
    yValue = analogRead( prefs::Y_AXIS_PIN );

    // x axis check
    if ( xValue < ( range.x.center - range.x.deadzone ) )
    {
      // left
      xStep = 0 - static_cast< int8_t >( ( range.x.center - xValue ) >> prefs::MOUSE_MOVE_STEP_SHIFT );
    }
    else if ( xValue > ( range.x.center + range.x.deadzone ) )
    {
      // right
      xStep = static_cast< int8_t >( ( xValue - range.x.center ) >> prefs::MOUSE_MOVE_STEP_SHIFT );
    }
    // y axis check
    if ( yValue < ( range.y.center - range.y.deadzone ) )
    {
      // down
      yStep = 0 - static_cast< int8_t >( ( range.y.center - yValue ) >> prefs::MOUSE_MOVE_STEP_SHIFT );
    }
    else if ( yValue > ( range.y.center + range.y.deadzone ) )
    {
      // up
      yStep = static_cast< int8_t >( ( yValue - range.y.center ) >> prefs::MOUSE_MOVE_STEP_SHIFT );
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
    //
    // if there tasks alive, wherre should killed
    //
    if ( BJoystick::calibreTaskHandle )
    {
      Logger.debug( prefs::TASKLOG, "kill calibre  task ..." );
      vTaskDelete( BJoystick::calibreTaskHandle );
      BJoystick::calibreTaskHandle = nullptr;
      delay( 2 );
    }
    if ( BJoystick::calibreADTaskHandle )
    {
      Logger.debug( prefs::TASKLOG, "kill calibre AD task ..." );
      vTaskDelete( BJoystick::calibreADTaskHandle );
      BJoystick::calibreADTaskHandle = nullptr;
      delay( 2 );
    }
    delay( prefs::JOYSTICK_MEASURE_RTIME_DELTA_MS );
  }
}

/**
 * the task who make the calibration
 */
void BJoystick::caliberTask( void * )
{
  //
  // define many variables local if task is running
  //
  bool calibreMax{ true };
  bool newValue{ false };
  uint16_t xMin{ 0xffff };
  uint16_t yMin{ 0xffff };
  uint16_t xMax{ 0 };
  uint16_t yMax{ 0 };
  uint16_t center_xMin{ 0xffff };
  uint16_t center_yMin{ 0xffff };
  uint16_t center_xMax{ 0 };
  uint16_t center_yMax{ 0 };
  delay( 100 );
  //
  Logger.registerSerial( prefs::CALLOG, prefs::LOG_LEVEL, "cal" );  // We want messages with DEBUG level and lower
  Logger.debug( prefs::CALLOG, "joystick calibre task started..." );
  //
  // start joystick calibre AD task
  //
  Logger.debug( prefs::MYLOG, "start joystick calibr AD (calibr-ad-task)..." );
  if ( BJoystick::calibreADTaskHandle )
  {
    vTaskDelete( BJoystick::calibreADTaskHandle );
    BJoystick::calibreADTaskHandle = nullptr;
    delay( 20 );
  }
  xTaskCreate( BJoystick::adCalTask, "calibr-ad-task", configMINIMAL_STACK_SIZE * 4, nullptr, tskIDLE_PRIORITY,
               &BJoystick::calibreADTaskHandle );
  delay( 100 );

  Logger.info( prefs::CALLOG, "START CALIBRATING STICK" );
  Logger.info( prefs::CALLOG, "turn joystick arround, many times unless press button..." );

  delay( 10 );
  while ( calibreMax )
  {
    MovementCalibr mv = BJoystick::getMovementCalibr();
    if ( mv.wasMoved )
    {
      if ( mv.mv[ BUTTONS ] & MOUSE_LEFT )
      {
        Logger.debug( prefs::CALLOG, "Button, calibre max success..." );
        Logger.info( prefs::CALLOG, "Calibre min/max success, let stick in neutral position..." );
        calibreMax = false;
      }
      else
      {
        if ( mv.mv[ X_MOVE ] < xMin )
        {
          // lower x found
          xMin = mv.mv[ X_MOVE ];
          newValue = true;
        }
        if ( mv.mv[ X_MOVE ] > xMax )
        {
          // higher x found
          xMax = mv.mv[ X_MOVE ];
          newValue = true;
        }

        if ( mv.mv[ Y_MOVE ] < yMin )
        {
          // lower y found
          yMin = mv.mv[ Y_MOVE ];
          newValue = true;
        }
        if ( mv.mv[ Y_MOVE ] > yMax )
        {
          // higher y found
          yMax = mv.mv[ Y_MOVE ];
          newValue = true;
        }
        if ( newValue )
        {
          Logger.info( prefs::CALLOG, "Found new value: xMin: %04d xMax: %04d, yMin: %04d, yMax: %04d", xMin, xMax, yMin, yMax );
          newValue = false;
        }
      }
      delay( 15 );
    }
  }
  //
  // i know min and max, now letzt see whats the middle is
  //
  Logger.debug( prefs::CALLOG, "calibre neutral, no moving stick, let's in the middle..." );
  delay( 5000 );
  uint32_t end_cal = millis() + 5000;
  while ( millis() < end_cal )
  {
    MovementCalibr mv = BJoystick::getMovementCalibr();
    if ( mv.wasMoved )
    {
      if ( mv.mv[ X_MOVE ] < center_xMin )
      {
        // lower x found
        center_xMin = mv.mv[ X_MOVE ];
        newValue = true;
      }
      if ( mv.mv[ X_MOVE ] > center_xMax )
      {
        // higher x found
        center_xMax = mv.mv[ X_MOVE ];
        newValue = true;
      }

      if ( mv.mv[ Y_MOVE ] < center_yMin )
      {
        // lower y found
        center_yMin = mv.mv[ Y_MOVE ];
        newValue = true;
      }
      if ( mv.mv[ Y_MOVE ] > center_yMax )
      {
        // higher y found
        center_yMax = mv.mv[ Y_MOVE ];
        newValue = true;
      }
      if ( newValue )
      {
        Logger.info( prefs::CALLOG, "Found new center value: xMin: %04d xMax: %04d, yMin: %04d, yMax: %04d", center_xMin, center_xMax,
                     center_yMin, center_yMax );
        uint32_t end_cal = millis() + 5000;
        newValue = false;
      }
      // Logger.debug( prefs::CALLOG, "Joystick moved!" );
      delay( 15 );
    }
  }
  //
  // center ready
  //
  // make calibre array
  //
  JoystickRange range;
  range.x.min = xMin;
  range.x.max = xMax;
  range.x.center = ( center_xMin + center_xMax ) >> 1;
  range.x.deadzone = ( ( center_xMax - center_yMin ) > ( prefs::DEFAULT_STICK_DEADZONE / 2 ) ) ? ( prefs::DEFAULT_STICK_DEADZONE << 1 )
                                                                                               : prefs::DEFAULT_STICK_DEADZONE;
  range.y.min = yMin;
  range.y.max = yMax;
  range.y.center = ( center_yMin + center_yMax ) >> 1;
  range.y.deadzone = ( ( center_yMax - center_yMin ) > ( prefs::DEFAULT_STICK_DEADZONE / 2 ) ) ? ( prefs::DEFAULT_STICK_DEADZONE << 1 )
                                                                                               : prefs::DEFAULT_STICK_DEADZONE;
  // set in object
  BJoystick::setCalibre( range );
  //
  // DEBUG Calibrierung
  //
  Logger.debug( prefs::CALLOG, "Calibrated:" );
  Logger.debug( prefs::CALLOG, "x-min: %04d:", range.x.min );
  Logger.debug( prefs::CALLOG, "x-max: %04d:", range.x.max );
  Logger.debug( prefs::CALLOG, "x-center: %04d:", range.x.center );
  Logger.debug( prefs::CALLOG, "x-daed-zone: %04d:", range.x.deadzone );
  Logger.debug( prefs::CALLOG, "y-min: %04d:", range.y.min );
  Logger.debug( prefs::CALLOG, "y-max: %04d:", range.y.max );
  Logger.debug( prefs::CALLOG, "y-center: %04d:", range.y.center );
  Logger.debug( prefs::CALLOG, "y-daed-zone: %04d:", range.y.deadzone );
  Logger.debug( prefs::CALLOG, "===" );
  delay( 300 );
  //
  // Kill joystick calibrating task
  //
  Logger.debug( prefs::CALLOG, "kill joystick calibr task..." );
  delay( 80 );
  if ( BJoystick::calibreADTaskHandle )
  {
    vTaskDelete( BJoystick::calibreADTaskHandle );
    BJoystick::calibreADTaskHandle = nullptr;
    delay( 10 );
  }
  //
  // restart joystick normal task
  //
  Logger.debug( prefs::CALLOG, "restart joystick worker task..." );
  delay( 10 );
  if ( BJoystick::normAdTaskHandle )
    vTaskResume( BJoystick::normAdTaskHandle );
  // wait if the worker task kill me
  while ( true )
    delay( 2 );
}

/**
 * the task who make the AD conversation for the joystick to made the calibration
 */
void BJoystick::adCalTask( void * )
{
  static uint8_t buttonPressed{ 0 };

  uint16_t xValue = 0;
  uint16_t yValue = 0;

  //
  // Logger.registerSerial( prefs::TASKLOG, prefs::LOG_LEVEL, "tsk" );  // We want messages with DEBUG level and lower
  Logger.debug( prefs::TASKLOG, "joystick calibre ad task started..." );
  memset( static_cast< void * >( mMovementCalibr ), 0, sizeof( MouseMovementCalibr ) );
  delay( 10 );
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
          Logger.debug( prefs::TASKLOG, "ctask - joystick button pressed..." );
          if ( xSemaphoreTake( BJoystick::joySem, pdMS_TO_TICKS( 10 ) ) == pdTRUE )
          {
            // set left mouse button pressed
            mMovementCalibr[ BUTTONS ] |= MOUSE_LEFT;
            BJoystick::movedCalibr = true;
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
        Logger.debug( prefs::TASKLOG, "ctask - joystick button released..." );
        buttonPressed = 0;
        if ( xSemaphoreTake( BJoystick::joySem, pdMS_TO_TICKS( 10 ) ) == pdTRUE )
        {
          // set left mouse button released
          mMovementCalibr[ BUTTONS ] &= ~MOUSE_LEFT;
          BJoystick::movedCalibr = true;
          xSemaphoreGive( BJoystick::joySem );
        }
      }
      else
        buttonPressed = 0;
    }

    //
    // joystick movement measure
    //

    uint16_t xStep = 0;
    uint16_t yStep = 0;
    xValue = analogRead( prefs::X_AXIS_PIN );
    yValue = analogRead( prefs::Y_AXIS_PIN );

    if ( xSemaphoreTake( BJoystick::joySem, pdMS_TO_TICKS( 10 ) ) == pdTRUE )
    {
      BJoystick::mMovementCalibr[ X_MOVE ] = xValue;
      BJoystick::mMovementCalibr[ Y_MOVE ] = yValue;
      BJoystick::movedCalibr = true;
      xSemaphoreGive( BJoystick::joySem );
    }
    else
    {
      Logger.warning( prefs::TASKLOG, "ctask - can't taking semaphore!" );
    }
    // Logger.debug( prefs::TASKLOG, "mouse calibre moved..." );

    delay( prefs::JOYSTICK_MEASURE_RTIME_DELTA_MS );
  }
}
