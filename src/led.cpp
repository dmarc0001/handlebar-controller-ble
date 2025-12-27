#include <led.hpp>
#include "appPreferences.hpp"
// TODO: Grüne LED in Reihe mit VCC joystick
namespace neopixel
{
  OnboardLed::OnboardLed( uint8_t lpin ) : pin( lpin )
  {
    Logger.debug( prefs::MYLOG, "INI LED object..." );
    color_off = getColor( 0, 0, 0 );
    color_white = getColor( 128, 128, 128 );
    color_blue = getColor( 0, 0, 128 );
    color_red = getColor( 180, 0, 0 );
    color_yellow = getColor( 128, 128, 0 );
    curr_color = getColor( 0, 0, 0 );
    Logger.debug( prefs::MYLOG, "INI LED object...OK" );
  }

  void OnboardLed::begin()
  {
    Logger.debug( prefs::MYLOG, "begin LED object..." );
    Logger.debug( prefs::MYLOG, "begin LED object...OK" );
  }

  PColor OnboardLed::getColor( uint8_t red, uint8_t green, uint8_t blue )
  {
    PColor myColor = red << 16 | green << 8 | blue;
    return myColor;
  }

  ColorSetStructPtr OnboardLed::getColorSet( PColor color )
  {
    ColorSetStructPtr cs = std::make_shared< ColorSetStruct >();
    cs->red = ( color >> 16 ) & 0xFF;
    cs->green = ( color >> 8 ) & 0xFF;
    cs->blue = color & 0xFF;
    return cs;
  }

  void OnboardLed::setColor( uint8_t r, uint8_t g, uint8_t b )
  {
    if( curr_color == getColor( r, g, b ) )
    {
      return;
    }
    curr_color = getColor( r, g, b );
    neopixelWrite( pin, g, r, b );
  }

  void OnboardLed::setColor( PColor color )
  {
    if( curr_color == color )
    {
      return;
    }
    curr_color = color;
    neopixelWrite( pin, ( color >> 8 ) & 0xFF, ( color >> 16 ) & 0xFF, color & 0xFF );
  }

  void OnboardLed::setColor( ColorSetStruct& color)
  {
    if( curr_color == getColor( color.red, color.green, color.blue ) )
    {
      return;
    }
    curr_color = getColor( color.red, color.green, color.blue );
    neopixelWrite( pin, color.green, color.red, color.blue );
  }

  void OnboardLed::clear()
  {
    curr_color = color_off;
    neopixelWrite( pin, 0, 0, 0 );
  }

}  // namespace neopixel
