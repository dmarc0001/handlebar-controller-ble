#pragma once
#include <Preferences.h>
#include "joystick.hpp"
#include <memory>

class CurrPrefs;

using CurrPrefsPtr = std::shared_ptr< CurrPrefs >;

class CurrPrefs : public Preferences
{
  private:

  public:
  CurrPrefs();

  JoystickRange getJoystickRange();
  bool setJoystickRange( JoystickRange& );

// typedef struct
// {
//   uint16_t min;
//   uint16_t max;
//   uint16_t center;
//   uint16_t deadzone;
// } JoystickRangeAxis;

// typedef struct
// {
//   JoystickRangeAxis x;
//   JoystickRangeAxis y;
// } JoystickRange;

};
