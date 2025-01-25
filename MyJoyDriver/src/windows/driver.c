
#include <ntstatus.h>
#include <windows.h>

#include <wdf.h>

#include "device.h"
#include "utils.h"
#include "driver.h"

NTSTATUS DriverEntry(IN PDRIVER_OBJECT  DriverObject,
                     IN PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status = STATUS_SUCCESS;

    WDF_DRIVER_CONFIG config;
    WDF_DRIVER_CONFIG_INIT(&config, MyJoyDeviceAdd);

    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             WDF_NO_OBJECT_ATTRIBUTES,
                             &config,
                             WDF_NO_HANDLE);
    ASSERT_STATUS_SUCCESS(status, "WdfDriverCreate failed");

    return status;
}
