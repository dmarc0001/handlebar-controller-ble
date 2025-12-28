#pragma once

#include "appPreferences.hpp"
#include <HIDTypes.h>

// Report IDs:
constexpr uint8_t MOUSE_ID = 0x01;
constexpr uint8_t KEYBOARD_ID = 0x02;
constexpr uint8_t MEDIA_KEYS_ID = 0x03;

extern uint8_t *_hidReportDescriptorPtr;
extern size_t hidReportDescriptorSize;
