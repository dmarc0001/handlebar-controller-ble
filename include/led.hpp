#pragma once
#include <memory>
#include <Arduino.h>

namespace neopixel
{
  class OnboardLed;
}  // namespace neopixel

using PColor = uint32_t;
using LedSptr = std::shared_ptr< class neopixel::OnboardLed >;

namespace neopixel
{
  using ColorSetStruct = struct
  {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };

  using ColorSetStructPtr = std::shared_ptr< ColorSetStruct >;

  class OnboardLed
  {
    private:
    uint8_t pin;
    PColor curr_color;

    public:
    PColor color_off;
    PColor color_white;
    PColor color_blue;
    PColor color_red;
    PColor color_yellow;

    public:
    OnboardLed( uint8_t );
    void begin();
    void setColor( uint8_t, uint8_t, uint8_t );
    void setColor( PColor );
    void setColor( ColorSetStruct& );
    PColor getColor( uint8_t, uint8_t, uint8_t );
    ColorSetStructPtr getColorSet( PColor color );
    void clear();
    void show();
  };

}  // namespace neopixel
