// Copy Form
// https://github.com/microsoft/Windows-driver-samples/blob/main/general/toaster/umdf2/inc/public.h

#pragma once

#include <windows.h>

typedef struct _BUSENUM_PLUGIN_HARDWARE
{
    //
    // sizeof (struct _BUSENUM_HARDWARE)
    //
    IN ULONG Size;

    //
    // Unique serial number of the device to be enumerated.
    // Enumeration will be failed if another device on the
    // bus has the same serail number.
    //

    IN ULONG SerialNo;

//
// An array of (zero terminated wide character strings). The array itself
//  also null terminated (ie, MULTI_SZ)
//
#pragma warning(disable : 4200)  // nonstandard extension used

    IN WCHAR HardwareIDs[];

#pragma warning(default : 4200)

} BUSENUM_PLUGIN_HARDWARE, *PBUSENUM_PLUGIN_HARDWARE;

typedef struct _BUSENUM_UNPLUG_HARDWARE
{
    //
    // sizeof (struct _REMOVE_HARDWARE)
    //

    IN ULONG Size;

    //
    // Serial number of the device to be plugged out
    //

    ULONG SerialNo;

    ULONG Reserved[2];

} BUSENUM_UNPLUG_HARDWARE, *PBUSENUM_UNPLUG_HARDWARE;

typedef struct _BUSENUM_EJECT_HARDWARE
{
    //
    // sizeof (struct _EJECT_HARDWARE)
    //

    IN ULONG Size;

    //
    // Serial number of the device to be ejected
    //

    ULONG SerialNo;

    ULONG Reserved[2];

} BUSENUM_EJECT_HARDWARE, *PBUSENUM_EJECT_HARDWARE;

#define FILE_DEVICE_BUSENUM FILE_DEVICE_BUS_EXTENDER
#define BUSENUM_IOCTL(_index_) \
    CTL_CODE(FILE_DEVICE_BUSENUM, _index_, METHOD_BUFFERED, FILE_READ_DATA)
#define IOCTL_BUSENUM_PLUGIN_HARDWARE BUSENUM_IOCTL(0x0)
#define IOCTL_BUSENUM_UNPLUG_HARDWARE BUSENUM_IOCTL(0x1)
#define IOCTL_BUSENUM_EJECT_HARDWARE BUSENUM_IOCTL(0x2)
#define IOCTL_BUSJOY_DONT_DISPLAY_IN_UI_DEVICE BUSENUM_IOCTL(0x3)

#define BUS_HARDWARE_IDS L"{2163FA90-57F7-9801-A2B0-AEC1E5FBFEEA}\\BusJoy\0"
#define BUS_HARDWARE_IDS_LENGTH sizeof(BUS_HARDWARE_IDS)

#define BUSENUM_COMPATIBLE_IDS \
    L"{2163FA90-57F7-9801-A2B0-AEC1E5FBFEEA}\\CompatibleBusJoy\0"
#define BUSENUM_COMPATIBLE_IDS_LENGTH sizeof(BUSENUM_COMPATIBLE_IDS)

#define BUS_TAG 'EsuB'
#define BUSRESOURCENAME L"BusJoy_MOF_Resources"

#define MAX_INSTANCE_ID_LEN 80

#include "initguid.h"

//{9FA5FE9B-1435-F489-A37F-E9E243175929}
DEFINE_GUID(GUID_DEVINTERFACE_BUSJOY,
            0x9FA5FE9B,
            0x1435,
            0xF489,
            0xA3,
            0x7F,
            0xE9,
            0xE2,
            0x43,
            0x17,
            0x59,
            0x29);
//{2163FA90-57F7-9801-A2B0-AEC1E5FBFEEA}
DEFINE_GUID(GUID_DEVCLASS_BUSJOY,
            0x2163FA90,
            0x57F7,
            0x9801,
            0xA2,
            0xB0,
            0xAE,
            0xC1,
            0xE5,
            0xFB,
            0xFE,
            0xEA);
