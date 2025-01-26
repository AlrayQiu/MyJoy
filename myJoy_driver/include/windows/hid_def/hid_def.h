#pragma once

#ifdef MYJOY_DEFAULT_HID
#include "./default/myjoy_default_hid_def.h"
#else
#include "./default/myjoy_default_hid_def.h"
#endif

#define MYJOY_VID     0xA1A1
#define MYJOY_PID     0x0001
#define MYJOY_VERSION 0x0001

#define MYJOY_MANUFACTURER_STRING  L"Alray Qiu @2025"
#define MYJOY_PRODUCT_STRING       L"MYJOY test driver v1"
#define MYJOY_SERIAL_NUMBER_STRING L"i2misaka0mikoto&goto1hitori"
#define MYJOY_DEVICE_STRING        L"Virtual hid device driver,for now, only joystick usable"
#define MYJOY_DEVICE_STRING_INDEX  5

ELIMINATE_UNUSED_HEADER_FILE_WARNINGS

#define CONTROL_COLLECTION_REPORT_ID 0x01
#define TEST_COLLECTION_REPORT_ID    0x02

#define HIDMINI_CONTROL_CODE_SET_ATTRIBUTES 0x00
#define HIDMINI_CONTROL_CODE_DUMMY1         0x01
#define HIDMINI_CONTROL_CODE_DUMMY2         0x02

typedef struct _MY_DEVICE_ATTRIBUTES
{
    USHORT VendorID;
    USHORT ProductID;
    USHORT VersionNumber;

} MY_DEVICE_ATTRIBUTES, *PMY_DEVICE_ATTRIBUTES;

typedef struct _HIDMINI_CONTROL_INFO
{
    //
    // report ID of the collection to which the control request is sent
    //
    UCHAR ReportId;

    //
    // One byte control code (user-defined) for communication with hid
    // mini driver
    //
    UCHAR ControlCode;

    //
    // This union contains input data for the control request.
    //
    union
    {
        MY_DEVICE_ATTRIBUTES Attributes;
        struct
        {
            ULONG Dummy1;
            ULONG Dummy2;
        } Dummy;
    } u;

} HIDMINI_CONTROL_INFO, *PMYJOY_CONTROL_INFO;

#undef ELIMINATE_UNUSED_HEADER_FILE_WARNINGS
#define ELIMINATE_UNUSED_HEADER_FILE_WARNINGS()