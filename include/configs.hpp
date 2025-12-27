#pragma once
#include <Arduino.h>
#include "BleCombo.hpp"
#include "led.hpp"

using bleComboSptr = std::shared_ptr< class BleCombo >;

namespace config
{
  class ConfigObj
  {
    public:
    static bleComboSptr combo;
    static LedSptr led;
  };
}  // namespace config