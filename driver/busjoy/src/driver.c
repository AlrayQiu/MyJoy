
#include <bus_driver.h>

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#endif
NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING RegistryPath)
{
    NTSTATUS          status = STATUS_SUCCESS;
    WDF_DRIVER_CONFIG config;

    KdPrint(("BUSJOY Driver Entry\r\n"));

    WDF_DRIVER_CONFIG_INIT(&config, BusDeviceAdd);

    status =
        WdfDriverCreate(pDriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);

    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfDriverCreate Failed 0x%x\r\n", status));
        return status;
    }

    return status;
}