#include "appPreferences.hpp"
#include "appCurrPrefs.hpp"
#

constexpr const char *WAS_INIT{ "wasInit" };
constexpr const char *JR_X_MIN{ "range_x_min" };
constexpr const char *JR_X_MAX{ "range_x_max" };
constexpr const char *JR_X_CENTER{ "range_x_cent" };
constexpr const char *JR_X_DAEDZONE{ "range_x_daed" };
constexpr const char *JR_Y_MIN{ "range_y_min" };
constexpr const char *JR_Y_MAX{ "range_y_max" };
constexpr const char *JR_Y_CENTER{ "range_y_cent" };
constexpr const char *JR_Y_DAEDZONE{ "range_y_daed" };

CurrPrefs::CurrPrefs()
{
  this->begin( prefs::APPNAME, false );
}

JoystickRange CurrPrefs::getJoystickRange()
{
  JoystickRange range{ 0 };
  range.x.min = getUShort( JR_X_MIN, prefs::DEFAULT_STICK_MIN );
  range.x.max = getUShort( JR_X_MAX, prefs::DEFAULT_STICK_MAX );
  range.x.center = getUShort( JR_X_CENTER, prefs::DEFAULT_STICK_CENTER );
  range.x.deadzone = getUShort( JR_X_DAEDZONE, prefs::DEFAULT_STICK_DEADZONE );
  range.y.min = getUShort( JR_Y_MIN, prefs::DEFAULT_STICK_MIN );
  range.y.max = getUShort( JR_Y_MAX, prefs::DEFAULT_STICK_MAX );
  range.y.center = getUShort( JR_Y_CENTER, prefs::DEFAULT_STICK_CENTER );
  range.y.deadzone = getUShort( JR_Y_DAEDZONE, prefs::DEFAULT_STICK_DEADZONE );
  return range;
}

bool CurrPrefs::setJoystickRange( JoystickRange& _ra)
{
  size_t sum {};
  sum +=  putUShort( JR_X_MIN, _ra.x.min );
  sum +=  putUShort( JR_X_MAX, _ra.x.max );
  sum +=  putUShort( JR_X_CENTER, _ra.x.center );
  sum +=  putUShort( JR_X_DAEDZONE, _ra.x.deadzone );
  sum +=  putUShort( JR_Y_MIN, _ra.y.min );
  sum +=  putUShort( JR_Y_MAX, _ra.y.max );
  sum +=  putUShort( JR_Y_CENTER, _ra.y.center );
  sum +=  putUShort( JR_Y_DAEDZONE, _ra.y.deadzone );
  if( sum != 16 )
  {
    Serial.println( "ERROR: write JoistickRange calibre data failed!");
    return false;
  }
  return true;
}

