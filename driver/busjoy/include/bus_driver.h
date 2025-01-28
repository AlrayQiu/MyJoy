#pragma once
#include <ntddk.h>
#include <wdf.h>
#define NTSTRSAFE_LIB
#include <initguid.h>
#include <busjoy_wmi.h>

typedef BusjoyBusInformation BUS_WMI_STD_DATA, *PBUS_WMI_STD_DATA;

typedef struct _BUS_FDO_DEVICE_DATA
{
    BUS_WMI_STD_DATA StdBusjoyBusData;
} BUS_FDO_DEVICE_DATA, *PBUS_FDO_DEVICE_DATA;
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(BUS_FDO_DEVICE_DATA, BusFdoGetData);

DRIVER_INITIALIZE         DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD BusDeviceAdd;