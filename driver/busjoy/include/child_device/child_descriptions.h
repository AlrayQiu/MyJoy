#pragma once

#include <ntddk.h>
#include <wdf.h>

typedef struct _CHILD_PDO_IDENTIFICATION_DESCRIPTION
{
    WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER Header; // should contain this header

    ULONG SerialNo;
} CHILD_PDO_IDENTIFICATION_DESCRIPTION, *PCHILD_PDO_IDENTIFICATION_DESCRIPTION;

typedef struct _PDO_DEVICE_DATA
{
    // Unique serail number of the device on the bus

    ULONG SerialNo;

} PDO_DEVICE_DATA, *PPDO_DEVICE_DATA;
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(PDO_DEVICE_DATA, GetPdoContext);

typedef struct _BUSJOY_INTERFACE_STANDARD
{
    //
    // generic interface header
    //
    USHORT                 Size;
    USHORT                 Version;
    PVOID                  Context;
    PINTERFACE_REFERENCE   InterfaceReference;
    PINTERFACE_DEREFERENCE InterfaceDereference;
    //
    // standard bus interfaces
    //
    PTRANSLATE_BUS_ADDRESS TranslateBusAddress;
    PGET_DMA_ADAPTER       GetDmaAdapter;
    PGET_SET_DEVICE_DATA   SetBusData;
    PGET_SET_DEVICE_DATA   GetBusData;

} BUSJOY_INTERFACE_STANDARD, *PBUSJOY_INTERFACE_STANDARD;