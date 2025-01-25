
#pragma once

#include <Windows.h>
#include <wdf.h>

#include <hidport.h>

#include <usb.h>
#include <wdfusb.h>

DRIVER_INITIALIZE              DriverEntry;
EVT_WDF_OBJECT_CONTEXT_CLEANUP MyJoyDriverContextCleanup;