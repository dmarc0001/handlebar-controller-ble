#pragma once
#include <esp_sleep.h>

enum esp_sleep_type
{
  SLEEP_LIGHT,
  SLEEP_DEEP
};


void setup();
void loop();
void blink_blue_led( uint8_t, uint32_t _interval = 150 );
esp_sleep_wakeup_cause_t make_sleep( esp_sleep_type );
