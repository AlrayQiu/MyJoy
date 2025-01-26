
#pragma once

#include <ntddk.h>
#include <wdf.h>

DRIVER_INITIALIZE              DriverEntry;
EVT_WDF_OBJECT_CONTEXT_CLEANUP MyJoyDriverContextCleanup;