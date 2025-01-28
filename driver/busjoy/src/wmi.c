/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    WMI.C

Abstract:

    This module handles all the WMI Irps.

Environment:

    Kernel mode

--*/

#include "wmi.h"
#include "bus_driver.h"
#include "bus_public.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, BusWmiRegistration)
#pragma alloc_text(PAGE, BusEvtStdDataSetItem)
#pragma alloc_text(PAGE, BusEvtStdDataSetInstance)
#pragma alloc_text(PAGE, BusEvtStdDataQueryInstance)
#endif

NTSTATUS
BusWmiRegistration(WDFDEVICE Device)
{
    WDF_WMI_PROVIDER_CONFIG providerConfig;
    WDF_WMI_INSTANCE_CONFIG instanceConfig;
    PBUS_FDO_DEVICE_DATA    deviceData;
    NTSTATUS                status;
    DECLARE_CONST_UNICODE_STRING(busRsrcName, BUSRESOURCENAME);

    PAGED_CODE();

    deviceData = BusFdoGetData(Device);

    status = WdfDeviceAssignMofResourceName(Device, &busRsrcName);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    WDF_WMI_PROVIDER_CONFIG_INIT(&providerConfig, &BusjoyBusInformation_GUID);
    providerConfig.MinInstanceBufferSize = sizeof(BUS_WMI_STD_DATA);

    WDF_WMI_INSTANCE_CONFIG_INIT_PROVIDER_CONFIG(&instanceConfig, &providerConfig);

    instanceConfig.Register                    = TRUE;
    instanceConfig.EvtWmiInstanceQueryInstance = BusEvtStdDataQueryInstance;
    instanceConfig.EvtWmiInstanceSetInstance   = BusEvtStdDataSetInstance;
    instanceConfig.EvtWmiInstanceSetItem       = BusEvtStdDataSetItem;

    status = WdfWmiInstanceCreate(Device, &instanceConfig, WDF_NO_OBJECT_ATTRIBUTES, WDF_NO_HANDLE);

    if (NT_SUCCESS(status))
    {
        deviceData->StdBusjoyBusData.ErrorCount = 0;
    }

    return status;
}

NTSTATUS
BusEvtStdDataSetItem(IN WDFWMIINSTANCE WmiInstance, IN ULONG DataItemId, IN ULONG InBufferSize, IN PVOID InBuffer)
{
    PBUS_FDO_DEVICE_DATA fdoData;

    PAGED_CODE();

    fdoData = BusFdoGetData(WdfWmiInstanceGetDevice(WmiInstance));

    if (DataItemId == 2)
    {
        if (InBufferSize < sizeof(ULONG))
        {
            return STATUS_BUFFER_TOO_SMALL;
        }

        G_BusJoyDebugLevel = fdoData->StdBusjoyBusData.DebugPrintLevel = *((PULONG)InBuffer);

        return STATUS_SUCCESS;
    }

    //
    // All other fields are read only
    //
    return STATUS_WMI_READ_ONLY;
}

NTSTATUS
BusEvtStdDataSetInstance(IN WDFWMIINSTANCE WmiInstance, IN ULONG InBufferSize, IN PVOID InBuffer)
{
    PBUS_FDO_DEVICE_DATA fdoData;

    UNREFERENCED_PARAMETER(InBufferSize);

    PAGED_CODE();

    fdoData = BusFdoGetData(WdfWmiInstanceGetDevice(WmiInstance));

    G_BusJoyDebugLevel = fdoData->StdBusjoyBusData.DebugPrintLevel = ((PBUS_WMI_STD_DATA)InBuffer)->DebugPrintLevel;

    return STATUS_SUCCESS;
}

NTSTATUS
BusEvtStdDataQueryInstance(_In_ WDFWMIINSTANCE                                     WmiInstance,
                           _In_ ULONG                                              OutBufferSize,
                           _Out_writes_bytes_to_(OutBufferSize, *BufferUsed) PVOID OutBuffer,
                           _Out_ PULONG                                            BufferUsed)

{
    PBUS_FDO_DEVICE_DATA fdoData;

    UNREFERENCED_PARAMETER(OutBufferSize);

    PAGED_CODE();

    fdoData = BusFdoGetData(WdfWmiInstanceGetDevice(WmiInstance));

    *BufferUsed = sizeof(fdoData->StdBusjoyBusData);
    if (OutBufferSize < sizeof(fdoData->StdBusjoyBusData))
    {
        return STATUS_BUFFER_TOO_SMALL;
    }

    *(PBUS_WMI_STD_DATA)OutBuffer = fdoData->StdBusjoyBusData;

    return STATUS_SUCCESS;
}