#pragma once

#ifdef MYJOY_DEFAULT_HID
#include "./default/myjoy_default_hid_def.h"
#else
#include "./default/myjoy_default_hid_def.h"
#endif

#define MYJOY_VID 0xA1A1
#define MYJOY_PID 0x0001
#define MYJOY_VERSION 0x0001

#define MYJOY_MANUFACTURER_STRING L"Alray Qiu @2025"
#define MYJOY_PRODUCT_STRING L"MYJOY test driver v1"
#define MYJOY_SERIAL_NUMBER_STRING L"i2misaka0mikoto&goto1hitori"
#define MYJOY_DEVICE_STRING \
    L"Virtual hid device driver,for now, only joystick usable"
#define MYJOY_DEVICE_STRING_INDEX 5

ELIMINATE_UNUSED_HEADER_FILE_WARNINGS

#undef ELIMINATE_UNUSED_HEADER_FILE_WARNINGS
#define ELIMINATE_UNUSED_HEADER_FILE_WARNINGS()