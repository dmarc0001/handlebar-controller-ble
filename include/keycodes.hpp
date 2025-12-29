#pragma once


typedef struct
{
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} KeyReport;

typedef uint8_t MediaKeyReport[2];

constexpr uint8_t MOUSE_LEFT = 1;
constexpr uint8_t MOUSE_RIGHT = 2;
constexpr uint8_t MOUSE_MIDDLE = 4;
constexpr uint8_t MOUSE_BACK = 8; 
constexpr uint8_t MOUSE_FORWARD  = 16;
constexpr uint8_t MOUSE_ALL = (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE);
//
constexpr uint8_t KEY_LEFT_CTRL = 0x80;
constexpr uint8_t KEY_LEFT_SHIFT = 0x81;
constexpr uint8_t KEY_LEFT_ALT = 0x82;
constexpr uint8_t KEY_LEFT_GUI = 0x83;
constexpr uint8_t KEY_RIGHT_CTRL = 0x84;
constexpr uint8_t KEY_RIGHT_SHIFT = 0x85;
constexpr uint8_t KEY_RIGHT_ALT = 0x86;
constexpr uint8_t KEY_RIGHT_GUI = 0x87;
constexpr uint8_t KEY_UP_ARROW = 0xDA;
constexpr uint8_t KEY_DOWN_ARROW = 0xD9;
constexpr uint8_t KEY_LEFT_ARROW = 0xD8;
constexpr uint8_t KEY_RIGHT_ARROW = 0xD7;
constexpr uint8_t KEY_BACKSPACE = 0xB2;
constexpr uint8_t KEY_TAB = 0xB3;
constexpr uint8_t KEY_RETURN = 0xB0;
constexpr uint8_t KEY_ESC = 0xB1;
constexpr uint8_t KEY_INSERT = 0xD1;
constexpr uint8_t KEY_DELETE = 0xD4;
constexpr uint8_t KEY_PAGE_UP = 0xD3;
constexpr uint8_t KEY_PAGE_DOWN = 0xD6;
constexpr uint8_t KEY_HOME = 0xD2;
constexpr uint8_t KEY_END = 0xD5;
constexpr uint8_t KEY_CAPS_LOCK = 0xC1;
constexpr uint8_t KEY_F1 = 0xC2;
constexpr uint8_t KEY_F2 = 0xC3;
constexpr uint8_t KEY_F3 = 0xC4;
constexpr uint8_t KEY_F4 = 0xC5;
constexpr uint8_t KEY_F5 = 0xC6;
constexpr uint8_t KEY_F6 = 0xC7;
constexpr uint8_t KEY_F7 = 0xC8;
constexpr uint8_t KEY_F8 = 0xC9;
constexpr uint8_t KEY_F9 = 0xCA;
constexpr uint8_t KEY_F10 = 0xCB;
constexpr uint8_t KEY_F11 = 0xCC;
constexpr uint8_t KEY_F12 = 0xCD;
constexpr uint8_t KEY_F13 = 0xF0;
constexpr uint8_t KEY_F14 = 0xF1;
constexpr uint8_t KEY_F15 = 0xF2;
constexpr uint8_t KEY_F16 = 0xF3;
constexpr uint8_t KEY_F17 = 0xF4;
constexpr uint8_t KEY_F18 = 0xF5;
constexpr uint8_t KEY_F19 = 0xF6;
constexpr uint8_t KEY_F20 = 0xF7;
constexpr uint8_t KEY_F21 = 0xF8;
constexpr uint8_t KEY_F22 = 0xF9;
constexpr uint8_t KEY_F23 = 0xFA;
constexpr uint8_t KEY_F24 = 0xFB;

constexpr MediaKeyReport KEY_MEDIA_NEXT_TRACK = {1, 0};
constexpr MediaKeyReport KEY_MEDIA_PREVIOUS_TRACK = {2, 0};
constexpr MediaKeyReport KEY_MEDIA_STOP = {4, 0};
constexpr MediaKeyReport KEY_MEDIA_PLAY_PAUSE = {8, 0};
constexpr MediaKeyReport KEY_MEDIA_MUTE = {16, 0};
constexpr MediaKeyReport KEY_MEDIA_VOLUME_UP = {32, 0};
constexpr MediaKeyReport KEY_MEDIA_VOLUME_DOWN = {64, 0};
constexpr MediaKeyReport KEY_MEDIA_WWW_HOME = {128, 0};
constexpr MediaKeyReport KEY_MEDIA_LOCAL_MACHINE_BROWSER = {0, 1}; // Opens "My Computer" on Windows
constexpr MediaKeyReport KEY_MEDIA_CALCULATOR = {0, 2};
constexpr MediaKeyReport KEY_MEDIA_WWW_BOOKMARKS = {0, 4};
constexpr MediaKeyReport KEY_MEDIA_WWW_SEARCH = {0, 8};
constexpr MediaKeyReport KEY_MEDIA_WWW_STOP = {0, 16};
constexpr MediaKeyReport KEY_MEDIA_WWW_BACK = {0, 32};
constexpr MediaKeyReport KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION = {0, 64}; // Media Selection
constexpr MediaKeyReport KEY_MEDIA_EMAIL_READER = {0, 128};
