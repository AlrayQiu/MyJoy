

#include "device.h"
#include "utils.h"

#include <ntstatus.h>

#include <wdf.h>

#include <wdm.h>
#include <wdf.h>

#include <wpprecorder.h>

#define WPP_FLAG_LEVEL_LOGGER(flag, level)  WPP_LEVEL_LOGGER(flag)
#define WPP_FLAG_LEVEL_ENABLED(flag, level) WPP_LEVEL_ENABLED(flag, level)

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
    KdPrint(("MyJoy Driver Entry Start\r\n"));
    KdPrint(("4\r\n"));

    NTSTATUS          status = STATUS_SUCCESS;
    WDF_DRIVER_CONFIG config;

    ExInitializeDriverRuntime(DrvRtPoolNxOptIn);
    WDF_DRIVER_CONFIG_INIT(&config, MyJoyDeviceAdd);

    status =
        WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);
    ASSERT_STATUS_SUCCESS(status, "WdfDriverCreate failed");

    KdPrint(("MyJoy Driver Entry End:0x%x\r\n", status));
    return status;
}
